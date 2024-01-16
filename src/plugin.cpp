#include "plugin.h"

#include <algorithm>
#include <format>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

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
    DisplayUserMessage(PLUGIN_NAME.c_str(), PLUGIN_NAME.c_str(), msg.c_str(), true, false, false, false, false);
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

    for (EuroScopePlugIn::CFlightPlan flight_plan = FlightPlanSelectFirst(); flight_plan.IsValid(); flight_plan = FlightPlanSelectNext(
            flight_plan)) {

        if (!filter(airport, flight_plan))
            continue;

        println(std::format("Syncing: {}, got clearance: {}, with ground state: {}", flight_plan.GetCallsign(),
                            flight_plan.GetClearenceFlag(), flight_plan.GetGroundState()));
    }


    /*
    auto assigned = fp.GetControllerAssignedData();
    this->println(std::format("Setting clearance flag for: {}", fp.GetCallsign()));
    auto old = assigned.GetScratchPadString();
    this->println(std::format("Old: {}", old));
    assigned.SetScratchPadString("CLEA");
    assigned.SetScratchPadString(old);
    */

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
        println(std::format("New status: {} for: {}", status::to_string(*s), FlightPlan.GetCallsign()));
    }
}
