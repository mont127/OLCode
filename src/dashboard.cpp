#include "dashboard.hpp"
#include "lorea.hpp"
#include "render.hpp"
#include "types.hpp"
#include "secutil.hpp"
#include "pty_session.hpp"

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <cerrno>
#include <csignal>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <unistd.h>

namespace ocli {

namespace {

std::mutex g_agent_mutex;

struct DashboardRuntime {
    std::mutex m;
    bool initialized = false;
    bool busy = false;
    std::string status = "idle";
    std::string current;
    std::string last_error;
    int turn_id = 0;
    json messages = json::array();
};

DashboardRuntime g_dashboard;

std::string trim_copy(const std::string& s) {
    std::size_t a = 0;
    std::size_t b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

std::string lower_copy(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) out += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return out;
}

json dashboard_messages_from_agent(const LOREA& agent) {
    json msgs = json::array();
    for (std::size_t i = 0; i < agent.messages.size(); ++i) {
        std::string role = msg_role(agent.messages[i]);
        if (role != "user" && role != "assistant") continue;
        std::string content = msg_content(agent.messages[i]);
        if (content.empty()) continue;
        msgs.push_back(json{{"role", role}, {"content", content}});
    }
    return msgs;
}

void initialize_dashboard_history(LOREA& agent) {
    std::lock_guard<std::mutex> agent_lk(g_agent_mutex);
    std::lock_guard<std::mutex> dash_lk(g_dashboard.m);
    g_dashboard.messages = dashboard_messages_from_agent(agent);
    g_dashboard.initialized = true;
}

std::string lan_ipv4() {
    struct ifaddrs* ifap = nullptr;
    std::string result = "127.0.0.1";
    if (getifaddrs(&ifap) == 0) {
        for (struct ifaddrs* ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == nullptr) continue;
            if (ifa->ifa_addr->sa_family != AF_INET) continue;
            if (ifa->ifa_flags & IFF_LOOPBACK) continue;
            if (!(ifa->ifa_flags & IFF_UP)) continue;
            char ip[INET_ADDRSTRLEN] = {0};
            auto* sin = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
            if (inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip)) != nullptr) {
                std::string s(ip);
                if (!s.empty() && s != "127.0.0.1") {
                    result = s;
                    break;
                }
            }
        }
        freeifaddrs(ifap);
    }
    return result;
}

bool read_http_request(int fd, std::string& method, std::string& path, std::string& body) {
    std::string buf;
    char tmp[4096];
    std::size_t header_end = std::string::npos;
    while ((header_end = buf.find("\r\n\r\n")) == std::string::npos) {
        ssize_t n = ::recv(fd, tmp, sizeof(tmp), 0);
        if (n <= 0) return false;
        buf.append(tmp, static_cast<std::size_t>(n));
        if (buf.size() > static_cast<std::size_t>(16) * 1024 * 1024) return false;
    }

    std::string headers = buf.substr(0, header_end);
    std::size_t line_end = headers.find("\r\n");
    std::string request_line = (line_end == std::string::npos) ? headers : headers.substr(0, line_end);

    {
        std::istringstream iss(request_line);
        std::string ver;
        iss >> method >> path >> ver;
    }
    if (method.empty() || path.empty()) return false;

    std::size_t qpos = path.find('?');
    if (qpos != std::string::npos) path.erase(qpos);

    std::size_t content_length = 0;
    std::size_t pos = (line_end == std::string::npos) ? headers.size() : line_end + 2;
    while (pos < headers.size()) {
        std::size_t eol = headers.find("\r\n", pos);
        std::size_t line_stop = (eol == std::string::npos) ? headers.size() : eol;
        std::string line = headers.substr(pos, line_stop - pos);
        pos = (eol == std::string::npos) ? headers.size() : eol + 2;
        std::size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = lower_copy(trim_copy(line.substr(0, colon)));
        if (key == "content-length") {
            try {
                content_length = static_cast<std::size_t>(std::stoul(trim_copy(line.substr(colon + 1))));
            } catch (...) {
                content_length = 0;
            }
        }
    }

    body = buf.substr(header_end + 4);
    while (body.size() < content_length) {
        ssize_t n = ::recv(fd, tmp, sizeof(tmp), 0);
        if (n <= 0) break;
        body.append(tmp, static_cast<std::size_t>(n));
        if (body.size() > static_cast<std::size_t>(16) * 1024 * 1024) break;
    }
    if (body.size() > content_length) body.resize(content_length);
    return true;
}

void send_response(int fd, int status, const std::string& content_type, const std::string& body) {
    const char* reason = "OK";
    if (status == 202) reason = "Accepted";
    if (status == 400) reason = "Bad Request";
    else if (status == 409) reason = "Conflict";
    else if (status == 404) reason = "Not Found";
    else if (status == 500) reason = "Internal Server Error";

    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << ' ' << reason << "\r\n"
        << "Content-Type: " << content_type << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Cache-Control: no-store\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << body;

    std::string out = oss.str();
    std::size_t sent = 0;
    while (sent < out.size()) {
        ssize_t n = ::send(fd, out.data() + sent, out.size() - sent, 0);
        if (n <= 0) break;
        sent += static_cast<std::size_t>(n);
    }
}

void send_json(int fd, int status, const json& j) {
    send_response(fd, status, "application/json; charset=utf-8", j.dump());
}

bool parse_json_body(const std::string& body, json& out) {
    try {
        out = json::parse(body.empty() ? std::string("{}") : body);
    } catch (...) {
        return false;
    }
    return out.is_object();
}

std::string build_shared_context(const std::vector<Message>& msgs) {
    std::vector<std::string> lines;
    std::size_t start = 0;
    const std::size_t max_msgs = 20;
    if (msgs.size() > max_msgs) start = msgs.size() - max_msgs;
    for (std::size_t i = start; i < msgs.size(); ++i) {
        std::string role = msg_role(msgs[i]);
        if (role != "user" && role != "assistant") continue;
        std::string content = msg_content(msgs[i]);
        if (content.empty()) continue;
        if (content.size() > 2000) content = content.substr(0, 2000) + " ...";
        lines.push_back((role == "user" ? std::string("User: ") : std::string("Assistant: ")) + content);
    }
    std::string out;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (i) out += "\n\n";
        out += lines[i];
    }
    return out;
}

void handle_info(LOREA& agent, int fd, int port) {
    json j;
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        j["model"] = agent.model_name;
        j["backend"] = agent.backend;
        j["lan_url"] = std::string("http://") + lan_ipv4() + ":" + std::to_string(port);
        j["auto"] = agent.auto_mode;
        j["tool_access"] = agent.tool_access;
    }
    {
        std::lock_guard<std::mutex> lk(g_dashboard.m);
        j["busy"] = g_dashboard.busy;
        j["status"] = g_dashboard.status;
    }
    send_json(fd, 200, j);
}

void handle_history(LOREA& agent, int fd) {
    (void)agent;
    json out;
    {
        std::lock_guard<std::mutex> lk(g_dashboard.m);
        out = json{{"messages", g_dashboard.messages},
                   {"busy", g_dashboard.busy},
                   {"status", g_dashboard.status}};
    }
    send_json(fd, 200, out);
}

void handle_status(int fd) {
    json out;
    {
        std::lock_guard<std::mutex> lk(g_dashboard.m);
        out = json{{"busy", g_dashboard.busy},
                   {"status", g_dashboard.status},
                   {"current", g_dashboard.current},
                   {"error", g_dashboard.last_error},
                   {"turn_id", g_dashboard.turn_id}};
    }
    send_json(fd, 200, out);
}

void finish_dashboard_turn(int turn_id, const json& messages, const std::string& error) {
    std::lock_guard<std::mutex> lk(g_dashboard.m);
    if (turn_id != g_dashboard.turn_id) return;
    if (!messages.empty()) {
        g_dashboard.messages = messages;
    } else if (!error.empty()) {
        g_dashboard.messages.push_back(json{{"role", "assistant"}, {"content", error}});
    }
    g_dashboard.busy = false;
    g_dashboard.current.clear();
    g_dashboard.last_error = error;
    g_dashboard.status = error.empty() ? "idle" : "error";
}

void handle_chat(LOREA& agent, int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    std::string message = req.value("message", std::string(""));
    if (trim_copy(message).empty()) {
        send_json(fd, 400, json{{"error", "missing message"}});
        return;
    }
    bool        req_web_search = req.value("web_search", true);
    std::string req_effort     = req.value("effort", std::string("basic"));
    std::string req_workspace  = trim_copy(req.value("workspace", std::string("")));
    if (!effort_levels().count(req_effort)) req_effort = "basic";

    int turn_id = 0;
    json busy_response;
    {
        std::lock_guard<std::mutex> lk(g_dashboard.m);
        if (g_dashboard.busy) {
            busy_response = json{{"error", "agent is already working"},
                                 {"busy", true},
                                 {"turn_id", g_dashboard.turn_id}};
        } else {
            g_dashboard.busy = true;
            g_dashboard.status = "working";
            g_dashboard.current = message;
            g_dashboard.last_error.clear();
            turn_id = ++g_dashboard.turn_id;
            g_dashboard.messages.push_back(json{{"role", "user"}, {"content", message}});
        }
    }
    if (!busy_response.is_null()) {
        send_json(fd, 409, busy_response);
        return;
    }

    std::thread([&agent, message, turn_id, req_web_search, req_effort, req_workspace]() {
        std::string response_text;
        std::string log_text;
        std::string error_text;
        json display_messages = json::array();

        try {
            std::lock_guard<std::mutex> lk(g_agent_mutex);

            std::string shared = build_shared_context(agent.messages);
            agent.messages.push_back(json{{"role", "user"}, {"content", message}});

            std::string ta = (agent.tool_access == "read_only" || agent.tool_access == "full")
                                 ? agent.tool_access
                                 : std::string("full");

            bool ok = false;
            try {
                std::string dashboard_context =
                    "This request came from the OCLI web dashboard. Complete it as a single "
                    "autonomous local-agent turn. Do not stop after describing what you will do. "
                    "If the task requires workspace changes, inspect relevant files, use write_file "
                    "for edits, run focused verification when feasible, and finish with files touched "
                    "and verification results. Never output <voice_note> blocks.";
                json spec = json{{"name", "dashboard"},
                                 {"task", message},
                                 {"context", dashboard_context},
                                 {"web_search", req_web_search},
                                 {"effort", req_effort},
                                 {"workspace", req_workspace}};
                json result = agent.spawn_agent_chat(spec, shared, 240, 8, ta);
                if (result.is_object()) {
                    std::string st = result.value("status", std::string("ok"));
                    response_text = result.value("response", std::string(""));
                    log_text = result.value("log", std::string(""));
                    if (!response_text.empty() && st != "error" && st != "timeout") ok = true;
                }
            } catch (const std::exception&) {
                ok = false;
            }

            if (!ok) {
                try {
                    std::string fallback = agent.agent_completion_once(agent.messages, 0.2, 1800);
                    if (!fallback.empty()) response_text = fallback;
                } catch (const std::exception& e) {
                    if (response_text.empty()) response_text = std::string("Error: ") + e.what();
                }
            }

            if (response_text.empty()) response_text = "The agent did not produce a response.";
            agent.messages.push_back(json{{"role", "assistant"}, {"content", response_text}});
            display_messages = dashboard_messages_from_agent(agent);
            // Attach the agent's work trace to the last assistant message (display-only,
            // not fed back into the model context) so LOREA Code can surface its activity.
            if (!log_text.empty() && display_messages.is_array()) {
                std::string act = log_text;
                if (act.size() > 40000) act = act.substr(act.size() - 40000);
                for (auto it = display_messages.end(); it != display_messages.begin(); ) {
                    --it;
                    if (it->is_object() && it->value("role", std::string("")) == "assistant") {
                        (*it)["activity"] = act;
                        break;
                    }
                }
            }
        } catch (const std::exception& e) {
            error_text = std::string("Error: ") + e.what();
        } catch (...) {
            error_text = "Error: internal dashboard worker failure";
        }

        if (!error_text.empty() && display_messages.empty()) {
            display_messages = json::array();
        }
        finish_dashboard_turn(turn_id, display_messages, error_text);
        (void)log_text;
    }).detach();

    send_json(fd, 202, json{{"status", "accepted"}, {"busy", true}, {"turn_id", turn_id}});
}

void handle_exec(LOREA& agent, int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    std::string command = req.value("command", std::string(""));
    if (!command.empty()) {
        terminal_session().write_input(command + "\n");
    }
    (void)agent;
    send_json(fd, 200, json{{"output", std::string()}});
}

bool base64_decode(const std::string& in, std::string& out) {
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int T[256];
    for (int i = 0; i < 256; ++i) T[i] = -1;
    for (int i = 0; i < 64; ++i) T[static_cast<unsigned char>(chars[i])] = i;
    out.clear();
    int val = 0, bits = -8;
    for (unsigned char c : in) {
        if (c == '=') break;
        if (std::isspace(c)) continue;
        if (T[c] == -1) return false;
        val = (val << 6) + T[c];
        bits += 6;
        if (bits >= 0) {
            out.push_back(static_cast<char>((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return true;
}

// POST /api/upload  {"filename":"...", "content_base64":"..."}  (data: URL also accepted)
// Saves into <cwd>/uploads/<basename> and returns {"path","size"}.
void handle_upload(int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    std::string filename = trim_copy(req.value("filename", std::string("")));
    std::string b64 = req.value("content_base64", std::string(""));
    std::size_t comma = b64.find(',');
    if (b64.rfind("data:", 0) == 0 && comma != std::string::npos) b64 = b64.substr(comma + 1);
    if (filename.empty() || b64.empty()) {
        send_json(fd, 400, json{{"error", "missing filename or content_base64"}});
        return;
    }
    filename = std::filesystem::path(filename).filename().string();  // strip path traversal
    if (filename.empty() || filename == "." || filename == "..") {
        send_json(fd, 400, json{{"error", "invalid filename"}});
        return;
    }
    std::string bytes;
    if (!base64_decode(b64, bytes)) {
        send_json(fd, 400, json{{"error", "invalid base64"}});
        return;
    }

    namespace fs = std::filesystem;
    std::string saved;
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);   // pin cwd against a running turn
        std::error_code ec;
        fs::path dir = fs::current_path(ec) / "uploads";
        fs::create_directories(dir, ec);
        if (ec) {
            send_json(fd, 500, json{{"error", "cannot create uploads dir"}});
            return;
        }
        std::ofstream ofs(dir / filename, std::ios::binary | std::ios::trunc);
        if (!ofs) {
            send_json(fd, 500, json{{"error", "cannot write file"}});
            return;
        }
        ofs.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        saved = (dir / filename).string();
    }
    send_json(fd, 200, json{{"path", saved}, {"size", bytes.size()}});
}

// GET /api/workspace -> {"workspace": <process cwd>}
void handle_get_workspace(int fd) {
    std::string cwd;
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        std::error_code ec;
        cwd = std::filesystem::current_path(ec).string();
        if (ec) cwd = ".";
    }
    send_json(fd, 200, json{{"workspace", cwd}});
}

// POST /api/workspace  {"path":"..."} -> chdir the dashboard process + sync the shared shell.
// Subsequently spawned agent-worker turns inherit this cwd, so they operate in the workspace.
void handle_set_workspace(int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    std::string path = trim_copy(req.value("path", std::string("")));
    if (path.empty()) {
        send_json(fd, 400, json{{"error", "missing path"}});
        return;
    }
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path target = fs::canonical(path, ec);
    if (ec || !fs::is_directory(target, ec)) {
        send_json(fd, 400, json{{"error", "not a directory"}});
        return;
    }
    std::string resolved;
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        if (::chdir(target.c_str()) != 0) {
            send_json(fd, 500, json{{"error", std::string("chdir failed: ") + std::strerror(errno)}});
            return;
        }
        terminal_session().write_input(std::string("cd ") + shlex_quote(target.string()) + "\n");
        resolved = target.string();
    }
    send_json(fd, 200, json{{"workspace", resolved}});
}

// POST /api/new_chat -> reset the conversation to just the system prompt.
void handle_new_chat(LOREA& agent, int fd) {
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        if (!agent.messages.empty()) {
            json system0 = agent.messages.front();   // preserve the system prompt
            agent.messages.clear();
            agent.messages.push_back(system0);
        }
    }
    {
        std::lock_guard<std::mutex> dlk(g_dashboard.m);
        g_dashboard.messages = json::array();
        g_dashboard.current.clear();
        g_dashboard.last_error.clear();
    }
    send_json(fd, 200, json{{"ok", true}});
}

// POST /api/load_chat {"messages":[{role,content}...]} -> replace the conversation
// (used to restore a past chat from the client-side history).
void handle_load_chat(LOREA& agent, int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) { send_json(fd, 400, json{{"error", "invalid json body"}}); return; }
    json incoming = req.value("messages", json::array());
    if (!incoming.is_array()) { send_json(fd, 400, json{{"error", "messages must be an array"}}); return; }
    json restored = json::array();
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        json system0 = agent.messages.empty() ? json::object() : agent.messages.front();
        agent.messages.clear();
        if (system0.is_object() && !system0.empty()) agent.messages.push_back(system0);
        for (const auto& m : incoming) {
            if (!m.is_object()) continue;
            std::string role = m.value("role", std::string(""));
            std::string content = m.value("content", std::string(""));
            if ((role == "user" || role == "assistant") && !content.empty()) {
                agent.messages.push_back(json{{"role", role}, {"content", content}});
                restored.push_back(json{{"role", role}, {"content", content}});
            }
        }
    }
    {
        std::lock_guard<std::mutex> dlk(g_dashboard.m);
        g_dashboard.messages = restored;
    }
    send_json(fd, 200, json{{"ok", true}, {"count", restored.size()}});
}

// GET /api/connect -> current MPC connection status.
void handle_get_connect(LOREA& agent, int fd) {
    bool connected = agent.mpc_url.has_value() && !agent.mpc_url->empty();
    send_json(fd, 200, json{
        {"connected", connected},
        {"url", connected ? *agent.mpc_url : std::string("")},
        {"model", agent.model_name},
        {"streaming", connected ? agent.mpc_supports("streaming") : false}});
}

// POST /api/connect {"url","token"} -> connect to an MPC server and persist it.
void handle_connect(LOREA& agent, int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) { send_json(fd, 400, json{{"error", "invalid json body"}}); return; }
    std::string url = trim_copy(req.value("url", std::string("")));
    std::string token = trim_copy(req.value("token", std::string("")));
    if (url.empty()) { send_json(fd, 400, json{{"error", "missing url"}}); return; }
    auto nurl = agent.normalize_mpc_url(url);
    if (!nurl) { send_json(fd, 400, json{{"error", "invalid url"}}); return; }
    bool ok;
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        ok = agent.activate_mpc(*nurl, token);
        if (ok) agent.save_mpc_connection();
    }
    if (!ok) { send_json(fd, 502, json{{"error", "could not reach MPC server at that URL/token"}}); return; }
    send_json(fd, 200, json{
        {"connected", true},
        {"url", agent.mpc_url.value_or("")},
        {"model", agent.model_name},
        {"streaming", agent.mpc_supports("streaming")}});
}

// POST /api/disconnect -> drop the MPC connection and clear the saved file.
void handle_disconnect(LOREA& agent, int fd) {
    {
        std::lock_guard<std::mutex> lk(g_agent_mutex);
        agent.disconnect_mpc();
    }
    send_json(fd, 200, json{{"connected", false}, {"model", agent.model_name}});
}

bool term_send_all(int fd, const char* data, std::size_t len) {
    std::size_t sent = 0;
    while (sent < len) {
        ssize_t n = ::send(fd, data + sent, len - sent, 0);
        if (n <= 0) return false;
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

bool term_send_event(int fd, const std::string& data) {
    std::string ev;
    std::string payload = json(data).dump();
    ev.reserve(payload.size() + 16);
    ev += "data: ";
    ev += payload;
    ev += "\n\n";
    return term_send_all(fd, ev.data(), ev.size());
}

std::string strip_terminal_wheel_reports(const std::string& data) {
    std::string out;
    out.reserve(data.size());
    for (std::size_t i = 0; i < data.size();) {
        if (static_cast<unsigned char>(data[i]) == 0x1b && i + 2 < data.size() && data[i + 1] == '[') {
            // SGR mouse mode: ESC [ < button ; x ; y M/m. Wheel buttons set bit 64.
            if (data[i + 2] == '<') {
                std::size_t j = i + 3;
                unsigned button = 0;
                bool any_digit = false;
                while (j < data.size() && std::isdigit(static_cast<unsigned char>(data[j]))) {
                    any_digit = true;
                    button = button * 10u + static_cast<unsigned>(data[j] - '0');
                    ++j;
                }
                if (any_digit && j < data.size() && data[j] == ';') {
                    std::size_t k = j + 1;
                    while (k < data.size() && data[k] != 'M' && data[k] != 'm') ++k;
                    if (k < data.size() && (button & 64u)) {
                        i = k + 1;
                        continue;
                    }
                }
            }
            // X10 mouse mode: ESC [ M cb cx cy. Wheel buttons also set bit 64.
            if (data[i + 2] == 'M' && i + 5 < data.size()) {
                unsigned button = static_cast<unsigned char>(data[i + 3]);
                if (button >= 32u) button -= 32u;
                if (button & 64u) {
                    i += 6;
                    continue;
                }
            }
        }
        out.push_back(data[i]);
        ++i;
    }
    return out;
}

void handle_term_stream(int fd) {
    terminal_session().start();

    static const char* kHeaders =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/event-stream\r\n"
        "Cache-Control: no-store\r\n"
        "Connection: keep-alive\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "X-Accel-Buffering: no\r\n"
        "\r\n";
    if (!term_send_all(fd, kHeaders, std::strlen(kHeaders))) return;

    std::string snap = terminal_session().snapshot();
    std::size_t cursor = 0;
    terminal_session().read_since(cursor);
    if (!snap.empty()) {
        if (!term_send_event(fd, snap)) return;
    }

    for (;;) {
        std::string chunk = terminal_session().read_since(cursor);
        if (!chunk.empty()) {
            if (!term_send_event(fd, chunk)) return;
        } else {
            if (!term_send_all(fd, ":\n\n", 3)) return;
            if (!terminal_session().alive()) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void handle_term_input(int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    std::string data = req.value("data", std::string(""));
    data = strip_terminal_wheel_reports(data);
    if (!data.empty()) terminal_session().write_input(data);
    send_json(fd, 200, json{{"ok", true}});
}

void handle_term_resize(int fd, const std::string& body) {
    json req;
    if (!parse_json_body(body, req)) {
        send_json(fd, 400, json{{"error", "invalid json body"}});
        return;
    }
    int rows = req.value("rows", 24);
    int cols = req.value("cols", 80);
    if (rows < 1) rows = 1;
    if (cols < 1) cols = 1;
    terminal_session().resize(rows, cols);
    send_json(fd, 200, json{{"ok", true}});
}

void route(LOREA& agent, int fd, const std::string& method,
           const std::string& path, const std::string& body, int port) {
    if (method == "GET" && (path == "/" || path == "/index.html")) {
        send_response(fd, 200, "text/html; charset=utf-8",
                      DASHBOARD_HTML != nullptr ? std::string(DASHBOARD_HTML) : std::string());
        return;
    }
    if (method == "GET" && path == "/vendor/xterm.js") {
        send_response(fd, 200, "application/javascript; charset=utf-8",
                      XTERM_JS != nullptr ? std::string(XTERM_JS) : std::string());
        return;
    }
    if (method == "GET" && path == "/vendor/xterm.css") {
        send_response(fd, 200, "text/css; charset=utf-8",
                      XTERM_CSS != nullptr ? std::string(XTERM_CSS) : std::string());
        return;
    }
    if (method == "GET" && path == "/vendor/xterm-addon-fit.js") {
        send_response(fd, 200, "application/javascript; charset=utf-8",
                      XTERM_FIT_JS != nullptr ? std::string(XTERM_FIT_JS) : std::string());
        return;
    }
    if (method == "GET" && path == "/api/info") {
        handle_info(agent, fd, port);
        return;
    }
    if (method == "GET" && path == "/api/history") {
        handle_history(agent, fd);
        return;
    }
    if (method == "GET" && path == "/api/status") {
        handle_status(fd);
        return;
    }
    if (method == "POST" && path == "/api/chat") {
        handle_chat(agent, fd, body);
        return;
    }
    if (method == "POST" && path == "/api/exec") {
        handle_exec(agent, fd, body);
        return;
    }
    if (method == "GET" && path == "/api/term/stream") {
        handle_term_stream(fd);
        return;
    }
    if (method == "POST" && path == "/api/term/input") {
        handle_term_input(fd, body);
        return;
    }
    if (method == "POST" && path == "/api/term/resize") {
        handle_term_resize(fd, body);
        return;
    }
    if (method == "POST" && path == "/api/upload") {
        handle_upload(fd, body);
        return;
    }
    if (method == "POST" && path == "/api/workspace") {
        handle_set_workspace(fd, body);
        return;
    }
    if (method == "GET" && path == "/api/workspace") {
        handle_get_workspace(fd);
        return;
    }
    if (method == "POST" && path == "/api/new_chat") {
        handle_new_chat(agent, fd);
        return;
    }
    if (method == "POST" && path == "/api/load_chat") {
        handle_load_chat(agent, fd, body);
        return;
    }
    if (method == "GET" && path == "/api/connect") {
        handle_get_connect(agent, fd);
        return;
    }
    if (method == "POST" && path == "/api/connect") {
        handle_connect(agent, fd, body);
        return;
    }
    if (method == "POST" && path == "/api/disconnect") {
        handle_disconnect(agent, fd);
        return;
    }
    if (method == "OPTIONS") {
        send_response(fd, 200, "text/plain", std::string());
        return;
    }
    send_json(fd, 404, json{{"error", "not found"}});
}

void handle_client(LOREA& agent, int fd, int port) {
#ifdef SO_NOSIGPIPE
    // macOS/BSD: suppress SIGPIPE per-socket. On Linux this option doesn't exist;
    // serve() ignores SIGPIPE process-wide instead, so writes just return EPIPE.
    int set = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set));
#endif
    std::string method;
    std::string path;
    std::string body;
    if (read_http_request(fd, method, path, body)) {
        try {
            route(agent, fd, method, path, body, port);
        } catch (const std::exception& e) {
            send_json(fd, 500, json{{"error", std::string(e.what())}});
        } catch (...) {
            send_json(fd, 500, json{{"error", "internal error"}});
        }
    }
    ::close(fd);
}

void serve(LOREA& agent, int port) {
    ::signal(SIGPIPE, SIG_IGN);

    int srv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) {
        log_warn("dashboard: socket() failed");
        return;
    }

    int opt = 1;
    ::setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(srv, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        log_warn(std::string("dashboard: bind failed on port ") + std::to_string(port));
        ::close(srv);
        return;
    }
    if (::listen(srv, 16) < 0) {
        log_warn("dashboard: listen failed");
        ::close(srv);
        return;
    }

    std::string lan_url = std::string("http://") + lan_ipv4() + ":" + std::to_string(port);
    log_info("Dashboard live at " + lan_url);

    for (;;) {
        struct sockaddr_in cli;
        socklen_t cli_len = sizeof(cli);
        int fd = ::accept(srv, reinterpret_cast<struct sockaddr*>(&cli), &cli_len);
        if (fd < 0) {
            if (errno == EINTR) continue;
            break;
        }
        std::thread(handle_client, std::ref(agent), fd, port).detach();
    }

    ::close(srv);
}

bool can_bind_dashboard_port(int port) {
    int srv = ::socket(AF_INET, SOCK_STREAM, 0);
    if (srv < 0) return false;

    int opt = 1;
    ::setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    bool ok = (::bind(srv, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == 0);
    ::close(srv);
    return ok;
}

}

bool start_dashboard(LOREA& agent, int port) {
    if (!can_bind_dashboard_port(port)) {
        log_warn(std::string("dashboard: bind failed on port ") + std::to_string(port));
        return false;
    }
    terminal_session();
    g_shared_terminal_active = true;
    initialize_dashboard_history(agent);
    std::thread([&agent, port]() {
        serve(agent, port);
    }).detach();
    return true;
}

}
