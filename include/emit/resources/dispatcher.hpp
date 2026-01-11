#pragma once

#ifdef EMIT_SUPPORTED

#include "../utility/generic_pipeline.hpp"
#include "../utility/type_index.hpp"

namespace emit {
    class dispatcher {
        using generic_pipeline = detail::generic_pipeline;

    public:
        dispatcher() = default;
        ~dispatcher() = default;

        dispatcher(const dispatcher&) = delete;
        dispatcher(dispatcher&&) noexcept = default;

        dispatcher& operator=(const dispatcher&) = delete;
        dispatcher& operator=(dispatcher&&) noexcept = default;

        template <typename T>
        [[nodiscard]] pipeline<T>& acquire() {
            std::size_t index = detail::type_index<T>();

            if (index >= delegates_.size()) {
                delegates_.resize(index + 1);
            }

            generic_pipeline& delegate = delegates_[index];

            if (!delegate) {
                delegate = generic_pipeline::create<T>();
            }

            return delegate.acquire<T>();
        }

        void clear() {
            for (generic_pipeline& delegate : delegates_) {
                if (delegate) {
                    delegate.clear();
                }
            }
        }

        void dispatch() {
            for (generic_pipeline& delegate : delegates_) {
                if (delegate) {
                    delegate.dispatch();
                }
            }
        }

    private:
        std::vector<generic_pipeline> delegates_;
    };
}

#endif