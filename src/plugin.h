#pragma once

#include <euroscope/EuroScopePlugIn.h>
#include <string>
#include <unordered_map>

#include "status.h"

const std::string PLUGIN_NAME = "Sync";
const std::string PLUGIN_VERSION = "1.0.0";
const std::string PLUGIN_AUTHOR = "constvoidptr";
const std::string PLUGIN_COPYRIGHT = "(c) 2023-2024, MIT License";

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

    void OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan FlightPlan) override;

    /// Sync all aircraft at a given airport.
    /// Returns the number of aircraft synced.
    uint32_t sync(const std::string& airport);

    /// The GroundRadar plugin states are not being tracked by EuroScope, so we need to keep track of all status changes ourselves.
    std::unordered_map<std::string, status::Status> statuses;
};