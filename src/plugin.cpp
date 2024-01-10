#include "plugin.h"

static Plugin* INSTANCE = nullptr;

Plugin& Plugin::instance() {
    if (!INSTANCE)
        INSTANCE = new Plugin();
    return *INSTANCE;
}

void Plugin::reset() {
    if (!INSTANCE)
        return;

    delete INSTANCE;
    INSTANCE = nullptr;
}

Plugin::Plugin() : EuroScopePlugIn::CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE,
                                            PLUGIN_NAME.c_str(),
                                            PLUGIN_VERSION.c_str(),
                                            PLUGIN_AUTHOR.c_str(),
                                            PLUGIN_COPYRIGHT.c_str()) {
}

void Plugin::println(const std::string& msg) {
    DisplayUserMessage(PLUGIN_NAME.c_str(), PLUGIN_NAME.c_str(), msg.c_str(), true, false, false, false, false);
}

bool Plugin::OnCompileCommand(const char* sCommandLine) {
    return CPlugIn::OnCompileCommand(sCommandLine);
}
