# Graph Report - ocli-cpp  (2026-07-10)

## Corpus Check
- 50 files · ~183,412 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 911 nodes · 4120 edges · 20 communities detected
- Extraction: 50% EXTRACTED · 50% INFERRED · 0% AMBIGUOUS · INFERRED: 2044 edges (avg confidence: 0.8)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- [[_COMMUNITY_Community 0|Community 0]]
- [[_COMMUNITY_Community 1|Community 1]]
- [[_COMMUNITY_Community 2|Community 2]]
- [[_COMMUNITY_Community 3|Community 3]]
- [[_COMMUNITY_Community 4|Community 4]]
- [[_COMMUNITY_Community 5|Community 5]]
- [[_COMMUNITY_Community 6|Community 6]]
- [[_COMMUNITY_Community 7|Community 7]]
- [[_COMMUNITY_Community 8|Community 8]]
- [[_COMMUNITY_Community 9|Community 9]]
- [[_COMMUNITY_Community 10|Community 10]]
- [[_COMMUNITY_Community 11|Community 11]]
- [[_COMMUNITY_Community 12|Community 12]]
- [[_COMMUNITY_Community 13|Community 13]]
- [[_COMMUNITY_Community 14|Community 14]]
- [[_COMMUNITY_Community 15|Community 15]]
- [[_COMMUNITY_Community 16|Community 16]]
- [[_COMMUNITY_Community 17|Community 17]]
- [[_COMMUNITY_Community 18|Community 18]]
- [[_COMMUNITY_Community 26|Community 26]]

## God Nodes (most connected - your core abstractions)
1. `empty()` - 221 edges
2. `size()` - 218 edges
3. `push_back()` - 152 edges
4. `process_chat()` - 110 edges
5. `find()` - 106 edges
6. `run()` - 92 edges
7. `end()` - 79 edges
8. `to_string()` - 77 edges
9. `is_object()` - 76 edges
10. `begin()` - 57 edges

## Surprising Connections (you probably didn't know these)
- `run()` --calls--> `get()`  [INFERRED]
  search_backend.py → third_party/json.hpp
- `emit()` --calls--> `compose()`  [INFERRED]
  nvidia_backend.py → src/live_view.cpp
- `resolve_key()` --calls--> `get()`  [INFERRED]
  nvidia_backend.py → third_party/json.hpp
- `resolve_key()` --calls--> `strip()`  [INFERRED]
  nvidia_backend.py → src/lorea_mpc.cpp
- `load_request()` --calls--> `strip()`  [INFERRED]
  nvidia_backend.py → src/lorea_mpc.cpp

## Communities

### Community 0 - "Community 0"
Cohesion: 0.04
Nodes (101): cookie_names(), header_cb(), accept(), at(), back(), basic_json(), begin(), binary() (+93 more)

### Community 1 - "Community 1"
Cohesion: 0.05
Nodes (103): can_use_terminal_keys(), celebrate(), center_pad(), clean_ansi(), clean_len(), frame_bottom(), frame_title(), gradient_text() (+95 more)

### Community 2 - "Community 2"
Cohesion: 0.06
Nodes (98): kv_row(), mode_value(), print_panel(), status_label(), empty(), live_end(), pty_pane_lines(), sync_context_budget() (+90 more)

### Community 3 - "Community 3"
Cohesion: 0.06
Nodes (76): build_query(), content_type(), do_request(), header(), http_perform(), http_stream(), HttpClient(), iequal() (+68 more)

### Community 4 - "Community 4"
Cohesion: 0.1
Nodes (68): get_impl_ptr(), is_array(), is_boolean(), is_null(), is_number(), is_number_float(), is_number_integer(), is_number_unsigned() (+60 more)

### Community 5 - "Community 5"
Cohesion: 0.12
Nodes (55): array(), menu_choice(), theme_command(), activate_mpc(), apply_mpc_selection(), auto_reconnect_mpc(), basename_of(), cancel_mpc_download() (+47 more)

### Community 6 - "Community 6"
Cohesion: 0.09
Nodes (53): json_sax_dom_callback_parser, parse(), push_back(), arr_of(), capitalize(), contains(), drop_last_cp(), endswith() (+45 more)

### Community 7 - "Community 7"
Cohesion: 0.13
Nodes (54): utf8_len(), object(), rbegin(), rend(), advr_reminder_message(), ascii_lower(), ascii_upper(), collapse_ws() (+46 more)

### Community 8 - "Community 8"
Cohesion: 0.08
Nodes (48): cleanup(), after_last_colon(), after_slashslash(), before_first_colon(), ends_with(), ensure_llamacpp_server(), ensure_local_server(), ensure_lorea() (+40 more)

### Community 9 - "Community 9"
Cohesion: 0.1
Nodes (48): base64_decode(), build_shared_context(), can_bind_dashboard_port(), dashboard_messages_from_agent(), finish_dashboard_turn(), handle_chat(), handle_client(), handle_connect() (+40 more)

### Community 10 - "Community 10"
Cohesion: 0.1
Nodes (39): adjust_scroll(), append_pending_input(), apply_sticky_scroll(), blink_on(), clamp_scrolls_unlocked(), clip_visible(), collapse_cr(), compose() (+31 more)

### Community 11 - "Community 11"
Cohesion: 0.14
Nodes (14): BaseHTTPRequestHandler, decode(), split(), startswith(), _client(), Handler, _list_models(), _openai_messages() (+6 more)

### Community 12 - "Community 12"
Cohesion: 0.15
Nodes (25): realpath_str(), ascii_lower(), basename_of(), check_path_safety(), classify_offensive(), classify_v4(), classify_v6(), domain_matches() (+17 more)

### Community 13 - "Community 13"
Cohesion: 0.32
Nodes (10): format_local(), print_sessions(), python_round(), session_menu(), ends_with(), estimate_tokens(), list_saved_sessions(), path_join() (+2 more)

### Community 14 - "Community 14"
Cohesion: 0.22
Nodes (8): divide(), multiply(), subtract(), add(), test_add(), test_divide(), test_multiply(), test_subtract()

### Community 15 - "Community 15"
Cohesion: 0.67
Nodes (1): InterruptionManager

### Community 16 - "Community 16"
Cohesion: 1.0
Nodes (1): LOREA

### Community 17 - "Community 17"
Cohesion: 1.0
Nodes (1): LOREA

### Community 18 - "Community 18"
Cohesion: 1.0
Nodes (1): Subprocess

### Community 26 - "Community 26"
Cohesion: 1.0
Nodes (1): Pass OCLI's role/content messages straight through; coerce tool rows to user.

## Knowledge Gaps
- **10 isolated node(s):** `Pass OCLI's role/content messages straight through; coerce tool rows to user.`, `LOREA`, `LOREA`, `Subprocess`, `invalid_iterator` (+5 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **Thin community `Community 15`** (3 nodes): `interrupt.hpp`, `InterruptionManager`, `.InterruptionManager()`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 16`** (2 nodes): `LOREA`, `dashboard.hpp`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 17`** (2 nodes): `lorea.hpp`, `LOREA`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 18`** (2 nodes): `secutil.hpp`, `Subprocess`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.
- **Thin community `Community 26`** (1 nodes): `Pass OCLI's role/content messages straight through; coerce tool rows to user.`
  Too small to be a meaningful cluster - may be noise or needs more connections extracted.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `size()` connect `Community 1` to `Community 0`, `Community 2`, `Community 3`, `Community 4`, `Community 5`, `Community 6`, `Community 7`, `Community 8`, `Community 9`, `Community 10`, `Community 11`, `Community 13`?**
  _High betweenness centrality (0.220) - this node is a cross-community bridge._
- **Why does `empty()` connect `Community 2` to `Community 0`, `Community 1`, `Community 3`, `Community 4`, `Community 5`, `Community 6`, `Community 7`, `Community 8`, `Community 9`, `Community 10`, `Community 11`, `Community 12`, `Community 13`?**
  _High betweenness centrality (0.203) - this node is a cross-community bridge._
- **Why does `push_back()` connect `Community 6` to `Community 0`, `Community 1`, `Community 2`, `Community 3`, `Community 4`, `Community 5`, `Community 7`, `Community 8`, `Community 9`, `Community 10`, `Community 11`, `Community 12`, `Community 13`, `Community 14`?**
  _High betweenness centrality (0.118) - this node is a cross-community bridge._
- **Are the 213 inferred relationships involving `empty()` (e.g. with `jtruthy()` and `run_spawn_agent_worker()`) actually correct?**
  _`empty()` has 213 INFERRED edges - model-reasoned connections that need verification._
- **Are the 201 inferred relationships involving `size()` (e.g. with `strip()` and `run_spawn_agent_worker()`) actually correct?**
  _`size()` has 201 INFERRED edges - model-reasoned connections that need verification._
- **Are the 131 inferred relationships involving `push_back()` (e.g. with `run_spawn_agent_worker()` and `render_text()`) actually correct?**
  _`push_back()` has 131 INFERRED edges - model-reasoned connections that need verification._
- **Are the 84 inferred relationships involving `process_chat()` (e.g. with `run_spawn_agent_worker()` and `count()`) actually correct?**
  _`process_chat()` has 84 INFERRED edges - model-reasoned connections that need verification._