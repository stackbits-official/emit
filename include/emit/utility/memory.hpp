#pragma once

#ifdef EMIT_SUPPORTED

#include <cstddef>
#include <cstdlib>

namespace emit::detail {
    template <std::size_t S, std::size_t A>
    class memory {
    private:
        alignas(A) std::byte data_[S];
    };

    template <typename T>
    using memory_of = memory<sizeof(T), alignof(T)>;
}

#endif