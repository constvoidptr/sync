#pragma once

// https://www.euroscope.hu/wp/non-standard-extensions/
// https://vatsim-scandinavia.org/forums/topic/4664-ground-radar-plugin-dev-question/

#include <optional>
#include <string>

namespace status {
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

    std::optional<Status> try_from(const std::string& status);

    std::string to_string(const Status& status);
}
