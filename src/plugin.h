#pragma once

#include <euroscope/EuroScopePlugIn.h>
#include <string>

const std::string PLUGIN_NAME = "Sync";
const std::string PLUGIN_VERSION = "0.0.1";
const std::string PLUGIN_AUTHOR = "constvoidptr";
const std::string PLUGIN_COPYRIGHT = "MIT";

class Plugin : public EuroScopePlugIn::CPlugIn {
public:
    static Plugin& instance();

    static void reset();

    Plugin(const Plugin&) = delete;

    Plugin& operator=(const Plugin&) = delete;

    void println(const std::string& msg);

private:
    Plugin();

public:
    bool OnCompileCommand(const char* sCommandLine) override;
};