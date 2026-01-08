#pragma once

#ifdef EMIT_SUPPORTED

#include <cstdlib>

namespace emit::detail {
    [[nodiscard]] inline std::size_t increment() {
        static std::size_t n = 0;

        return n++;
    }

    template <typename>
    [[nodiscard]] std::size_t type_index() {
        static std::size_t index = increment();

        return index;
    }
}

#endif