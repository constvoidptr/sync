#pragma once

// https://www.euroscope.hu/wp/non-standard-extensions/
// https://vatsim-scandinavia.org/forums/topic/4664-ground-radar-plugin-dev-question/

#include <optional>
#include <string>

namespace tracked {
    enum class Status {
        NoState,
        OnFreq,
        Stby,
        Stup,
        Push,
        Taxi,
        Lineup,
        Takeoff,
    };

    std::optional<Status> status_try_from(const std::string& value);
    std::string to_string(const Status& status);

    enum class Clearance {
        Cleared,
        NotCleared,
    };

    std::optional<Clearance> clearance_try_from(const std::string& value);
    std::string to_string(const Clearance& status);

    struct Tracked {
        bool cleared = false;
        std::optional<Status> status = std::nullopt;
    };
}
