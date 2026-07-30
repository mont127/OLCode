// Dashboard server entry points and the embedded web asset symbols.

#pragma once

namespace ocli {

class LOREA;

bool start_dashboard(LOREA& agent, int port = 8730);

extern const char* DASHBOARD_HTML;

extern const char* XTERM_JS;
extern const char* XTERM_CSS;
extern const char* XTERM_FIT_JS;

}
