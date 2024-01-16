#pragma once

#include <euroscope/EuroScopePlugIn.h>
#include <string>
#include <unordered_map>

#include "status.h"

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

    bool OnCompileCommand(const char* sCommandLine) override;

    void OnFlightPlanControllerAssignedDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan, int DataType) override;

    /// Custom Ground Radar states that are not part of EuroScope, so we need to keep track of them ourselves.
    std::unordered_map<std::string, status::Status> status;
};