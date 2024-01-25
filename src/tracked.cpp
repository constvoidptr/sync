#include "tracked.h"

namespace tracked {
    std::optional<Status> status_try_from(const std::string& value) {
        if (value == "NOSTATE" || value == "NSTS") return Status::NoState;
        if (value == "ONFREQ") return Status::OnFreq;
        if (value == "DE-ICE") return Status::Stby;
        if (value == "ST-UP" || value == "STUP") return Status::Stup;
        if (value == "PUSH") return Status::Push;
        if (value == "TAXI" || value == "TXIN") return Status::Taxi;
        if (value == "LINEUP") return Status::Lineup;
        if (value == "DEPA") return Status::Takeoff;
        return std::nullopt;
    }

    std::string to_string(const Status& status) {
        switch (status) {
            case Status::NoState:
                return "NOSTATE";
            case Status::OnFreq:
                return "ONFREQ";
            case Status::Stby:
                return "DE-ICE";
            case Status::Stup:
                return "ST-UP";
            case Status::Push:
                return "PUSH";
            case Status::Taxi:
                return "TAXI";
            case Status::Lineup:
                return "LINEUP";
            case Status::Takeoff:
                return "DEPA";
        }

        // This should never be reached
        return "UNKNOWN";
    }

    std::optional<Clearance> clearance_try_from(const std::string& value) {
        if (value == "CLEA") return Clearance::Cleared;
        if (value == "NOTC") return Clearance::NotCleared;
        return std::nullopt;
    }

    std::string to_string(const Clearance& status) {
        switch (status) {
            case Clearance::Cleared:
                return "CLEA";
            case Clearance::NotCleared:
                return "NOTC";
        }

        // This should never be reached
        return "UNKNOWN";
    }
}
