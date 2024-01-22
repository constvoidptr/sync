#include "plugin.h"

#include <algorithm>
#include <format>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

const char* ABOUT_MSG = "Synchronize the Departure List with other controllers.";
const char* HELP_MSG = "Usage: .sync <Airport ICAO> (e.g. .sync LOWW).";

/// Filter out aircraft that are not on the ground at the specified airport.
/// Returns true if we should keep the aircraft, false otherwise.
bool filter(const std::string& airport, const EuroScopePlugIn::CFlightPlan& flight_plan) {
    EuroScopePlugIn::CFlightPlanData flight_plan_data = flight_plan.GetFlightPlanData();
    if (flight_plan_data.GetOrigin() != airport)
        return false;

    if (flight_plan.GetDistanceFromOrigin() > 5.0)
        return false;

    // Filter out aircraft that are not on the ground
    EuroScopePlugIn::CRadarTarget target = flight_plan.GetCorrelatedRadarTarget();
    if (!target.IsValid())
        return false;

    if (target.GetGS() > 60)
        return false;

    const char* tracking_controller = flight_plan.GetTrackingControllerCallsign();
    if (tracking_controller && *tracking_controller)
        return false;

    return true;
}

// Plugin singleton :(
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
    println(ABOUT_MSG);
    println(HELP_MSG);
}

void Plugin::println(const std::string& msg) {
    DisplayUserMessage(PLUGIN_NAME.c_str(), PLUGIN_NAME.c_str(), msg.c_str(), true, true, false, false, false);
}

bool Plugin::OnCompileCommand(const char* sCommandLine) {
    // Command parsing
    std::vector<std::string> args;
    std::istringstream stream(sCommandLine);
    for (std::string arg; std::getline(stream, arg, ' ');) {
        if (!arg.empty())
            args.emplace_back(arg);
    }

    if (args.size() < 2)
        return false;

    if (args[0] != ".sync")
        return false;

    if (args[1] == "help") {
        println(HELP_MSG);
        return true;
    }

    if (args[1] == "debug") {
        println(std::format("Number of stored statuses: {}", this->statuses.size()));
        return true;
    }

    if (args[1] == "clear") {
        println("Clearing out stored statuses.");
        this->statuses.clear();
        return true;
    }

    // Convert the airport to uppercase
    std::string airport = args[1];
    if (airport.size() != 4)
        return false;
    std::transform(airport.begin(), airport.end(), airport.begin(), ::toupper);

    // Sync the aircraft
    uint32_t num_synced = sync(airport);
    println(std::format("Synced {} aircraft in {}", num_synced, airport));

    return true;
}

void Plugin::OnFlightPlanControllerAssignedDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan, int DataType) {
    using status::Status;

    std::optional<Status> parsed_status;
    switch (DataType) {
        case EuroScopePlugIn::CTR_DATA_TYPE_SCRATCH_PAD_STRING:
            parsed_status = status::try_from(FlightPlan.GetControllerAssignedData().GetScratchPadString());
            break;
        case EuroScopePlugIn::CTR_DATA_TYPE_GROUND_STATE:
            parsed_status = status::try_from(FlightPlan.GetGroundState());
            break;
        default:
            return;
    }

    if (std::optional<Status> s = parsed_status) {
        if (*s == Status::NoState) {
            this->statuses.erase(FlightPlan.GetCallsign());
            return;
        }

        this->statuses.insert_or_assign(FlightPlan.GetCallsign(), *s);
    }
}

void Plugin::OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan FlightPlan) {
    // Remove the status of the aircraft when it disconnects or is out of range.
    this->statuses.erase(FlightPlan.GetCallsign());
}

uint32_t Plugin::sync(const std::string& airport) {
    uint32_t num_synced = 0;
    for (EuroScopePlugIn::CFlightPlan flight_plan = FlightPlanSelectFirst(); flight_plan.IsValid(); flight_plan = FlightPlanSelectNext(
            flight_plan)) {

        if (!filter(airport, flight_plan))
            continue;

        bool modified = false;
        EuroScopePlugIn::CFlightPlanControllerAssignedData assigned = flight_plan.GetControllerAssignedData();
        std::string scratch_pad_backup = assigned.GetScratchPadString();

        // Sync clearance received flag
        if (flight_plan.GetClearenceFlag()) {
            assigned.SetScratchPadString("CLEA");
            modified = true;
        }

        // Sync status
        if (auto status = this->statuses.find(flight_plan.GetCallsign()); status != this->statuses.end()) {
            assigned.SetScratchPadString(status::to_string(status->second).c_str());
            modified = true;
        }

        // Restore the scratch pad if we modified it
        if (modified) {
            assigned.SetScratchPadString(scratch_pad_backup.c_str());
            num_synced += 1;
        }
    }

    return num_synced;
}
