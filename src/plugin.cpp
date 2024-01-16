#include "plugin.h"

#include <algorithm>
#include <format>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

/// Filter out aircraft that are not on the ground at the specified airport.
/// Returns true if we should keep the aircraft, false otherwise.
bool filter(const std::string& airport, const EuroScopePlugIn::CFlightPlan& flight_plan) {
    auto flight_plan_data = flight_plan.GetFlightPlanData();
    if (flight_plan_data.GetOrigin() != airport)
        return false;

    if (flight_plan.GetDistanceFromOrigin() > 2.5)
        return false;

    // Check ground speed less than 60 knots
    auto target = flight_plan.GetCorrelatedRadarTarget();
    if (!target.IsValid())
        return false;

    if (target.GetGS() > 60)
        return false;

    const char* tracking_controller = flight_plan.GetTrackingControllerCallsign();
    if (tracking_controller && *tracking_controller)
        return false;

    return true;
}

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
    DisplayUserMessage(PLUGIN_NAME.c_str(), PLUGIN_NAME.c_str(), msg.c_str(), true, true, false, false, false);
}

bool Plugin::OnCompileCommand(const char* sCommandLine) {
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

    std::string airport = args[1];
    std::transform(airport.begin(), airport.end(), airport.begin(), ::toupper);
    if (airport.size() != 4)
        return false;

    uint32_t num_synced = 0;
    for (EuroScopePlugIn::CFlightPlan flight_plan = FlightPlanSelectFirst(); flight_plan.IsValid(); flight_plan = FlightPlanSelectNext(
            flight_plan)) {

        if (!filter(airport, flight_plan))
            continue;

        bool modified = false;
        auto assigned = flight_plan.GetControllerAssignedData();
        auto cached_scratch_pad = assigned.GetScratchPadString();

        if (flight_plan.GetClearenceFlag()) {
            assigned.SetScratchPadString("CLEA");
            modified = true;
        }

        auto status = this->status.find(flight_plan.GetCallsign());
        if (status != this->status.end()) {
            assigned.SetScratchPadString(status::to_string(status->second).c_str());
            modified = true;
        }

        if (modified) {
            assigned.SetScratchPadString(cached_scratch_pad);
            num_synced += 1;
        }
    }

    println(std::format("Synced {} aircraft in {}", num_synced, airport));

    return true;
}

void Plugin::OnFlightPlanControllerAssignedDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan, int DataType) {
    using status::Status;

    std::optional<Status> parsed_status = std::nullopt;
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
            this->status.erase(FlightPlan.GetCallsign());
            return;
        }

        this->status.insert_or_assign(FlightPlan.GetCallsign(), *s);
    }
}
