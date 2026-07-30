// The dashboard web UI (HTML/CSS/JS) embedded as a C++ raw string literal.

#include "dashboard.hpp"

namespace ocli {

const char* DASHBOARD_HTML = R"DASH(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="color-scheme" content="dark">
<title>LOREA</title>
<link rel="stylesheet" href="/vendor/xterm.css">
<script src="/vendor/xterm.js"></script>
<script src="/vendor/xterm-addon-fit.js"></script>
<style>
  :root{
    --bg:#0A0C0B; --panel:#0E1211; --panel-2:#121716; --elevated:#161C1B;
    --hair:rgba(255,255,255,.06); --hair-2:rgba(255,255,255,.09);
    --text:#ECF1EF; --muted:#9BA8A4; --faint:#7d8a85;
    --accent:#3FE08A; --accent-2:#22C55E; --accent-dim:rgba(63,224,138,.12); --danger:#F0616D;
    --radius:12px; --radius-sm:8px;
    --ease:cubic-bezier(.16,1,.3,1);
    --ui:Inter,system-ui,-apple-system,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
    --mono:"JetBrains Mono",ui-monospace,"SF Mono",Menlo,Consolas,monospace;
    --sidebar-w:260px;
  }
  *{box-sizing:border-box}
  html,body{height:100%;margin:0;overflow:hidden}
  .app{height:100dvh}
  body{
    margin:0; background:var(--bg); color:var(--text);
    font-family:var(--ui); font-size:14.5px; line-height:1.6; font-weight:400;
    -webkit-font-smoothing:antialiased; text-rendering:optimizeLegibility;
    overflow:hidden; letter-spacing:0;
  }
  button{font-family:inherit; color:inherit; letter-spacing:inherit}
  input,textarea,select{font-family:inherit}
  ::selection{background:rgba(63,224,138,.26)}
  svg{display:block}
  a{color:var(--accent); text-underline-offset:2px}
  .sr-only{position:absolute;width:1px;height:1px;padding:0;margin:-1px;overflow:hidden;clip:rect(0 0 0 0);white-space:nowrap;border:0}
  :focus-visible{outline:2px solid var(--accent);outline-offset:2px;border-radius:6px}
  button,[role="button"],select,summary{cursor:pointer}

  /* scrollbars */
  *::-webkit-scrollbar{width:10px;height:10px}
  *::-webkit-scrollbar-thumb{background:rgba(255,255,255,.08);border-radius:8px;border:2px solid transparent;background-clip:content-box}
  *::-webkit-scrollbar-thumb:hover{background:rgba(255,255,255,.15);background-clip:content-box}
  *::-webkit-scrollbar-track{background:transparent}

  .dot{width:6px;height:6px;border-radius:50%;background:var(--accent);flex:0 0 auto;box-shadow:0 0 0 3px var(--accent-dim);animation:pulse 2.2s var(--ease) infinite}
  @keyframes pulse{0%,100%{opacity:1}50%{opacity:.45}}

  /* ===================== app shell ===================== */
  .app{display:flex;height:100%}

  /* -------- sidebar -------- */
  .sidebar{
    width:var(--sidebar-w);flex:0 0 var(--sidebar-w);height:100%;
    background:var(--panel);border-right:1px solid var(--hair);
    display:flex;flex-direction:column;min-height:0;
  }
  .sb-head{display:flex;align-items:center;gap:10px;padding:16px 16px 12px}
  .mark{object-fit:contain;width:30px;height:30px;flex:0 0 auto;border-radius:8px}
  .sb-word{font-weight:600;font-size:15px;letter-spacing:.02em;color:var(--text)}
  .sb-build{margin-left:auto;font-family:var(--mono);font-size:10px;font-weight:600;letter-spacing:.09em;text-transform:uppercase;
    color:var(--faint);border:1px solid var(--hair);border-radius:999px;padding:2px 7px}

  .sb-new{
    margin:2px 12px 10px;display:flex;align-items:center;gap:9px;
    background:var(--panel-2);border:1px solid var(--hair-2);color:var(--text);
    font-size:13.5px;font-weight:550;padding:9px 12px;border-radius:var(--radius-sm);width:calc(100% - 24px);
    transition:border-color .16s var(--ease),background .16s var(--ease);
  }
  .sb-new:hover{border-color:rgba(63,224,138,.35);background:var(--elevated)}
  .sb-new svg{width:16px;height:16px;color:var(--accent)}

  .sb-label{padding:6px 18px 6px;font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:.07em;color:var(--faint);
    display:flex;align-items:center;justify-content:space-between}
  .sb-chats{flex:1 1 auto;min-height:0;overflow-y:auto;padding:0 8px 8px}
  .sb-empty{padding:10px 10px;color:var(--faint);font-size:13px}

  .chat-row{position:relative;display:flex;align-items:center;gap:8px;padding:8px 10px 8px 12px;border-radius:var(--radius-sm);
    transition:background .14s var(--ease);margin-bottom:1px}
  .chat-row:hover{background:var(--panel-2)}
  .chat-row.is-active{background:var(--accent-dim)}
  .chat-row.is-active::before{content:"";position:absolute;left:0;top:7px;bottom:7px;width:2px;border-radius:2px;background:var(--accent)}
  .chat-meta{flex:1 1 auto;min-width:0;display:flex;flex-direction:column;gap:1px}
  .chat-title{font-size:13px;line-height:1.35;color:var(--text);white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
  .chat-row.is-active .chat-title{color:var(--text)}
  .chat-time{font-size:11px;color:var(--faint);font-variant-numeric:tabular-nums}
  .chat-del{position:absolute;right:6px;top:50%;transform:translateY(-50%);display:inline-flex;align-items:center;justify-content:center;
    width:24px;height:24px;border:0;background:transparent;color:var(--muted);border-radius:6px;opacity:0;transition:opacity .14s var(--ease),background .14s var(--ease),color .14s var(--ease)}
  .chat-row:hover .chat-del,.chat-del:focus-visible{opacity:1}
  .chat-del:hover{background:rgba(255,255,255,.07);color:var(--danger)}
  .chat-del svg{width:14px;height:14px}

  /* -------- sidebar tab switcher (pinned) -------- */
  .sb-nav{flex:0 0 auto;border-top:1px solid var(--hair);padding:8px;display:flex;flex-direction:column;gap:2px}
  .nav-tab{display:flex;align-items:center;gap:11px;padding:9px 11px;border:0;background:transparent;color:var(--muted);
    font-size:13.5px;font-weight:550;border-radius:var(--radius-sm);text-align:left;transition:color .14s var(--ease),background .14s var(--ease)}
  .nav-tab svg{width:17px;height:17px;flex:0 0 auto}
  .nav-tab:hover{color:var(--text);background:var(--panel-2)}
  .nav-tab.is-active{color:var(--accent);background:var(--accent-dim)}
  .nav-tab .kb{margin-left:auto;font-family:var(--mono);font-size:10.5px;color:var(--faint)}
  .nav-tab.is-active .kb{color:rgba(63,224,138,.7)}

  /* -------- main -------- */
  .main{flex:1 1 auto;min-width:0;height:100%;position:relative}
  .view{position:absolute;inset:0;display:none;flex-direction:column;min-height:0}
  .view.is-active{display:flex}

  .view-top{flex:0 0 auto;min-height:52px;display:flex;align-items:center;justify-content:space-between;gap:14px;
    padding:0 22px;border-bottom:1px solid var(--hair)}
  .vt-left{display:flex;flex-direction:column;gap:1px;min-width:0}
  .vt-title{margin:0;font-size:14px;font-weight:600;letter-spacing:-.01em;color:var(--text)}
  .vt-eyebrow{font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:.07em;color:var(--faint)}
  .vt-right{display:flex;align-items:center;gap:12px;min-width:0}
  .ws-path{font-family:var(--mono);font-size:12.5px;color:var(--muted);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;max-width:52vw}
  .ws-path.is-set{color:var(--text)}
  .work-pill{display:inline-flex;align-items:center;gap:8px;font-family:var(--mono);font-size:11.5px;color:var(--muted)}

  .ghost-btn{display:inline-flex;align-items:center;gap:8px;background:var(--panel-2);border:1px solid var(--hair-2);color:var(--text);
    font-size:13px;font-weight:550;padding:7px 12px;border-radius:var(--radius-sm);white-space:nowrap;
    transition:border-color .15s var(--ease),background .15s var(--ease)}
  .ghost-btn:hover{border-color:rgba(63,224,138,.35);background:var(--elevated)}
  .ghost-btn svg{width:15px;height:15px;color:var(--muted)}
  .primary-btn{display:inline-flex;align-items:center;gap:8px;background:var(--accent);border:0;color:#04140A;
    font-size:13px;font-weight:600;padding:8px 14px;border-radius:var(--radius-sm);white-space:nowrap;
    transition:background .15s var(--ease)}
  .primary-btn:hover{background:#54e69a}
  .primary-btn svg{width:15px;height:15px}

  /* -------- conversation surface -------- */
  .conv-scroll{flex:1 1 auto;min-height:0;overflow-y:auto;overflow-x:hidden}
  .conv-col{max-width:min(1040px,88%);margin:0 auto;padding:26px 32px 32px}
  .msg-list{display:flex;flex-direction:column;gap:22px}

  .empty{padding:6px 0 0;color:var(--muted)}
  .empty h2{margin:0 0 6px;font-size:18px;font-weight:600;letter-spacing:-.3px;color:var(--text)}
  .empty p{margin:0;font-size:14px;color:var(--muted);max-width:46ch}

  .msg-user{align-self:flex-end;max-width:82%}
  .msg-user .bubble{background:var(--accent-dim);border:1px solid rgba(63,224,138,.16);border-radius:var(--radius);
    padding:9px 14px;font-size:14.5px;color:var(--text)}
  .msg-assistant{align-self:stretch;max-width:100%}
  .assistant-content{font-size:14.5px;line-height:1.65;color:var(--text)}

  /* markdown */
  .assistant-content>*:first-child,.bubble>*:first-child{margin-top:0}
  .assistant-content>*:last-child,.bubble>*:last-child{margin-bottom:0}
  .assistant-content p,.bubble p{margin:0 0 12px}
  .assistant-content h1,.assistant-content h2,.assistant-content h3,.assistant-content h4{
    margin:20px 0 10px;line-height:1.3;font-weight:600;letter-spacing:-.01em}
  .assistant-content h1{font-size:1.34em}.assistant-content h2{font-size:1.18em}.assistant-content h3{font-size:1.05em}
  .assistant-content ul,.assistant-content ol,.bubble ul,.bubble ol{margin:0 0 12px;padding-left:1.35em}
  .assistant-content li,.bubble li{margin:4px 0}
  .assistant-content blockquote,.bubble blockquote{margin:0 0 12px;padding:2px 0 2px 14px;border-left:2px solid var(--hair-2);color:var(--muted)}
  code{font-family:var(--mono);font-size:.9em}
  .assistant-content :not(pre)>code,.bubble :not(pre)>code{
    background:var(--panel-2);border:1px solid var(--hair);border-radius:6px;padding:1px 5px;color:var(--accent);font-size:.86em}
  .code-block{margin:0 0 14px;border:1px solid var(--hair-2);border-radius:10px;overflow:hidden;background:#0B0F0D}
  .code-head{display:flex;align-items:center;justify-content:space-between;padding:6px 8px 6px 13px;background:var(--panel-2);border-bottom:1px solid var(--hair)}
  .code-lang{font-family:var(--mono);font-size:11px;color:var(--muted);text-transform:lowercase;letter-spacing:.03em}
  .copy-btn{display:inline-flex;align-items:center;gap:6px;background:transparent;border:1px solid var(--hair-2);color:var(--muted);
    font-size:11px;padding:4px 9px;border-radius:7px;transition:color .15s var(--ease),border-color .15s var(--ease)}
  .copy-btn:hover{color:var(--text);border-color:rgba(255,255,255,.18)}
  .copy-btn.copied{color:var(--accent);border-color:rgba(63,224,138,.4)}
  .copy-btn svg{width:13px;height:13px}
  .code-block pre{margin:0;padding:12px 14px;overflow-x:auto}
  .code-block code{color:#d6e2df;font-family:var(--mono);font-size:12.5px;line-height:1.55}

  /* agent activity disclosure */
  .activity{margin-top:10px;border:1px solid var(--hair);border-radius:var(--radius-sm);background:var(--panel)}
  .activity summary{list-style:none;display:flex;align-items:center;gap:7px;padding:7px 11px;font-size:12px;font-weight:550;color:var(--muted)}
  .activity summary::-webkit-details-marker{display:none}
  .activity summary:hover{color:var(--text)}
  .act-caret{display:inline-flex;transition:transform .16s var(--ease)}
  .act-caret svg{width:13px;height:13px}
  .activity[open] .act-caret{transform:rotate(90deg)}
  .act-body{margin:0;padding:10px 13px;border-top:1px solid var(--hair);max-height:280px;overflow:auto;
    font-family:var(--mono);font-size:11.5px;line-height:1.5;color:var(--muted);white-space:pre-wrap;word-break:break-word}

  /* thinking */
  .thinking{display:inline-flex;align-items:center;gap:8px;color:var(--muted);font-size:14px;padding:2px 0}
  .thinking .tdot{width:6px;height:6px;border-radius:50%;background:var(--muted);opacity:.5;animation:bounce 1.3s infinite var(--ease)}
  .thinking .tdot:nth-child(2){animation-delay:.16s}.thinking .tdot:nth-child(3){animation-delay:.32s}
  @keyframes bounce{0%,60%,100%{transform:translateY(0);opacity:.4}30%{transform:translateY(-4px);opacity:.9}}

  /* -------- composer -------- */
  .composer-wrap{flex:0 0 auto;background:linear-gradient(180deg,transparent,var(--bg) 44%)}
  .composer-col{max-width:min(1040px,88%);margin:0 auto;padding:6px 32px 18px}
  .composer{background:var(--panel-2);border:1px solid var(--hair-2);border-radius:var(--radius);padding:9px 9px 7px;
    transition:border-color .16s var(--ease),box-shadow .16s var(--ease)}
  .composer:focus-within{border-color:rgba(63,224,138,.4);box-shadow:0 0 0 3px var(--accent-dim)}
  .chips{display:flex;flex-wrap:wrap;gap:7px;padding:2px 3px 7px}
  .chips:empty{display:none}
  .chip{display:inline-flex;align-items:center;gap:6px;background:var(--elevated);border:1px solid var(--hair-2);
    border-radius:999px;padding:4px 5px 4px 11px;font-size:12px;color:var(--text);max-width:220px}
  .chip .chip-name{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
  .chip-x{display:inline-flex;background:transparent;border:0;color:var(--muted);padding:2px;border-radius:6px}
  .chip-x:hover{color:var(--text);background:rgba(255,255,255,.07)}
  .chip-x svg{width:12px;height:12px}
  .composer-input{width:100%;resize:none;border:0;background:transparent;color:var(--text);
    font-size:14.5px;line-height:1.55;padding:5px 7px 7px;max-height:200px;overflow-y:auto}
  .composer-input::placeholder{color:var(--faint)}
  .composer-input:focus{outline:none}
  .composer-tools{display:flex;align-items:center;justify-content:space-between;gap:10px;padding:3px 2px 1px}
  .tools-left{display:flex;align-items:center;gap:7px;flex-wrap:wrap}

  .icon-btn{display:inline-flex;align-items:center;justify-content:center;width:32px;height:32px;border-radius:var(--radius-sm);
    background:transparent;border:1px solid var(--hair-2);color:var(--muted);
    transition:color .14s var(--ease),border-color .14s var(--ease),background .14s var(--ease)}
  .icon-btn:hover{color:var(--text);border-color:rgba(255,255,255,.18);background:var(--elevated)}
  .icon-btn svg{width:17px;height:17px}
  .toggle-chip{display:inline-flex;align-items:center;gap:7px;height:32px;padding:0 11px;border-radius:var(--radius-sm);
    background:transparent;border:1px solid var(--hair-2);color:var(--muted);font-size:12.5px;font-weight:550;
    transition:color .14s var(--ease),border-color .14s var(--ease),background .14s var(--ease)}
  .toggle-chip svg{width:15px;height:15px}
  .toggle-chip:hover{color:var(--text);border-color:rgba(255,255,255,.18)}
  .toggle-chip.is-on{color:var(--accent);border-color:rgba(63,224,138,.4);background:var(--accent-dim)}
  .effort-field{position:relative;display:inline-flex}
  .effort-select{appearance:none;height:32px;padding:0 28px 0 11px;border-radius:var(--radius-sm);background:transparent;
    border:1px solid var(--hair-2);color:var(--muted);font-size:12.5px;font-weight:550;
    transition:color .14s var(--ease),border-color .14s var(--ease)}
  .effort-select:hover{color:var(--text);border-color:rgba(255,255,255,.18)}
  .effort-field::after{content:"";position:absolute;right:10px;top:50%;width:6px;height:6px;border-right:1.6px solid var(--muted);
    border-bottom:1.6px solid var(--muted);transform:translateY(-70%) rotate(45deg);pointer-events:none}
  .effort-select option{background:var(--elevated);color:var(--text)}

  .send-btn{display:inline-flex;align-items:center;justify-content:center;width:36px;height:36px;border-radius:50%;
    background:var(--accent);border:0;color:#04140A;flex:0 0 auto;
    transition:transform .14s var(--ease),background .14s var(--ease)}
  .send-btn svg{width:18px;height:18px}
  .send-btn:hover{background:#54e69a;transform:translateY(-1px)}
  .send-btn:active{transform:translateY(0)}
  .send-btn:disabled{background:var(--elevated);color:var(--faint);cursor:not-allowed;transform:none}

  /* -------- space notice + input -------- */
  .ws-notice{max-width:480px;border:1px solid var(--hair-2);border-radius:var(--radius);background:var(--panel-2);padding:18px 20px}
  .ws-notice .notice-title{font-size:15px;font-weight:600;letter-spacing:-.01em;margin-bottom:5px}
  .ws-notice p{margin:0 0 14px;color:var(--muted);font-size:13.5px}
  .ws-input-row{flex:0 0 auto;display:flex;gap:8px;align-items:center;padding:10px 22px;border-bottom:1px solid var(--hair);background:var(--panel)}
  .text-input{flex:1 1 auto;background:var(--bg);border:1px solid var(--hair-2);border-radius:var(--radius-sm);color:var(--text);
    font-family:var(--mono);font-size:12.5px;padding:8px 11px}
  .text-input:focus{outline:none;border-color:rgba(63,224,138,.4);box-shadow:0 0 0 3px var(--accent-dim)}

  /* -------- code split + terminal -------- */
  .code-split{flex:1 1 auto;min-height:0;display:flex}
  .code-left{flex:1 1 46%;min-width:0;display:flex;flex-direction:column;border-right:1px solid var(--hair)}
  .code-right{flex:1 1 54%;min-width:0;display:flex;flex-direction:column;background:#0A0C0B}
  .code-col{max-width:none;padding:20px 24px 24px}
  .code-left .composer-col{max-width:none;padding:6px 24px 16px}
  .term-head{flex:0 0 auto;height:40px;display:flex;align-items:center;justify-content:space-between;padding:0 14px;border-bottom:1px solid var(--hair)}
  .term-title{display:inline-flex;align-items:center;gap:8px;font-size:12px;font-weight:600;color:var(--text)}
  .term-title svg{width:14px;height:14px;color:var(--accent)}
  .term-live{display:inline-flex;align-items:center;gap:7px;font-family:var(--mono);font-size:11px;color:var(--faint)}
  .term-live .dot{background:var(--faint);box-shadow:none;animation:none}
  .term-live.live{color:var(--accent)}
  .term-live.live .dot{background:var(--accent);box-shadow:0 0 0 3px var(--accent-dim);animation:pulse 2.2s var(--ease) infinite}
  .term-host{flex:1 1 auto;min-height:0;padding:8px 8px 4px}
  .term-host .xterm{height:100%}
  .term-host .xterm-viewport{background:transparent!important}

  /* -------- toast -------- */
  .toast{position:fixed;left:50%;bottom:26px;transform:translateX(-50%) translateY(8px);
    background:var(--elevated);border:1px solid var(--hair-2);color:var(--text);
    padding:10px 16px;border-radius:11px;font-size:13px;box-shadow:0 12px 34px rgba(0,0,0,.5);
    opacity:0;pointer-events:none;transition:opacity .2s var(--ease),transform .2s var(--ease);z-index:120;max-width:80%}
  .toast.show{opacity:1;transform:translateX(-50%) translateY(0)}
  .toast.err{border-color:rgba(240,97,109,.5)}

  @media (max-width:960px){ .code-split{flex-direction:column} .code-left{border-right:0;border-bottom:1px solid var(--hair)} .code-right{min-height:320px;flex:0 0 auto} }
  @media (max-width:720px){ :root{--sidebar-w:216px} .conv-col,.composer-col{padding-left:20px;padding-right:20px} }
  @media (prefers-reduced-motion:reduce){
    *{animation-duration:.001ms!important;animation-iteration-count:1!important;transition-duration:.001ms!important}
  }

  /* --- brand bloom logo (colored ASCII, flower palette) --- */
  .logo-bloom{font-family:var(--mono);white-space:pre;font-size:clamp(7px,1.05vw,19px);line-height:1;font-weight:700;display:inline-block;margin:0 0 clamp(14px,1.4vw,26px);
    background:radial-gradient(125% 125% at 57% 43%, #F6E94E 0%, #C8EC55 13%, #6FE87A 29%, #3FE08A 44%, #3FE0C4 60%, #46B6F0 79%, #4361EE 100%);
    -webkit-background-clip:text;background-clip:text;color:transparent;-webkit-text-fill-color:transparent;
    filter:drop-shadow(0 0 22px rgba(63,224,138,.16))}
  .empty{display:flex;flex-direction:column;align-items:center;justify-content:center;text-align:center;min-height:60vh;gap:2px}
  .empty .brand-word{font-family:'Inter',system-ui;font-size:clamp(22px,2vw,36px);font-weight:600;letter-spacing:.14em;color:var(--text);margin:0}
  .empty p{font-size:clamp(14px,1.05vw,17px)}
  .empty p{color:var(--muted);max-width:46ch;margin:6px 0 0}
  @media (prefers-reduced-motion: reduce){.logo-bloom{filter:none}}

  /* ===================== new-chat + command affordance row ===================== */
  .sb-actions{display:flex;gap:8px;align-items:stretch;margin:2px 12px 10px}
  .sb-actions .sb-new{margin:0;width:auto;flex:1 1 auto}
  .sb-cmd{flex:0 0 auto;display:inline-flex;align-items:center;justify-content:center;padding:0 11px;
    background:var(--panel-2);border:1px solid var(--hair-2);color:var(--faint);border-radius:var(--radius-sm);
    font-family:var(--mono);font-size:11px;font-weight:600;letter-spacing:.03em;
    transition:color .16s var(--ease),border-color .16s var(--ease),background .16s var(--ease)}
  .sb-cmd:hover{color:var(--text);border-color:rgba(63,224,138,.35);background:var(--elevated)}

  /* ===================== sidebar connection status ===================== */
  .sb-conn{flex:0 0 auto;display:flex;align-items:center;gap:9px;width:100%;text-align:left;min-width:0;
    border:0;border-top:1px solid var(--hair);background:transparent;color:var(--muted);
    padding:11px 16px;font-size:12.5px;font-weight:550;
    transition:background .14s var(--ease),color .14s var(--ease)}
  .sb-conn:hover{background:var(--panel-2);color:var(--text)}
  .conn-dot{width:7px;height:7px;border-radius:50%;flex:0 0 auto;background:var(--faint)}
  .conn-dot.is-on{background:var(--accent);box-shadow:0 0 0 3px var(--accent-dim)}
  .conn-text{overflow:hidden;text-overflow:ellipsis;white-space:nowrap;min-width:0}

  /* ===================== modal + scrim ===================== */
  .scrim{position:fixed;inset:0;z-index:80;background:rgba(0,0,0,.55);
    display:flex;align-items:flex-start;justify-content:center;padding:20px;
    opacity:0;pointer-events:none;transition:opacity .18s var(--ease)}
  .scrim.show{opacity:1;pointer-events:auto}
  .modal{margin-top:13vh;width:min(500px,94vw);max-height:74vh;display:flex;flex-direction:column;
    background:var(--panel);border:1px solid var(--hair-2);border-radius:var(--radius);
    box-shadow:0 24px 70px rgba(0,0,0,.55);transform:translateY(-10px);transition:transform .18s var(--ease)}
  .scrim.show .modal{transform:translateY(0)}
  .modal-head{flex:0 0 auto;display:flex;align-items:center;justify-content:space-between;gap:12px;
    padding:15px 16px 13px;border-bottom:1px solid var(--hair)}
  .modal-title{margin:0;font-size:14px;font-weight:600;letter-spacing:-.01em;color:var(--text)}
  .modal-x{display:inline-flex;align-items:center;justify-content:center;width:28px;height:28px;border-radius:var(--radius-sm);
    border:0;background:transparent;color:var(--muted);transition:color .14s var(--ease),background .14s var(--ease)}
  .modal-x:hover{color:var(--text);background:rgba(255,255,255,.07)}
  .modal-x svg{width:16px;height:16px}
  .modal-body{padding:16px;overflow-y:auto;min-height:0}
  .modal-help{margin:0 0 15px;color:var(--muted);font-size:13px;line-height:1.55}

  .field{display:flex;flex-direction:column;gap:6px;margin-bottom:13px}
  .field-label{font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:.07em;color:var(--faint)}
  .field-input{width:100%;background:var(--bg);border:1px solid var(--hair-2);border-radius:var(--radius-sm);color:var(--text);
    font-size:13.5px;padding:9px 11px;transition:border-color .15s var(--ease),box-shadow .15s var(--ease)}
  .field-input::placeholder{color:var(--faint)}
  .field-input:focus{outline:none;border-color:rgba(63,224,138,.4);box-shadow:0 0 0 3px var(--accent-dim)}
  .field-pw{position:relative;display:flex;align-items:center}
  .field-pw .field-input{padding-right:40px}
  .pw-eye{position:absolute;right:5px;display:inline-flex;align-items:center;justify-content:center;width:30px;height:30px;
    border:0;background:transparent;color:var(--muted);border-radius:6px;transition:color .14s var(--ease),background .14s var(--ease)}
  .pw-eye:hover{color:var(--text);background:rgba(255,255,255,.07)}
  .pw-eye svg{width:16px;height:16px}

  .modal-status{min-height:18px;font-size:12.5px;line-height:1.5;color:var(--muted);margin:2px 0 4px}
  .modal-status.is-err{color:var(--danger)}
  .modal-status.is-ok{color:var(--accent)}
  .modal-actions{display:flex;align-items:center;justify-content:flex-end;gap:9px;margin-top:12px}
  .danger-btn{margin-right:auto}
  .danger-btn:hover{border-color:rgba(240,97,109,.5);color:var(--danger)}
  .primary-btn:disabled{background:var(--elevated);color:var(--faint);cursor:not-allowed}
  .primary-btn[aria-busy="true"]{opacity:.9}

  .spinner{width:14px;height:14px;border:2px solid rgba(4,20,10,.35);border-top-color:#04140A;border-radius:50%;
    display:inline-block;animation:spin .7s linear infinite}
  @keyframes spin{to{transform:rotate(360deg)}}

  /* ===================== command palette ===================== */
  .cmd-modal{margin-top:11vh;width:min(560px,94vw)}
  .cmd-search-row{flex:0 0 auto;display:flex;align-items:center;gap:10px;padding:12px 15px;border-bottom:1px solid var(--hair)}
  .cmd-search-ico{display:inline-flex;color:var(--faint)}
  .cmd-search-ico svg{width:17px;height:17px}
  .cmd-input{flex:1 1 auto;min-width:0;background:transparent;border:0;color:var(--text);font-size:15px;padding:2px 0}
  .cmd-input::placeholder{color:var(--faint)}
  .cmd-input:focus{outline:none}
  .cmd-list{flex:1 1 auto;min-height:0;overflow-y:auto;padding:6px}
  .cmd-item{position:relative;display:flex;align-items:center;gap:11px;width:100%;text-align:left;
    padding:9px 11px;border:0;background:transparent;color:var(--text);border-radius:var(--radius-sm);
    font-size:13.5px;transition:background .12s var(--ease)}
  .cmd-item .cmd-ico{display:inline-flex;color:var(--muted);flex:0 0 auto}
  .cmd-item .cmd-ico svg{width:16px;height:16px}
  .cmd-label{flex:1 1 auto;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
  .cmd-hint{flex:0 0 auto;font-family:var(--mono);font-size:11px;color:var(--faint);letter-spacing:.03em}
  .cmd-item.is-sel{background:var(--accent-dim)}
  .cmd-item.is-sel::before{content:"";position:absolute;left:0;top:7px;bottom:7px;width:2px;border-radius:2px;background:var(--accent)}
  .cmd-item.is-sel .cmd-ico{color:var(--accent)}
  .cmd-empty{padding:16px 12px;color:var(--faint);font-size:13px;text-align:center}

  /* ===================== help ===================== */
  .help-list{list-style:none;margin:0;padding:0;display:flex;flex-direction:column;gap:10px}
  .help-list li{display:flex;align-items:flex-start;gap:14px;font-size:13.5px;color:var(--text)}
  .help-list li>span:last-child{color:var(--muted)}
  .help-keys{flex:0 0 auto;min-width:158px;display:inline-flex;align-items:center;gap:4px;flex-wrap:wrap}
  kbd{display:inline-flex;align-items:center;justify-content:center;min-width:20px;height:20px;padding:0 5px;
    background:var(--panel-2);border:1px solid var(--hair-2);border-radius:5px;
    font-family:var(--mono);font-size:11px;color:var(--muted)}

  /* ===================== model picker ===================== */
  .model-btn{display:inline-flex;align-items:center;gap:7px;background:transparent;border:1px solid var(--hair-2);
    color:var(--muted);font-size:12.5px;font-weight:550;padding:6px 9px 6px 10px;border-radius:var(--radius-sm);
    white-space:nowrap;max-width:220px;
    transition:color .15s var(--ease),border-color .15s var(--ease),background .15s var(--ease)}
  .model-btn:hover{color:var(--text);border-color:rgba(63,224,138,.35);background:var(--elevated)}
  .model-btn .mb-ico{display:inline-flex;color:var(--muted);flex:0 0 auto}
  .model-btn:hover .mb-ico{color:var(--text)}
  .model-btn .mb-ico svg{width:14px;height:14px}
  .model-btn .mb-name{font-family:var(--mono);font-size:12px;overflow:hidden;text-overflow:ellipsis;min-width:0}
  .model-btn .mb-caret{display:inline-flex;color:var(--faint);flex:0 0 auto}
  .model-btn .mb-caret svg{width:13px;height:13px}

  .model-list{display:flex;flex-direction:column;gap:2px;min-height:62px}
  .model-opt{position:relative;display:flex;align-items:center;gap:11px;width:100%;text-align:left;
    padding:10px 12px;border:1px solid transparent;background:transparent;color:var(--text);border-radius:var(--radius-sm);
    transition:background .12s var(--ease),border-color .12s var(--ease)}
  .model-opt:hover{background:var(--panel-2)}
  .model-opt.is-sel{background:var(--accent-dim);border-color:rgba(63,224,138,.16)}
  .model-opt.is-sel::before{content:"";position:absolute;left:0;top:8px;bottom:8px;width:2px;border-radius:2px;background:var(--accent)}
  .model-opt .mo-name{flex:1 1 auto;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;
    font-family:var(--mono);font-size:13px;color:var(--text)}
  .model-opt .mo-mark{flex:0 0 auto;display:inline-flex;align-items:center;justify-content:center;width:16px;height:16px;color:var(--accent)}
  .model-opt .mo-mark svg{width:16px;height:16px}
  .model-loading,.model-empty{padding:14px 12px;color:var(--faint);font-size:13px;line-height:1.5}

  .model-section{margin-top:10px;padding-top:10px;border-top:1px solid var(--hair)}
  .model-section-label{font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:.07em;color:var(--faint);padding:2px 12px 8px}
  .mo-size{flex:0 0 auto;font-family:var(--mono);font-size:12px;color:var(--muted);font-variant-numeric:tabular-nums}
  .dl-row{display:flex;align-items:center;gap:11px;width:100%;padding:7px 12px;border-radius:var(--radius-sm)}
  .dl-row .mo-name{flex:1 1 auto;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;font-family:var(--mono);font-size:13px;color:var(--text)}
  .dl-btn{display:inline-flex;align-items:center;gap:7px;flex:0 0 auto;background:transparent;border:1px solid var(--hair-2);color:var(--muted);
    font-size:12px;font-weight:550;padding:5px 11px;border-radius:var(--radius-sm);white-space:nowrap;
    transition:color .15s var(--ease),border-color .15s var(--ease),background .15s var(--ease)}
  .dl-btn:hover{color:var(--accent);border-color:rgba(63,224,138,.4);background:var(--accent-dim)}
  .dl-btn svg{width:14px;height:14px}
  .dl-btn:disabled{color:var(--faint);cursor:not-allowed;border-color:var(--hair);background:transparent}
  .dl-progress{display:inline-flex;align-items:center;gap:7px;flex:0 0 auto;color:var(--muted);font-size:12px}
  .dl-spin{width:13px;height:13px;border:2px solid var(--hair-2);border-top-color:var(--accent);border-radius:50%;
    display:inline-block;animation:spin .7s linear infinite}
  .model-opt-wrap{display:flex;align-items:stretch;gap:6px;width:100%}
  .model-opt-wrap .model-opt{flex:1 1 auto;min-width:0}
  .mo-del{flex:0 0 auto;display:inline-flex;align-items:center;justify-content:center;width:36px;
    background:transparent;border:1px solid transparent;border-radius:var(--radius-sm);color:var(--faint);cursor:pointer;
    transition:color .12s var(--ease),border-color .12s var(--ease),background .12s var(--ease)}
  .model-opt-wrap:hover .mo-del{border-color:var(--hair)}
  .mo-del:hover{color:#ff6b6b;border-color:rgba(255,107,107,.4);background:rgba(255,107,107,.09)}
  .mo-del svg{width:15px;height:15px}
  .mo-del:disabled{opacity:.4;cursor:not-allowed;color:var(--faint);border-color:transparent;background:transparent}
</style>
</head>
<body>
<div class="app">
  <!-- ===================== SIDEBAR ===================== -->
  <aside class="sidebar" aria-label="Navigation">
    <div class="sb-head">
      <img class="mark" src="data:image/png;base64,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" alt="LOREA logo" />
      <span class="sb-word">LOREA</span>
      <span class="sb-build">preview</span>
    </div>

    <div class="sb-actions">
      <button class="sb-new" id="sbNewChat" type="button">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M12 5v14"/><path d="M5 12h14"/></svg>
        <span>New chat</span>
      </button>
      <button class="sb-cmd" id="sbCmd" type="button" aria-label="Open command palette, Command K" title="Command palette (Cmd K)">
        <span aria-hidden="true">&#8984;K</span>
      </button>
    </div>

    <div class="sb-label"><span>Chats</span></div>
    <div class="sb-chats" id="sbChatList"></div>

    <nav class="sb-nav" role="tablist" aria-label="Views">
      <button class="nav-tab is-active" role="tab" id="tabbtn-chat" aria-selected="true" aria-controls="view-chat" data-tab="chat" type="button">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M21 15a2 2 0 0 1-2 2H8l-4 4V5a2 2 0 0 1 2-2h13a2 2 0 0 1 2 2z"/></svg>
        <span>Chat</span><span class="kb">1</span>
      </button>
      <button class="nav-tab" role="tab" id="tabbtn-space" aria-selected="false" aria-controls="view-space" data-tab="space" type="button">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M4 20h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.93a2 2 0 0 1-1.66-.9l-.82-1.2A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z"/></svg>
        <span>Space</span><span class="kb">2</span>
      </button>
      <button class="nav-tab" role="tab" id="tabbtn-code" aria-selected="false" aria-controls="view-code" data-tab="code" type="button">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M4 17l6-6-6-6"/><path d="M12 19h8"/></svg>
        <span>Code</span><span class="kb">3</span>
      </button>
    </nav>

    <button class="sb-conn" id="sbConn" type="button" aria-label="Server connection">
      <span class="conn-dot" id="sbConnDot"></span>
      <span class="conn-text" id="sbConnText">Local</span>
    </button>
  </aside>

  <!-- ===================== MAIN ===================== -->
  <main class="main">
    <!-- CHAT -->
    <section id="view-chat" class="view is-active" role="tabpanel" aria-labelledby="tabbtn-chat" tabindex="0">
      <header class="view-top">
        <div class="vt-left"><h1 class="vt-title">Chat</h1></div>
        <div class="vt-right"><span class="work-pill" hidden><span class="dot"></span>Working</span><button type="button" class="model-btn" aria-label="Select model" aria-haspopup="dialog"><span class="mb-ico"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M12 2l10 10-10 10L2 12z"/></svg></span><span class="mb-name">model</span><span class="mb-caret"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M6 9l6 6 6-6"/></svg></span></button></div>
      </header>
      <div class="conv-scroll" id="chatScroll"><div class="conv-col"><div id="chatList" class="msg-list"></div></div></div>
      <div class="composer-wrap"><div class="composer-col" id="chatComposerWrap"></div></div>
    </section>

    <!-- SPACE -->
    <section id="view-space" class="view" role="tabpanel" aria-labelledby="tabbtn-space" tabindex="0">
      <header class="view-top">
        <div class="vt-left ws-info">
          <span class="vt-eyebrow">Workspace</span>
          <span class="ws-path" id="wsPath">No folder selected</span>
        </div>
        <div class="vt-right">
          <span class="work-pill" hidden><span class="dot"></span>Working</span>
          <button class="ghost-btn" id="wsChange" type="button">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M4 20h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.93a2 2 0 0 1-1.66-.9l-.82-1.2A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z"/></svg>
            <span>Change folder</span>
          </button>
          <button type="button" class="model-btn" aria-label="Select model" aria-haspopup="dialog"><span class="mb-ico"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M12 2l10 10-10 10L2 12z"/></svg></span><span class="mb-name">model</span><span class="mb-caret"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M6 9l6 6 6-6"/></svg></span></button>
        </div>
      </header>
      <div class="ws-input-row" id="wsInputRow" hidden>
        <input type="text" id="wsInput" class="text-input" placeholder="/absolute/path/to/folder" aria-label="Workspace folder path">
        <button class="primary-btn" id="wsConfirm" type="button">Set</button>
        <button class="ghost-btn" id="wsCancel" type="button">Cancel</button>
      </div>
      <div class="conv-scroll" id="spaceScroll"><div class="conv-col">
        <div class="ws-notice" id="spaceNotice">
          <div class="notice-title">No workspace selected</div>
          <p>Pick a folder and LOREA will read, edit, and run commands scoped to that directory.</p>
          <button class="primary-btn" id="wsEmptyPick" type="button">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M4 20h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.93a2 2 0 0 1-1.66-.9l-.82-1.2A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z"/></svg>
            <span>Choose folder</span>
          </button>
        </div>
        <div id="spaceList" class="msg-list" hidden></div>
      </div></div>
      <div class="composer-wrap" id="spaceComposerBar" hidden><div class="composer-col" id="spaceComposerWrap"></div></div>
    </section>

    <!-- CODE -->
    <section id="view-code" class="view" role="tabpanel" aria-labelledby="tabbtn-code" tabindex="0">
      <header class="view-top">
        <div class="vt-left"><h1 class="vt-title">Code</h1></div>
        <div class="vt-right"><span class="work-pill" hidden><span class="dot"></span>Working</span><button type="button" class="model-btn" aria-label="Select model" aria-haspopup="dialog"><span class="mb-ico"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M12 2l10 10-10 10L2 12z"/></svg></span><span class="mb-name">model</span><span class="mb-caret"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M6 9l6 6 6-6"/></svg></span></button></div>
      </header>
      <div class="code-split">
        <div class="code-left">
          <div class="conv-scroll" id="codeScroll"><div class="conv-col code-col"><div id="codeList" class="msg-list"></div></div></div>
          <div class="composer-wrap"><div class="composer-col" id="codeComposerWrap"></div></div>
        </div>
        <div class="code-right">
          <div class="term-head">
            <span class="term-title">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M4 17l6-6-6-6"/><path d="M12 19h8"/></svg>
              <span>workspace terminal</span>
            </span>
            <span class="term-live" id="codeTermStatus"><span class="dot"></span>idle</span>
          </div>
          <div class="term-host" id="termHost" aria-label="Interactive terminal"></div>
        </div>
      </div>
    </section>
  </main>
</div>
<div class="toast" id="toast" role="status" aria-live="polite" hidden></div>

<!-- ===================== CONNECT PANEL ===================== -->
<div class="scrim" id="connScrim" hidden>
  <div class="modal" id="connModal" role="dialog" aria-modal="true" aria-labelledby="connTitle" tabindex="-1">
    <div class="modal-head">
      <h2 class="modal-title" id="connTitle">Connect to a server</h2>
      <button type="button" class="modal-x" id="connClose" aria-label="Close">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M18 6L6 18"/><path d="M6 6l12 12"/></svg>
      </button>
    </div>
    <div class="modal-body">
      <p class="modal-help">Connect to a LOREA/GalliviumCloud MPC server. Your token is saved and reconnected automatically.</p>
      <div class="field">
        <label class="field-label" for="connUrl">Server URL</label>
        <input type="url" id="connUrl" class="field-input" placeholder="https://...trycloudflare.com" autocomplete="off" spellcheck="false" autocapitalize="off">
      </div>
      <div class="field">
        <label class="field-label" for="connToken">Token</label>
        <div class="field-pw">
          <input type="password" id="connToken" class="field-input" autocomplete="off" spellcheck="false" autocapitalize="off">
          <button type="button" class="pw-eye" id="connEye" aria-label="Show token"></button>
        </div>
      </div>
      <div class="modal-status" id="connStatus" role="status" aria-live="polite"></div>
      <div class="modal-actions">
        <button type="button" class="ghost-btn danger-btn" id="connDisconnect" hidden>Disconnect</button>
        <button type="button" class="primary-btn" id="connSubmit">Connect</button>
      </div>
    </div>
  </div>
</div>

<!-- ===================== COMMAND PALETTE ===================== -->
<div class="scrim" id="cmdScrim" hidden>
  <div class="modal cmd-modal" id="cmdModal" role="dialog" aria-modal="true" aria-label="Command palette" tabindex="-1">
    <div class="cmd-search-row">
      <span class="cmd-search-ico"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><circle cx="11" cy="11" r="7"/><path d="M21 21l-4.3-4.3"/></svg></span>
      <input type="text" id="cmdInput" class="cmd-input" placeholder="Type a command&hellip;" autocomplete="off" spellcheck="false" autocapitalize="off"
        role="combobox" aria-expanded="true" aria-controls="cmdList" aria-autocomplete="list" aria-label="Search commands">
    </div>
    <div class="cmd-list" id="cmdList" role="listbox" aria-label="Commands"></div>
  </div>
</div>

<!-- ===================== HELP ===================== -->
<div class="scrim" id="helpScrim" hidden>
  <div class="modal" id="helpModal" role="dialog" aria-modal="true" aria-labelledby="helpTitle" tabindex="-1">
    <div class="modal-head">
      <h2 class="modal-title" id="helpTitle">Help &amp; shortcuts</h2>
      <button type="button" class="modal-x" id="helpClose" aria-label="Close">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M18 6L6 18"/><path d="M6 6l12 12"/></svg>
      </button>
    </div>
    <div class="modal-body">
      <p class="modal-help">LOREA is a local-first AI workspace: chat, run tasks scoped to a workspace folder, and drive a live terminal. Connect to a LOREA/GalliviumCloud server to run against a remote model.</p>
      <ul class="help-list">
        <li><span class="help-keys"><kbd>&#8984;</kbd><kbd>K</kbd></span><span>Open the command palette</span></li>
        <li><span class="help-keys"><kbd>&#8984;</kbd><kbd>1</kbd> <kbd>&#8984;</kbd><kbd>2</kbd> <kbd>&#8984;</kbd><kbd>3</kbd></span><span>Switch to Chat / Space / Code</span></li>
        <li><span class="help-keys"><kbd>Enter</kbd></span><span>Send message</span></li>
        <li><span class="help-keys"><kbd>Shift</kbd><kbd>Enter</kbd></span><span>New line in the composer</span></li>
        <li><span class="help-keys"><kbd>Esc</kbd></span><span>Close dialogs</span></li>
      </ul>
    </div>
  </div>
</div>

<!-- ===================== MODEL PICKER ===================== -->
<div class="scrim" id="modelScrim" hidden>
  <div class="modal" id="modelModal" role="dialog" aria-modal="true" aria-labelledby="modelTitle" tabindex="-1">
    <div class="modal-head">
      <h2 class="modal-title" id="modelTitle">Select model</h2>
      <button type="button" class="modal-x" id="modelClose" aria-label="Close">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="M18 6L6 18"/><path d="M6 6l12 12"/></svg>
      </button>
    </div>
    <div class="modal-body">
      <p class="modal-help" id="modelHelp">Local models (ollama)</p>
      <div class="model-list" id="modelList" role="listbox" aria-label="Available models"></div>
    </div>
  </div>
</div>

<script>
(function(){
  "use strict";
  var $ = function(id){ return document.getElementById(id); };

  /* ---------- inline icons (Lucide-style, stroke 1.75) ---------- */
  function svg(inner){ return '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.75" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">'+inner+'</svg>'; }
  var icon = {
    paperclip: svg('<path d="M21.44 11.05l-9.19 9.19a6 6 0 0 1-8.49-8.49l9.19-9.19a4 4 0 0 1 5.66 5.66l-9.2 9.19a2 2 0 0 1-2.83-2.83l8.49-8.48"/>'),
    globe: svg('<circle cx="12" cy="12" r="9"/><path d="M3 12h18"/><path d="M12 3a15 15 0 0 1 4 9 15 15 0 0 1-4 9 15 15 0 0 1-4-9 15 15 0 0 1 4-9z"/>'),
    send: svg('<path d="M12 19V6"/><path d="M6 12l6-6 6 6"/>'),
    x: svg('<path d="M18 6L6 18"/><path d="M6 6l12 12"/>'),
    copy: svg('<rect x="9" y="9" width="12" height="12" rx="2"/><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/>'),
    caret: svg('<path d="M9 6l6 6-6 6"/>'),
    plus: svg('<path d="M12 5v14"/><path d="M5 12h14"/>'),
    chat: svg('<path d="M21 15a2 2 0 0 1-2 2H8l-4 4V5a2 2 0 0 1 2-2h13a2 2 0 0 1 2 2z"/>'),
    folder: svg('<path d="M4 20h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.93a2 2 0 0 1-1.66-.9l-.82-1.2A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2z"/>'),
    code: svg('<path d="M4 17l6-6-6-6"/><path d="M12 19h8"/>'),
    server: svg('<rect x="3" y="4" width="18" height="7" rx="2"/><rect x="3" y="13" width="18" height="7" rx="2"/><path d="M7 7.5h.01"/><path d="M7 16.5h.01"/>'),
    unplug: svg('<path d="M9 2v6"/><path d="M15 2v6"/><path d="M7 8h10v3a5 5 0 0 1-10 0z"/><path d="M12 16v6"/>'),
    gauge: svg('<path d="M12 14l4-4"/><path d="M3.34 19a10 10 0 1 1 17.32 0"/>'),
    help: svg('<circle cx="12" cy="12" r="9"/><path d="M9.5 9.5a2.5 2.5 0 0 1 4.5 1.5c0 1.6-2 2-2 3"/><path d="M12 17h.01"/>'),
    eye: svg('<path d="M2 12s3.5-7 10-7 10 7 10 7-3.5 7-10 7-10-7-10-7z"/><circle cx="12" cy="12" r="3"/>'),
    eyeOff: svg('<path d="M9.9 4.24A9.1 9.1 0 0 1 12 4c6.5 0 10 7 10 7a13.2 13.2 0 0 1-1.67 2.68"/><path d="M6.6 6.6A13.5 13.5 0 0 0 2 12s3.5 7 10 7a9.1 9.1 0 0 0 5.4-1.61"/><path d="M14.12 14.12a3 3 0 0 1-4.24-4.24"/><path d="M2 2l20 20"/>'),
    diamond: svg('<path d="M12 2l10 10-10 10L2 12z"/>'),
    chevronDown: svg('<path d="M6 9l6 6 6-6"/>'),
    check: svg('<path d="M20 6L9 17l-5-5"/>'),
    download: svg('<path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><path d="M7 10l5 5 5-5"/><path d="M12 15V3"/>'),
    trash: svg('<path d="M3 6h18"/><path d="M8 6V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/><path d="M19 6l-1 14a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2L5 6"/><path d="M10 11v6"/><path d="M14 11v6"/>')
  };

  /* ---------- shared conversation state ---------- */
  var state = { messages: [], busy: false, status: 'idle', turn_id: -1, workspace: '', activeTab: 'chat', poll: null, chats: [] };
  var currentChatId = null;
  var composerState = {
    chat:  { files: [], web: true, effort: 'basic' },
    space: { files: [], web: true, effort: 'basic' },
    code:  { files: [], web: true, effort: 'basic' }
  };
  var VIEWS = ['chat','space','code'];

  /* ---------- escaping + markdown (preserved) ---------- */
  function escapeHtml(s){
    return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
  }
  function inlineMd(raw){
    var parts = String(raw).split(/(`[^`]+`)/g);
    return parts.map(function(part){
      if(part.length>=2 && part.charAt(0)==='`' && part.charAt(part.length-1)==='`'){
        return '<code>'+escapeHtml(part.slice(1,-1))+'</code>';
      }
      var t = escapeHtml(part);
      t = t.replace(/\[([^\]]+)\]\((https?:\/\/[^\s)]+)\)/g, function(m,txt,url){ return '<a href="'+url+'" target="_blank" rel="noopener noreferrer">'+txt+'</a>'; });
      t = t.replace(/\*\*([^*]+)\*\*/g,'<strong>$1</strong>').replace(/__([^_]+)__/g,'<strong>$1</strong>');
      t = t.replace(/(^|[^*])\*([^*\n]+)\*/g,'$1<em>$2</em>');
      t = t.replace(/(^|[^_\w])_([^_\n]+)_/g,'$1<em>$2</em>');
      return t;
    }).join('');
  }
  function blockText(text){
    var lines = text.split('\n');
    var html='', para=[], listType=null;
    function flushPara(){ if(para.length){ html+='<p>'+inlineMd(para.join(' '))+'</p>'; para=[]; } }
    function closeList(){ if(listType){ html+= (listType==='ul'?'</ul>':'</ol>'); listType=null; } }
    for(var i=0;i<lines.length;i++){
      var line = lines[i].replace(/\s+$/,'');
      if(line.trim()===''){ flushPara(); closeList(); continue; }
      var m;
      if((m=line.match(/^(#{1,6})\s+(.*)$/))){ flushPara(); closeList(); var lvl=Math.min(4,m[1].length); html+='<h'+lvl+'>'+inlineMd(m[2])+'</h'+lvl+'>'; continue; }
      if((m=line.match(/^\s*[-*+]\s+(.*)$/))){ flushPara(); if(listType!=='ul'){ closeList(); listType='ul'; html+='<ul>'; } html+='<li>'+inlineMd(m[1])+'</li>'; continue; }
      if((m=line.match(/^\s*\d+[.)]\s+(.*)$/))){ flushPara(); if(listType!=='ol'){ closeList(); listType='ol'; html+='<ol>'; } html+='<li>'+inlineMd(m[1])+'</li>'; continue; }
      if((m=line.match(/^\s*>\s?(.*)$/))){ flushPara(); closeList(); html+='<blockquote>'+inlineMd(m[1])+'</blockquote>'; continue; }
      closeList(); para.push(line.trim());
    }
    flushPara(); closeList();
    return html;
  }
  function codeBlockHTML(lang, code){
    return '<div class="code-block"><div class="code-head"><span class="code-lang">'+escapeHtml(lang||'text')+
      '</span><button type="button" class="copy-btn" aria-label="Copy code">'+icon.copy+'<span class="cp-label">Copy</span></button></div>'+
      '<pre><code>'+escapeHtml(code)+'</code></pre></div>';
  }
  function renderMarkdown(src){
    var text = String(src==null?'':src).replace(/\r\n?/g,'\n');
    var lines = text.split('\n'), i=0, out='';
    while(i<lines.length){
      var fence = lines[i].match(/^\s*```(.*)$/);
      if(fence){
        var lang = fence[1].trim(), code=[]; i++;
        while(i<lines.length && !/^\s*```\s*$/.test(lines[i])){ code.push(lines[i]); i++; }
        i++;
        out += codeBlockHTML(lang, code.join('\n'));
      } else {
        var chunk=[];
        while(i<lines.length && !/^\s*```/.test(lines[i])){ chunk.push(lines[i]); i++; }
        out += blockText(chunk.join('\n'));
      }
    }
    return out;
  }

  /* ---------- ANSI strip for agent activity ---------- */
  function stripAnsi(s){
    return String(s)
      .replace(/\x1b\][^\x07\x1b]*(?:\x07|\x1b\\)/g,'')
      .replace(/\x1b[PX^_][^\x1b]*\x1b\\/g,'')
      .replace(/\x1b\[[0-9;?]*[ -\/]*[@-~]/g,'')
      .replace(/\x1b[()][A-Za-z0-9]/g,'')
      .replace(/\x1b[=>NOME78]/g,'')
      .replace(/[\x00-\x08\x0b\x0c\x0e-\x1f]/g,'')
      .replace(/\r/g,'');
  }

  /* ---------- message rendering ---------- */
  var LOGO_ASCII = `                        ---
                    .-----------------:.
                  .-=-----------------------
                  :===---------------------
                :=====--------------------=
               =========------------------=
             =============---------------==                                 :-=====
          =================--------------==                        .-==============
        .==================-------------===                -========================
        =======================--------.===            =============================
     :==========================--=---. ===           ==============================:
   -===============================--:  ===           ===============================
   ==================================   ===            ==============================.
                            === .:-==   ===            ===============================
                =====       -==.   ===  ===            :==============================
             ========.       ===:  ===  ===             ==============================-
          -=============      -===  ==: ===             -==============================
          ===========.====:    -=== === ===            .===============================
          ===========*. ====:   .===:==.===         ===================================
             =====*==**=  ====-   =====-===.     =====-  ==============================-
                +********   :===:  ::::::::::::====.     ===============================
             :************    :====::::::::::::==    :==================================
         *****************=====::=:::::::::::::==========    :==========================.
      .:-******************+====:::::::::::::::===-.            :=======================:
     --**********************====::::::::::::::::========================================
     ---*********************+  :-===-::::::::::::                      .================
       -----*********=********-=====:::::::::::::                            :==========:
        --------------------***-=:::::::::::::=====-                               .
         ---------------------:   -==::==:::===========:
          -------------------    ========. ===-===  :=======
          .-----------------:   ==========.:==- ====     ======:
           .----------------  -======:===== ===  :===.       ========:
            :--------------- ===:=== ====== :==.   -===.     ========
             :-------------:=== ===  ======  ===     ==== =========.
              .-----------===- ===  -==  ==: ===      :==========
                ----------==  =========  ===  ==:  -===========
                 --------==  :==============  ===============
                   ------:    .==============:=============
                    .---:       =========================
                                              :======.`;
  function logoHTML(){ return '<pre class="logo-bloom" aria-hidden="true">'+escapeHtml(LOGO_ASCII)+'</pre>'; }
  function emptyStateHTML(kind){
    var sub = kind==='code' ? 'Ask for edits or tasks on the left; watch the live workspace terminal on the right.'
            : kind==='space' ? 'Messages run with your chosen workspace folder as context.'
            : 'Ask a question, paste code, or describe a task.';
    return '<div class="empty">'+logoHTML()+'<div class="brand-word">LOREA</div><p>'+sub+'</p></div>';
  }
  function thinkingHTML(){
    return '<div class="msg-assistant"><div class="thinking" aria-live="polite">'+
      '<span class="tdot"></span><span class="tdot"></span><span class="tdot"></span>'+
      '<span>Thinking</span></div></div>';
  }
  function activityHTML(act){
    var clean = stripAnsi(act).replace(/\n{3,}/g,'\n\n').replace(/^\s+|\s+$/g,'');
    if(!clean) return '';
    return '<details class="activity"><summary><span class="act-caret">'+icon.caret+'</span>Agent activity</summary>'+
      '<pre class="act-body">'+escapeHtml(clean)+'</pre></details>';
  }
  function renderInto(el, kind){
    if(!el) return;
    var html='';
    if(state.messages.length===0 && !state.busy){ html += emptyStateHTML(kind); }
    for(var i=0;i<state.messages.length;i++){
      var m = state.messages[i];
      if(m.role==='user'){
        html += '<div class="msg-user"><div class="bubble">'+renderMarkdown(m.content)+'</div></div>';
      } else {
        html += '<div class="msg-assistant"><div class="assistant-content">'+renderMarkdown(m.content)+'</div>'+
          (m.activity ? activityHTML(m.activity) : '')+'</div>';
      }
    }
    if(state.busy){ html += thinkingHTML(); }
    el.innerHTML = html;
  }
  function nearBottom(sc){ return sc.scrollHeight - sc.scrollTop - sc.clientHeight < 140; }
  function renderMessages(){
    [['chatScroll','chatList','chat'],['spaceScroll','spaceList','space'],['codeScroll','codeList','code']].forEach(function(t){
      var sc=$(t[0]); if(!sc) return;
      var stick = nearBottom(sc);
      renderInto($(t[1]), t[2]);
      if(stick) sc.scrollTop = sc.scrollHeight;
    });
  }
  function setSendDisabled(disabled){
    ['chatSend','spaceSend','codeSend'].forEach(function(id){ var b=$(id); if(b){ b.disabled=disabled; b.setAttribute('aria-busy', disabled?'true':'false'); } });
  }
  function updateBusyUI(){
    setSendDisabled(state.busy);
    var pills=document.querySelectorAll('.work-pill');
    for(var i=0;i<pills.length;i++){ pills[i].hidden = !state.busy; }
    renderMessages();
  }

  /* ---------- chat history (localStorage) ---------- */
  function genId(){ return 'c'+Date.now().toString(36)+Math.random().toString(36).slice(2,7); }
  function loadChats(){
    try{ var raw=localStorage.getItem('lorea.chats'); var a=raw?JSON.parse(raw):[]; state.chats=Array.isArray(a)?a:[]; }
    catch(e){ state.chats=[]; }
  }
  function saveChats(){ try{ localStorage.setItem('lorea.chats', JSON.stringify(state.chats)); }catch(e){} }
  function findChat(id){ for(var i=0;i<state.chats.length;i++){ if(state.chats[i].id===id) return state.chats[i]; } return null; }
  function chatTitleFrom(msgs){
    for(var i=0;i<msgs.length;i++){ if(msgs[i].role==='user' && msgs[i].content){ return String(msgs[i].content).split('\n')[0].slice(0,80); } }
    return (msgs[0] && msgs[0].content) ? String(msgs[0].content).split('\n')[0].slice(0,80) : 'New chat';
  }
  function upsertCurrentChat(){
    var msgs = state.messages.map(function(m){ return { role:m.role, content:m.content }; });
    var existing = findChat(currentChatId);
    if(!msgs.length){
      if(existing){ state.chats = state.chats.filter(function(c){ return c.id!==currentChatId; }); saveChats(); renderSidebar(); }
      return;
    }
    var title = chatTitleFrom(msgs);
    if(existing){ existing.messages=msgs; existing.title=title; existing.ts=Date.now(); }
    else { state.chats.push({ id: currentChatId, title: title, ts: Date.now(), messages: msgs }); }
    saveChats(); renderSidebar();
  }
  function relTime(ts){
    var s = Math.floor((Date.now() - ts)/1000);
    if(s<45) return 'just now';
    var m=Math.floor(s/60); if(m<60) return m+'m';
    var h=Math.floor(m/60); if(h<24) return h+'h';
    var d=Math.floor(h/24); if(d===1) return 'yesterday'; if(d<7) return d+'d';
    var dt=new Date(ts); return (dt.getMonth()+1)+'/'+dt.getDate();
  }
  function renderSidebar(){
    var box=$('sbChatList'); if(!box) return;
    var chats = state.chats.slice().sort(function(a,b){ return b.ts-a.ts; });
    if(!chats.length){ box.innerHTML='<div class="sb-empty">No chats yet</div>'; return; }
    var html='';
    for(var i=0;i<chats.length;i++){
      var c=chats[i]; var active=(c.id===currentChatId);
      html += '<div class="chat-row'+(active?' is-active':'')+'" data-id="'+escapeHtml(c.id)+'" role="button" tabindex="0" title="'+escapeHtml(c.title||'New chat')+'">'+
        '<div class="chat-meta"><span class="chat-title">'+escapeHtml(c.title||'New chat')+'</span>'+
        '<span class="chat-time">'+escapeHtml(relTime(c.ts))+'</span></div>'+
        '<button type="button" class="chat-del" data-id="'+escapeHtml(c.id)+'" aria-label="Delete chat">'+icon.x+'</button></div>';
    }
    box.innerHTML=html;
  }
  function focusActiveComposer(){ var ta=$(state.activeTab+'Textarea'); if(ta){ requestAnimationFrame(function(){ ta.focus(); }); } }
  function newChat(){
    if(state.busy){ showToast('LOREA is still working — finish or wait before starting a new chat.', true); return; }
    upsertCurrentChat();
    fetch('/api/new_chat', { method:'POST', headers:{'Content-Type':'application/json'}, body:'{}' }).catch(function(){});
    currentChatId = genId();
    state.messages = [];
    renderMessages(); renderSidebar(); focusActiveComposer();
  }
  function loadChat(id){
    if(state.busy){ showToast('LOREA is still working — try again in a moment.', true); return; }
    var c=findChat(id); if(!c) return;
    upsertCurrentChat();
    var msgs = (c.messages||[]).map(function(m){ return { role:m.role, content:m.content }; });
    currentChatId = id;
    state.messages = msgs;
    fetch('/api/load_chat', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ messages: msgs }) }).catch(function(){});
    renderMessages(); renderSidebar();
    switchTab('chat');
  }
  function deleteChat(id){
    state.chats = state.chats.filter(function(c){ return c.id!==id; });
    saveChats();
    if(id===currentChatId){
      currentChatId = genId(); state.messages = [];
      fetch('/api/new_chat', { method:'POST', headers:{'Content-Type':'application/json'}, body:'{}' }).catch(function(){});
      renderMessages();
    }
    renderSidebar();
  }
  function wireSidebar(){
    $('sbNewChat').addEventListener('click', newChat);
    var list=$('sbChatList');
    list.addEventListener('click', function(e){
      var del=e.target.closest('.chat-del');
      if(del){ e.stopPropagation(); deleteChat(del.getAttribute('data-id')); return; }
      var row=e.target.closest('.chat-row');
      if(row){ loadChat(row.getAttribute('data-id')); }
    });
    list.addEventListener('keydown', function(e){
      var row=e.target.closest('.chat-row'); if(!row) return;
      if(e.key==='Enter' || e.key===' '){ e.preventDefault(); loadChat(row.getAttribute('data-id')); }
    });
  }

  /* ---------- API ---------- */
  function apiGet(path){ return fetch(path).then(function(r){ return r.json(); }); }
  function refreshHistory(){
    return apiGet('/api/history').then(function(h){
      if(h && Array.isArray(h.messages)) state.messages = h.messages;
      if(h){ state.busy = !!h.busy; state.status = h.status || state.status; }
      updateBusyUI();
      upsertCurrentChat();
    }).catch(function(){});
  }
  function stopPolling(){ if(state.poll){ clearInterval(state.poll); state.poll=null; } }
  function startPolling(){
    stopPolling();
    state.poll = setInterval(function(){
      apiGet('/api/status').then(function(s){
        if(!s) return;
        state.busy = !!s.busy; state.status = s.status || '';
        if(!s.busy){ stopPolling(); if(s.error) showToast(String(s.error), true); refreshHistory(); }
        else { updateBusyUI(); }
      }).catch(function(){});
    }, 800);
  }

  /* ---------- upload (JSON contract: {filename, content_base64}) ---------- */
  function uploadAttachments(files){
    if(!files || !files.length) return Promise.resolve([]);
    var jobs = files.map(function(f){
      return new Promise(function(resolve){
        var reader = new FileReader();
        reader.onload = function(){
          fetch('/api/upload', { method:'POST', headers:{'Content-Type':'application/json'},
            body: JSON.stringify({ filename: f.name, content_base64: reader.result }) })
            .then(function(res){
              /* fetch only rejects on network failure, so a 400/500 from /api/upload lands
                 here as a success. Check status explicitly or upload errors stay invisible. */
              return res.json().catch(function(){ return {}; }).then(function(j){
                if(!res.ok){
                  showToast('Upload failed: '+f.name+((j&&j.error)?' - '+j.error:' ('+res.status+')'), true);
                  resolve(null); return;
                }
                resolve((j && j.path) ? j.path : null);
              });
            }, function(){ showToast('Upload failed: '+f.name, true); resolve(null); });
        };
        reader.onerror = function(){ showToast('Could not read '+f.name, true); resolve(null); };
        reader.readAsDataURL(f);
      });
    });
    return Promise.all(jobs).then(function(rs){ return rs.filter(Boolean); });
  }

  /* ---------- submit (preserves 202/409/error handling) ---------- */
  function submitMessage(prefix){
    var CFG = { chat:{requireWs:false, sendWs:false}, space:{requireWs:true, sendWs:true}, code:{requireWs:false, sendWs:true} };
    var cfg = CFG[prefix];
    var cs = composerState[prefix];
    var ta = $(prefix+'Textarea');
    var text = (ta.value||'').trim();
    if(!text || state.busy) return;
    if(cfg.requireWs && !state.workspace){ showToast('Choose a workspace folder first.', true); return; }

    var files = cs.files.slice();
    var namesNote = files.length
      ? '\n\n[Attached: '+files.map(function(f){ return f.name; }).join(', ')+']' : '';

    var body = { message: text + namesNote, web_search: cs.web, effort: cs.effort };
    if(cfg.sendWs && state.workspace){ body.workspace = state.workspace; }

    /* optimistic */
    state.messages.push({ role:'user', content: text + namesNote });
    state.busy = true; state.status = 'working';
    ta.value=''; autoGrow(ta);
    cs.files = []; renderChips(prefix);
    updateBusyUI(); upsertCurrentChat();

    uploadAttachments(files).then(function(paths){
      /* The bubble shows bare filenames, but the agent needs the real on-disk paths the
         upload handler wrote to, otherwise it tries to read a name relative to cwd and
         the attachment looks like it silently did nothing. */
      if(paths && paths.length){
        body.message = text + '\n\nAttached files (already saved on disk, read them with read_file):\n'
                     + paths.map(function(p){ return '- ' + p; }).join('\n');
      }
      return fetch('/api/chat', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify(body) });
    }).then(function(res){
      if(res.status===202){
        return res.json().then(function(j){ if(j && typeof j.turn_id!=='undefined') state.turn_id=j.turn_id; startPolling(); });
      }
      if(res.status===409){
        /* Agent busy — message NOT accepted. Roll back optimistic bubble, restore text. */
        if(state.messages.length && state.messages[state.messages.length-1].role==='user'){ state.messages.pop(); }
        ta.value = text; autoGrow(ta);
        showToast('LOREA is still working on the previous message — try again in a moment.', true);
        upsertCurrentChat();
        return res.json().catch(function(){ return {}; }).then(function(j){ if(j && typeof j.turn_id!=='undefined') state.turn_id=j.turn_id; startPolling(); });
      }
      return res.json().catch(function(){ return {}; }).then(function(j){
        showToast((j && j.error) ? j.error : ('Request failed ('+res.status+')'), true);
        state.busy=false; return refreshHistory();
      });
    }).catch(function(){
      showToast('Network error — could not reach LOREA.', true);
      state.busy=false; updateBusyUI();
    });
  }

  /* ---------- composer ---------- */
  function autoGrow(ta){ ta.style.height='auto'; ta.style.height=Math.min(ta.scrollHeight,200)+'px'; }
  function composerHTML(prefix){
    var ph = prefix==='space' ? 'Message about your workspace' : prefix==='code' ? 'Ask about code, request edits, run tasks' : 'Message LOREA';
    return '<div class="composer">'+
      '<div class="chips" id="'+prefix+'Chips"></div>'+
      '<textarea id="'+prefix+'Textarea" class="composer-input" rows="1" placeholder="'+ph+'" aria-label="Message LOREA"></textarea>'+
      '<div class="composer-tools"><div class="tools-left">'+
        '<input type="file" id="'+prefix+'FileInput" multiple hidden>'+
        '<button type="button" class="icon-btn" id="'+prefix+'Attach" aria-label="Attach files" title="Attach files">'+icon.paperclip+'</button>'+
        '<button type="button" class="toggle-chip is-on" id="'+prefix+'Web" role="switch" aria-checked="true" aria-label="Toggle web search">'+icon.globe+'<span>Web</span></button>'+
        '<label class="effort-field"><span class="sr-only">Effort level</span>'+
          '<select id="'+prefix+'Effort" class="effort-select" aria-label="Effort level">'+
            '<option value="basic">Basic</option><option value="tuned">Tuned</option><option value="elite">Elite</option>'+
            '<option value="mythic">Mythic</option><option value="beyond">Beyond</option>'+
          '</select></label>'+
      '</div>'+
      '<button type="button" class="send-btn" id="'+prefix+'Send" aria-label="Send message">'+icon.send+'</button>'+
      '</div></div>';
  }
  function renderChips(prefix){
    var box = $(prefix+'Chips'); if(!box) return;
    var files = composerState[prefix].files;
    var html='';
    for(var i=0;i<files.length;i++){
      html += '<span class="chip"><span class="chip-name">'+escapeHtml(files[i].name)+'</span>'+
        '<button type="button" class="chip-x" data-idx="'+i+'" aria-label="Remove '+escapeHtml(files[i].name)+'">'+icon.x+'</button></span>';
    }
    box.innerHTML = html;
  }
  function wireComposer(prefix){
    var ta = $(prefix+'Textarea'), send=$(prefix+'Send'), attach=$(prefix+'Attach'),
        fileInput=$(prefix+'FileInput'), web=$(prefix+'Web'), effort=$(prefix+'Effort'), chips=$(prefix+'Chips');
    ta.addEventListener('input', function(){ autoGrow(ta); });
    ta.addEventListener('keydown', function(e){
      if(e.key==='Enter' && !e.shiftKey && !e.isComposing){ e.preventDefault(); submitMessage(prefix); }
    });
    send.addEventListener('click', function(){ submitMessage(prefix); });
    attach.addEventListener('click', function(){ fileInput.click(); });
    fileInput.addEventListener('change', function(){
      var fs = composerState[prefix].files;
      for(var i=0;i<fileInput.files.length;i++){ fs.push(fileInput.files[i]); }
      fileInput.value=''; renderChips(prefix);
    });
    chips.addEventListener('click', function(e){
      var btn = e.target.closest('.chip-x'); if(!btn) return;
      var idx = parseInt(btn.getAttribute('data-idx'),10);
      composerState[prefix].files.splice(idx,1); renderChips(prefix);
    });
    web.addEventListener('click', function(){
      var cs = composerState[prefix]; cs.web = !cs.web;
      web.classList.toggle('is-on', cs.web); web.setAttribute('aria-checked', cs.web?'true':'false');
    });
    effort.addEventListener('change', function(){ composerState[prefix].effort = effort.value; });
  }

  /* ---------- copy delegation ---------- */
  document.addEventListener('click', function(e){
    var btn = e.target.closest('.copy-btn'); if(!btn) return;
    var block = btn.closest('.code-block'); var code = block ? block.querySelector('code') : null;
    if(!code || !navigator.clipboard) return;
    navigator.clipboard.writeText(code.textContent).then(function(){
      var lbl = btn.querySelector('.cp-label'); btn.classList.add('copied'); if(lbl) lbl.textContent='Copied';
      setTimeout(function(){ btn.classList.remove('copied'); if(lbl) lbl.textContent='Copy'; }, 1500);
    }).catch(function(){});
  });

  /* ---------- toast ---------- */
  var toastTimer=null;
  function showToast(msg, isErr){
    var t = $('toast'); t.textContent=msg; t.hidden=false;
    t.classList.toggle('err', !!isErr);
    requestAnimationFrame(function(){ t.classList.add('show'); });
    if(toastTimer) clearTimeout(toastTimer);
    toastTimer = setTimeout(function(){ t.classList.remove('show'); setTimeout(function(){ t.hidden=true; }, 220); }, 4200);
  }

  /* ---------- workspace ---------- */
  function applyWorkspaceUI(){
    var has = !!state.workspace;
    var p=$('wsPath'); p.textContent = has ? state.workspace : 'No folder selected'; p.classList.toggle('is-set', has);
    $('spaceNotice').hidden = has;
    $('spaceList').hidden = !has;
    $('spaceComposerBar').hidden = !has;
  }
  function postWorkspace(path){
    fetch('/api/workspace', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ path: path }) }).catch(function(){});
  }
  function setWorkspace(path){
    state.workspace = String(path||'').trim();
    $('wsInputRow').hidden = true;
    applyWorkspaceUI(); renderMessages();
    if(state.workspace) postWorkspace(state.workspace);
  }
  function pickWorkspace(){
    if(window.lorea && typeof window.lorea.pickFolder==='function'){
      try{
        Promise.resolve(window.lorea.pickFolder()).then(function(p){ if(p) setWorkspace(p); else openWsInput(); }, function(){ openWsInput(); });
        return;
      }catch(e){}
    }
    openWsInput();
  }
  function openWsInput(){ var row=$('wsInputRow'); row.hidden=false; var inp=$('wsInput'); inp.value=state.workspace||''; inp.focus(); }
  function closeWsInput(){ $('wsInputRow').hidden=true; }
  function wireWorkspace(){
    $('wsChange').addEventListener('click', pickWorkspace);
    $('wsEmptyPick').addEventListener('click', pickWorkspace);
    $('wsConfirm').addEventListener('click', function(){ var v=$('wsInput').value.trim(); if(v) setWorkspace(v); });
    $('wsCancel').addEventListener('click', closeWsInput);
    $('wsInput').addEventListener('keydown', function(e){
      if(e.key==='Enter'){ e.preventDefault(); var v=$('wsInput').value.trim(); if(v) setWorkspace(v); }
      else if(e.key==='Escape'){ closeWsInput(); }
    });
  }

  /* ---------- tabs ---------- */
  function switchTab(name){
    state.activeTab = name;
    VIEWS.forEach(function(n){
      var view=$('view-'+n), btn=$('tabbtn-'+n), on=(n===name);
      if(view) view.classList.toggle('is-active', on);
      if(btn){ btn.classList.toggle('is-active', on); btn.setAttribute('aria-selected', on?'true':'false'); }
    });
    if(name==='code'){ Term.activate(); }
    renderMessages();
  }
  function wireTabs(){
    VIEWS.forEach(function(n){ var b=$('tabbtn-'+n); if(b) b.addEventListener('click', function(){ switchTab(n); }); });
    document.addEventListener('keydown', function(e){
      if((e.metaKey||e.ctrlKey) && e.key>='1' && e.key<='3'){ e.preventDefault(); switchTab(VIEWS[parseInt(e.key,10) - 1]); }
      else if(e.key==='Escape'){ closeWsInput(); }
    });
  }

  /* =========================================================
     Terminal — xterm.js over the SSE stream (assets from /vendor)
     ========================================================= */
  var Term = (function(){
    var term=null, fit=null, es=null, inited=false, host=null, statusEl=null, retries=0;
    function setLive(text, live){
      if(!statusEl) return;
      statusEl.innerHTML = '<span class="dot"></span>'+escapeHtml(text);
      statusEl.classList.toggle('live', !!live);
    }
    function postResize(){
      if(!term) return;
      fetch('/api/term/resize', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ rows: term.rows, cols: term.cols }) }).catch(function(){});
    }
    function sendInput(d){
      fetch('/api/term/input', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ data: d }) }).catch(function(){});
    }
    function doFit(){ if(!fit) return; try{ fit.fit(); postResize(); }catch(e){} }
    function init(){
      host=$('termHost'); statusEl=$('codeTermStatus');
      if(typeof window.Terminal==='undefined'){
        setLive('loading', false);
        if(retries<12){ retries++; setTimeout(function(){ if(state.activeTab==='code') init(); }, 350); }
        return;
      }
      inited=true;
      term = new Terminal({
        allowProposedApi:true, cursorBlink:true, scrollback:5000,
        fontFamily:"'JetBrains Mono', ui-monospace, 'SF Mono', Menlo, Consolas, monospace",
        fontSize:13, lineHeight:1.2, letterSpacing:0,
        theme:{
          background:'#0A0C0B', foreground:'#ECF1EF', cursor:'#3FE08A', cursorAccent:'#0A0C0B',
          selectionBackground:'rgba(63,224,138,.25)',
          black:'#0A0C0B', red:'#F0616D', green:'#3FE08A', yellow:'#E5C07B', blue:'#61AFEF',
          magenta:'#C678DD', cyan:'#56B6C2', white:'#c3ccc9',
          brightBlack:'#5b6a68', brightRed:'#f87171', brightGreen:'#5be3a0', brightYellow:'#fbe08a',
          brightBlue:'#8fc6ff', brightMagenta:'#d9a2ea', brightCyan:'#7fd7e0', brightWhite:'#ECF1EF'
        }
      });
      try{ fit = new FitAddon.FitAddon(); term.loadAddon(fit); }catch(e){ fit=null; }
      term.open(host);
      doFit();
      term.onData(sendInput);
      setLive('connecting', false);
      try{ es = new EventSource('/api/term/stream'); }catch(e){ setLive('unavailable', false); return; }
      es.onopen = function(){ setLive('live', true); };
      es.onmessage = function(e){ var data; try{ data=JSON.parse(e.data); }catch(_){ data=e.data; } term.write(data); };
      es.onerror = function(){ setLive('reconnecting', false); };
      window.addEventListener('resize', function(){ if(state.activeTab==='code') doFit(); });
      if(window.ResizeObserver){ new ResizeObserver(function(){ if(state.activeTab==='code') doFit(); }).observe(host); }
    }
    function activate(){
      if(!inited){ init(); }
      requestAnimationFrame(function(){ doFit(); if(term) term.focus(); });
    }
    return { activate: activate };
  })();

  /* =========================================================
     Modal manager (focus + Esc + scrim close), reused by all overlays
     ========================================================= */
  var activeModal = null;
  function openModal(scrim, panel, focusEl, onClose){
    var last = document.activeElement;
    scrim.hidden = false;
    requestAnimationFrame(function(){ scrim.classList.add('show'); });
    activeModal = { scrim: scrim, panel: panel, last: last, onClose: onClose };
    requestAnimationFrame(function(){ (focusEl || panel).focus(); });
  }
  function closeModal(){
    if(!activeModal) return;
    var m = activeModal; activeModal = null;
    m.scrim.classList.remove('show');
    setTimeout(function(){ m.scrim.hidden = true; }, 200);
    if(typeof m.onClose === 'function'){ try{ m.onClose(); }catch(e){} }
    if(m.last && typeof m.last.focus === 'function'){ try{ m.last.focus(); }catch(e){} }
  }
  function trapTab(e, panel){
    if(e.key !== 'Tab') return;
    var sel = 'a[href],button:not([disabled]),input:not([disabled]),select:not([disabled]),textarea:not([disabled]),[tabindex="0"]';
    var nodes = Array.prototype.slice.call(panel.querySelectorAll(sel)).filter(function(el){ return el.offsetParent !== null; });
    if(!nodes.length) return;
    var first = nodes[0], lastN = nodes[nodes.length - 1];
    if(e.shiftKey && document.activeElement === first){ e.preventDefault(); lastN.focus(); }
    else if(!e.shiftKey && document.activeElement === lastN){ e.preventDefault(); first.focus(); }
  }

  /* =========================================================
     Feature A — Connect to server (MPC)
     ========================================================= */
  var conn = { connected:false, url:'', model:'', streaming:false };
  function connHost(u){
    try{ var h = new URL(u).hostname; if(h) return h; }catch(e){}
    return String(u||'').replace(/^[a-z]+:\/\//i,'').split('/')[0] || 'server';
  }
  function renderConnStatus(){
    var dot=$('sbConnDot'), txt=$('sbConnText'), row=$('sbConn');
    if(!dot || !txt) return;
    var model = currentModelName();
    if(conn.connected){
      dot.className = 'conn-dot is-on';
      txt.textContent = connHost(conn.url) + ' · ' + model;
    } else {
      dot.className = 'conn-dot';
      txt.textContent = 'Local · ' + model;
    }
    if(row) row.title = (conn.connected ? ('Connected to ' + connHost(conn.url)) : 'Local') + ' · model ' + (modelState.selected || model);
  }
  function refreshConn(){
    return apiGet('/api/connect').then(function(d){
      if(d){ conn.connected=!!d.connected; conn.url=d.url||''; if(d.model) conn.model=d.model; conn.streaming=!!d.streaming; }
      renderConnStatus();
      return d;
    }).catch(function(){ renderConnStatus(); });
  }
  function setConnStatus(msg, isErr){
    var el=$('connStatus'); if(!el) return;
    el.textContent = msg || '';
    el.classList.toggle('is-err', !!isErr);
    el.classList.toggle('is-ok', !!msg && !isErr);
  }
  function setConnBusy(busy){
    var btn=$('connSubmit'), dis=$('connDisconnect');
    if(btn){
      btn.disabled = busy;
      btn.setAttribute('aria-busy', busy ? 'true' : 'false');
      btn.innerHTML = busy ? '<span class="spinner"></span><span>Connecting</span>' : 'Connect';
    }
    if(dis) dis.disabled = busy;
  }
  function updateConnModalUI(){
    var dis=$('connDisconnect'); if(dis) dis.hidden = !conn.connected;
  }
  function openConnect(){
    setConnStatus('', false);
    setConnBusy(false);
    var url=$('connUrl'), tok=$('connToken'), eye=$('connEye');
    if(url) url.value = conn.connected ? (conn.url || '') : '';
    if(tok){ tok.value=''; tok.type='password'; }
    if(eye){ eye.innerHTML = icon.eye; eye.setAttribute('aria-label','Show token'); }
    updateConnModalUI();
    openModal($('connScrim'), $('connModal'), $('connUrl'), null);
  }
  function doConnect(){
    if(state.busy){ showToast('Finish the current turn first.', true); return; }
    var url=($('connUrl').value||'').trim();
    var token=$('connToken').value||'';
    if(!url){ setConnStatus('Enter a server URL.', true); $('connUrl').focus(); return; }
    setConnBusy(true); setConnStatus('', false);
    fetch('/api/connect', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ url:url, token:token }) })
      .then(function(res){
        return res.json().catch(function(){ return {}; }).then(function(j){ return { ok:res.ok, status:res.status, j:j }; });
      })
      .then(function(r){
        setConnBusy(false);
        if(r.ok && r.j && r.j.connected){
          conn.connected=true; conn.url=r.j.url||url; if(r.j.model) conn.model=r.j.model; conn.streaming=!!r.j.streaming;
          renderConnStatus();
          refreshModel();
          closeModal();
          showToast('Connected to ' + connHost(conn.url));
        } else {
          setConnStatus((r.j && r.j.error) ? r.j.error : ('Connection failed (' + r.status + ')'), true);
        }
      })
      .catch(function(){ setConnBusy(false); setConnStatus('Network error — could not reach the server.', true); });
  }
  function doDisconnect(){
    if(state.busy){ showToast('Finish the current turn first.', true); return; }
    setConnBusy(true);
    fetch('/api/disconnect', { method:'POST', headers:{'Content-Type':'application/json'}, body:'{}' })
      .then(function(res){ return res.json().catch(function(){ return {}; }); })
      .then(function(j){
        setConnBusy(false);
        conn.connected=false; conn.url='';
        if(j && j.model) conn.model=j.model;
        renderConnStatus();
        refreshModel();
        updateConnModalUI();
        setConnStatus('Disconnected.', false);
        showToast('Disconnected');
      })
      .catch(function(){ setConnBusy(false); setConnStatus('Network error — could not disconnect.', true); showToast('Could not disconnect', true); });
  }
  function wireConnect(){
    $('sbConn').addEventListener('click', openConnect);
    $('connClose').addEventListener('click', closeModal);
    $('connSubmit').addEventListener('click', doConnect);
    $('connDisconnect').addEventListener('click', doDisconnect);
    $('connEye').addEventListener('click', function(){
      var inp=$('connToken'); var show = inp.type==='password';
      inp.type = show ? 'text' : 'password';
      this.innerHTML = show ? icon.eyeOff : icon.eye;
      this.setAttribute('aria-label', show ? 'Hide token' : 'Show token');
      inp.focus();
    });
    ['connUrl','connToken'].forEach(function(id){
      $(id).addEventListener('keydown', function(e){ if(e.key==='Enter'){ e.preventDefault(); doConnect(); } });
    });
    $('connModal').addEventListener('keydown', function(e){ trapTab(e, this); });
    $('connScrim').addEventListener('click', function(e){ if(e.target===this) closeModal(); });
  }

  /* ---------- help ---------- */
  function openHelp(){ openModal($('helpScrim'), $('helpModal'), $('helpClose'), null); }
  function wireHelp(){
    $('helpClose').addEventListener('click', closeModal);
    $('helpModal').addEventListener('keydown', function(e){ trapTab(e, this); });
    $('helpScrim').addEventListener('click', function(e){ if(e.target===this) closeModal(); });
  }

  /* ---------- composer state helpers (reflect in composer UI) ---------- */
  function setComposerWeb(prefix, on){
    var cs=composerState[prefix]; if(!cs) return;
    cs.web = !!on;
    var web=$(prefix+'Web');
    if(web){ web.classList.toggle('is-on', cs.web); web.setAttribute('aria-checked', cs.web?'true':'false'); }
  }
  function setComposerEffort(prefix, val){
    var cs=composerState[prefix]; if(!cs) return;
    cs.effort = val;
    var sel=$(prefix+'Effort');
    if(sel) sel.value = val;
  }

  /* =========================================================
     Feature B — Command palette (Cmd/Ctrl + K)
     ========================================================= */
  var palState = { items: [], filtered: [], sel: 0 };
  function paletteCommands(){
    var tab = state.activeTab, cs = composerState[tab] || composerState.chat;
    var list = [
      { icon:icon.chat,   label:'Go to Chat',  hint:'⌘1', run:function(){ switchTab('chat'); } },
      { icon:icon.folder, label:'Go to Space', hint:'⌘2', run:function(){ switchTab('space'); } },
      { icon:icon.code,   label:'Go to Code',  hint:'⌘3', run:function(){ switchTab('code'); } },
      { icon:icon.plus,   label:'New chat', run:function(){ newChat(); } },
      { icon:icon.server, label:'Connect to server…', run:function(){ openConnect(); } },
      { icon:icon.diamond, label:'Select model…', run:function(){ openModelPicker(); } }
    ];
    if(conn.connected){ list.push({ icon:icon.unplug, label:'Disconnect server', run:function(){ doDisconnect(); } }); }
    list.push({ icon:icon.globe, label:'Web search: On',  hint: cs.web?'active':'', run:function(){ setComposerWeb(tab, true); } });
    list.push({ icon:icon.globe, label:'Web search: Off', hint: cs.web?'':'active', run:function(){ setComposerWeb(tab, false); } });
    [['basic','Basic'],['tuned','Tuned'],['elite','Elite'],['mythic','Mythic'],['beyond','Beyond']].forEach(function(e){
      list.push({ icon:icon.gauge, label:'Effort: '+e[1], hint: cs.effort===e[0]?'active':'', run:function(){ setComposerEffort(tab, e[0]); } });
    });
    list.push({ icon:icon.folder, label:'Change workspace folder…', run:function(){ pickWorkspace(); } });
    list.push({ icon:icon.help,   label:'Help / shortcuts', run:function(){ openHelp(); } });
    return list;
  }
  function fuzzyMatch(label, q){
    if(!q) return true;
    label = label.toLowerCase(); q = q.toLowerCase();
    if(label.indexOf(q) >= 0) return true;
    var i=0;
    for(var j=0;j<label.length && i<q.length;j++){ if(label.charAt(j)===q.charAt(i)) i++; }
    return i === q.length;
  }
  function renderPalette(){
    var list=$('cmdList'), q=($('cmdInput').value||'').trim();
    palState.filtered = palState.items.filter(function(c){ return fuzzyMatch(c.label, q); });
    if(palState.sel >= palState.filtered.length) palState.sel = palState.filtered.length - 1;
    if(palState.sel < 0) palState.sel = 0;
    var html='';
    if(!palState.filtered.length){ html = '<div class="cmd-empty">No matching commands</div>'; }
    for(var i=0;i<palState.filtered.length;i++){
      var c=palState.filtered[i], on=(i===palState.sel);
      html += '<button type="button" class="cmd-item'+(on?' is-sel':'')+'" role="option" id="cmd-opt-'+i+'" '+
        'aria-selected="'+(on?'true':'false')+'" data-idx="'+i+'">'+
        '<span class="cmd-ico">'+c.icon+'</span><span class="cmd-label">'+escapeHtml(c.label)+'</span>'+
        (c.hint ? '<span class="cmd-hint">'+escapeHtml(c.hint)+'</span>' : '')+'</button>';
    }
    list.innerHTML = html;
    var selEl=$('cmd-opt-'+palState.sel);
    if(selEl) selEl.scrollIntoView({ block:'nearest' });
    $('cmdInput').setAttribute('aria-activedescendant', selEl ? ('cmd-opt-'+palState.sel) : '');
  }
  function openPalette(){
    palState.items = paletteCommands();
    palState.sel = 0;
    $('cmdInput').value = '';
    renderPalette();
    openModal($('cmdScrim'), $('cmdModal'), $('cmdInput'), null);
  }
  function runPaletteSel(){
    var c = palState.filtered[palState.sel];
    if(!c) return;
    closeModal();
    requestAnimationFrame(function(){ try{ c.run(); }catch(e){} });
  }
  function wirePalette(){
    $('sbCmd').addEventListener('click', openPalette);
    var input=$('cmdInput'), list=$('cmdList');
    input.addEventListener('input', function(){ palState.sel = 0; renderPalette(); });
    input.addEventListener('keydown', function(e){
      if(e.key==='ArrowDown'){ e.preventDefault(); palState.sel = Math.min(palState.sel + 1, palState.filtered.length - 1); renderPalette(); }
      else if(e.key==='ArrowUp'){ e.preventDefault(); palState.sel = Math.max(palState.sel - 1, 0); renderPalette(); }
      else if(e.key==='Enter'){ e.preventDefault(); runPaletteSel(); }
    });
    list.addEventListener('click', function(e){
      var btn=e.target.closest('.cmd-item'); if(!btn) return;
      palState.sel = parseInt(btn.getAttribute('data-idx'),10) || 0;
      runPaletteSel();
    });
    list.addEventListener('mousemove', function(e){
      var btn=e.target.closest('.cmd-item'); if(!btn) return;
      var idx=parseInt(btn.getAttribute('data-idx'),10);
      if(!isNaN(idx) && idx!==palState.sel){ palState.sel = idx; renderPalette(); }
    });
    $('cmdModal').addEventListener('keydown', function(e){ trapTab(e, this); });
    $('cmdScrim').addEventListener('click', function(e){ if(e.target===this) closeModal(); });
  }

  /* =========================================================
     Feature C — Model picker (local ollama or connected server)
     ========================================================= */
  var modelState = { models: [], selected: '', source: 'local', downloads: [] };
  var dlState = { active: false, model: '', poll: null };
  function shortModel(name){
    var s = String(name==null ? '' : name);
    return s.replace(/:latest$/, '') || s;
  }
  function dlShortName(id){
    var s = String(id==null ? '' : id);
    var i = s.lastIndexOf(':');
    return i >= 0 ? s.slice(i + 1) : s;
  }
  function currentModelName(){ return shortModel(modelState.selected) || conn.model || 'local'; }
  function modelHelpText(){ return modelState.source === 'mpc' ? 'Models on the connected server' : 'Local models (ollama)'; }
  function updateModelLabels(){
    var label = shortModel(modelState.selected) || 'model';
    var names = document.querySelectorAll('.model-btn .mb-name');
    for(var i=0;i<names.length;i++){ names[i].textContent = label; }
    var btns = document.querySelectorAll('.model-btn');
    for(var j=0;j<btns.length;j++){ btns[j].setAttribute('title', 'Model: ' + (modelState.selected || label)); }
  }
  function refreshModel(){
    return apiGet('/api/models').then(function(d){
      if(d){
        modelState.models = Array.isArray(d.models) ? d.models : [];
        if(typeof d.selected === 'string') modelState.selected = d.selected;
        if(d.source) modelState.source = d.source;
        modelState.downloads = (modelState.source === 'local' && Array.isArray(d.downloads)) ? d.downloads : [];
      }
      updateModelLabels();
      renderConnStatus();
      return d;
    }).catch(function(){ updateModelLabels(); });
  }
  function modelOptHTML(fullId, name, isSel, size, canDelete){
    var opt = '<button type="button" class="model-opt'+(isSel?' is-sel':'')+'" role="option" '+
      'aria-selected="'+(isSel?'true':'false')+'" data-model="'+escapeHtml(fullId)+'">'+
      '<span class="mo-name" title="'+escapeHtml(fullId)+'">'+escapeHtml(name)+'</span>'+
      (size ? '<span class="mo-size">'+escapeHtml(size)+'</span>' : '')+
      '<span class="mo-mark">'+(isSel?icon.check:'')+'</span></button>';
    if(!canDelete) return opt;
    return '<div class="model-opt-wrap">'+opt+
      '<button type="button" class="mo-del" data-uninstall="'+escapeHtml(fullId)+'" '+
      'title="Uninstall '+escapeHtml(name)+'" aria-label="Uninstall '+escapeHtml(name)+'">'+icon.trash+'</button></div>';
  }
  function dlRowHTML(id, name, size){
    var isThis = dlState.active && dlState.model && (dlState.model === id || dlShortName(dlState.model) === name);
    var right;
    if(isThis){
      right = '<span class="dl-progress"><span class="dl-spin"></span><span>Downloading&hellip;</span></span>';
    } else {
      right = '<button type="button" class="dl-btn" data-download="'+escapeHtml(id)+'"'+(dlState.active?' disabled':'')+'>'+
        icon.download+'<span>Download</span></button>';
    }
    return '<div class="dl-row" data-dl-id="'+escapeHtml(id)+'">'+
      '<span class="mo-name" title="'+escapeHtml(id)+'">'+escapeHtml(name)+'</span>'+
      (size ? '<span class="mo-size">'+escapeHtml(size)+'</span>' : '')+
      right+'</div>';
  }
  function renderModelList(loading){
    var box = $('modelList'); if(!box) return;
    if(loading){ box.innerHTML = '<div class="model-loading">Loading models&hellip;</div>'; return; }
    var models = modelState.models;
    var downloads = (modelState.source === 'local' && Array.isArray(modelState.downloads)) ? modelState.downloads : [];
    if(!models.length && !downloads.length){ box.innerHTML = '<div class="model-empty">No models found — is ollama running, or connect to a server?</div>'; return; }
    var local = (modelState.source === 'local');
    var sel = shortModel(modelState.selected), html='';
    for(var i=0;i<models.length;i++){
      var full = models[i];
      html += modelOptHTML(full, shortModel(full), shortModel(full) === sel, '', local);
    }
    if(downloads.length){
      html += '<div class="model-section"><div class="model-section-label">Download more</div>';
      for(var k=0;k<downloads.length;k++){
        var d = downloads[k] || {};
        var id = d.model || '', nm = dlShortName(id), size = d.size || '';
        if(d.installed){ html += modelOptHTML(id, nm, shortModel(id) === sel, size, true); }
        else { html += dlRowHTML(id, nm, size); }
      }
      html += '</div>';
    }
    box.innerHTML = html;
  }
  function focusSelectedModelRow(){
    var box=$('modelList'); if(!box) return;
    var sel = box.querySelector('.model-opt.is-sel') || box.querySelector('.model-opt');
    if(sel){ try{ sel.focus(); }catch(e){} }
  }
  function modelModalOpen(){ return !!(activeModal && activeModal.scrim === $('modelScrim')); }
  function openModelPicker(){
    if(state.busy){ showToast('Finish the current turn first.', true); return; }
    $('modelHelp').textContent = modelHelpText();
    renderModelList(true);
    openModal($('modelScrim'), $('modelModal'), $('modelModal'), null);
    refreshModel().then(function(){
      if(!modelModalOpen()) return;
      $('modelHelp').textContent = modelHelpText();
      renderModelList(false);
      focusSelectedModelRow();
    });
  }
  function selectModel(full){
    if(!full) return;
    if(state.busy){ showToast('Finish the current turn first.', true); return; }
    fetch('/api/model', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ model: full }) })
      .then(function(res){ return res.json().catch(function(){ return {}; }).then(function(j){ return { ok:res.ok, status:res.status, j:j }; }); })
      .then(function(r){
        if(r.ok && r.j && r.j.selected){
          modelState.selected = r.j.selected;
          if(r.j.source) modelState.source = r.j.source;
          updateModelLabels();
          renderConnStatus();
          closeModal();
          showToast('Model set to ' + shortModel(modelState.selected));
        } else {
          showToast((r.j && r.j.error) ? r.j.error : ('Could not set model (' + r.status + ')'), true);
        }
      })
      .catch(function(){ showToast('Network error — could not set model.', true); });
  }
  function uninstallModel(full){
    if(!full) return;
    if(state.busy){ showToast('Finish the current turn first.', true); return; }
    if(dlState.active){ showToast('Finish the current download first.', true); return; }
    var name = shortModel(full) || full;
    if(!window.confirm('Remove ' + name + ' from this machine?\n\nThis deletes the downloaded model files (ollama rm).')) return;
    fetch('/api/uninstall', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ model: full }) })
      .then(function(res){ return res.json().catch(function(){ return {}; }).then(function(j){ return { ok:res.ok, status:res.status, j:j }; }); })
      .then(function(r){
        if(r.ok && r.j && r.j.removed){
          if(r.j.selected){ modelState.selected = r.j.selected; updateModelLabels(); renderConnStatus(); }
          showToast('Removed ' + name);
          refreshModel().then(function(){ if(modelModalOpen()) renderModelList(false); });
        } else {
          showToast((r.j && r.j.error) ? r.j.error : ('Could not remove model (' + r.status + ')'), true);
        }
      })
      .catch(function(){ showToast('Network error — could not remove model.', true); });
  }
  function stopDownloadPoll(){ if(dlState.poll){ clearInterval(dlState.poll); dlState.poll = null; } }
  function startDownloadPoll(){ stopDownloadPoll(); dlState.poll = setInterval(pollDownload, 1500); pollDownload(); }
  function pollDownload(){
    apiGet('/api/download_status').then(function(s){
      if(!s) return;
      var status = s.status || '';
      if(s.model) dlState.model = s.model;
      if(status === 'done'){
        stopDownloadPoll();
        var done = dlState.model;
        dlState.active = false; dlState.model = '';
        showToast('Downloaded ' + dlShortName(done));
        refreshModel().then(function(){ if(modelModalOpen()) renderModelList(false); });
      } else if(status === 'error'){
        stopDownloadPoll();
        dlState.active = false; dlState.model = '';
        showToast(s.error ? String(s.error) : 'Download failed.', true);
        if(modelModalOpen()) renderModelList(false);
      } else {
        dlState.active = !!s.active || status === 'downloading';
        if(!dlState.active){ stopDownloadPoll(); if(modelModalOpen()) renderModelList(false); }
      }
    }).catch(function(){});
  }
  function startDownload(fullId){
    if(!fullId) return;
    if(dlState.active){ showToast('A download is already running.', true); return; }
    fetch('/api/download', { method:'POST', headers:{'Content-Type':'application/json'}, body: JSON.stringify({ model: fullId }) })
      .then(function(res){ return res.json().catch(function(){ return {}; }).then(function(j){ return { ok:res.ok, status:res.status, j:j }; }); })
      .then(function(r){
        if(r.status === 409){
          showToast('A download is already running.', true);
          if(!dlState.active){ dlState.active = true; dlState.model = (r.j && r.j.model) || fullId; if(modelModalOpen()) renderModelList(false); startDownloadPoll(); }
          return;
        }
        if(r.ok){
          dlState.active = true; dlState.model = (r.j && r.j.model) || fullId;
          if(modelModalOpen()) renderModelList(false);
          startDownloadPoll();
        } else {
          showToast((r.j && r.j.error) ? r.j.error : ('Could not start download (' + r.status + ')'), true);
        }
      })
      .catch(function(){ showToast('Network error — could not start download.', true); });
  }
  function resumeDownload(){
    apiGet('/api/download_status').then(function(s){
      if(s && (s.active || s.status === 'downloading')){ dlState.active = true; dlState.model = s.model || ''; startDownloadPoll(); }
    }).catch(function(){});
  }
  function onModelListKey(e){
    if(e.key !== 'ArrowDown' && e.key !== 'ArrowUp') return;
    var opts = Array.prototype.slice.call($('modelList').querySelectorAll('.model-opt'));
    if(!opts.length) return;
    e.preventDefault();
    var idx = opts.indexOf(document.activeElement);
    if(e.key === 'ArrowDown'){ idx = (idx < 0) ? 0 : Math.min(idx + 1, opts.length - 1); }
    else { idx = (idx <= 0) ? 0 : idx - 1; }
    opts[idx].focus();
  }
  function wireModel(){
    var btns = document.querySelectorAll('.model-btn');
    for(var i=0;i<btns.length;i++){ btns[i].addEventListener('click', openModelPicker); }
    $('modelClose').addEventListener('click', closeModal);
    $('modelList').addEventListener('click', function(e){
      var del = e.target.closest('.mo-del');
      if(del){ if(!del.disabled) uninstallModel(del.getAttribute('data-uninstall')); return; }
      var dl = e.target.closest('.dl-btn');
      if(dl){ if(!dl.disabled) startDownload(dl.getAttribute('data-download')); return; }
      var opt = e.target.closest('.model-opt'); if(!opt) return;
      selectModel(opt.getAttribute('data-model'));
    });
    $('modelModal').addEventListener('keydown', function(e){ trapTab(e, this); onModelListKey(e); });
    $('modelScrim').addEventListener('click', function(e){ if(e.target===this) closeModal(); });
  }

  /* ---------- global overlay shortcuts (Cmd/Ctrl+K, Esc) ---------- */
  function wireOverlays(){
    document.addEventListener('keydown', function(e){
      if((e.metaKey || e.ctrlKey) && !e.altKey && (e.key==='k' || e.key==='K')){
        e.preventDefault();
        if(activeModal && activeModal.scrim === $('cmdScrim')){ closeModal(); }
        else { if(activeModal) closeModal(); openPalette(); }
        return;
      }
      if(e.key==='Escape' && activeModal){ e.preventDefault(); e.stopPropagation(); closeModal(); }
    });
  }

  /* ---------- init ---------- */
  function init(){
    currentChatId = genId();
    loadChats();
    $('chatComposerWrap').innerHTML = composerHTML('chat');
    $('spaceComposerWrap').innerHTML = composerHTML('space');
    $('codeComposerWrap').innerHTML = composerHTML('code');
    VIEWS.forEach(wireComposer);
    wireTabs(); wireWorkspace(); wireSidebar();
    wireConnect(); wireHelp(); wirePalette(); wireModel(); wireOverlays();
    applyWorkspaceUI(); renderSidebar();
    renderConnStatus(); refreshConn(); refreshModel(); resumeDownload();
    refreshHistory().then(function(){ if(state.busy) startPolling(); });
  }
  if(document.readyState==='loading') document.addEventListener('DOMContentLoaded', init); else init();
})();
</script>
</body>
</html>
)DASH";

}
