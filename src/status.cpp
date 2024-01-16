#include "status.h"

namespace status {
    std::optional<Status> try_from(const std::string& status) {
        if (status == "NOSTATE" || status == "NSTS") return Status::NoState;
        if (status == "ONFREQ") return Status::OnFreq;
        if (status == "DE-ICE") return Status::Stby;
        if (status == "ST-UP" || status == "STUP") return Status::Stup;
        if (status == "PUSH") return Status::Push;
        if (status == "TAXI" || status == "TXIN") return Status::Taxi;
        if (status == "LINEUP") return Status::Lineup;
        if (status == "DEPA") return Status::Takeoff;
        return std::nullopt;
    }

    std::string to_string(const Status& status) {
        switch (status) {
            case Status::NoState: return "NOSTATE";
            case Status::OnFreq: return "ONFREQ";
            case Status::Stby: return "STBY";
            case Status::Stup: return "STUP";
            case Status::Push: return "PUSH";
            case Status::Taxi: return "TAXI";
            case Status::Lineup: return "LINEUP";
            case Status::Takeoff: return "TAKEOFF";
        }

        return "UNKNOWN";
    }
}
