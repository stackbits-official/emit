#pragma once

#ifdef EMIT_SUPPORTED

#include "../detail/pipeline_delegate.hpp"
#include "../detail/type_index.hpp"

namespace emit {
    class dispatcher {
        using pipeline_delegate = detail::pipeline_delegate;

    public:
        dispatcher() = default;
        ~dispatcher() = default;

        dispatcher(const dispatcher&) = delete;
        dispatcher(dispatcher&&) noexcept = default;

        dispatcher& operator=(const dispatcher&) = delete;
        dispatcher& operator=(dispatcher&&) noexcept = delete;

        template <typename T>
        [[nodiscard]] pipeline<T>& acquire() {
            std::size_t index = detail::type_index<T>();

            if (index >= delegates_.size()) {
                delegates_.resize(index + 1);
            }

            pipeline_delegate& delegate = delegates_[index];

            if (!delegate) {
                delegate = pipeline_delegate::create<T>();
            }

            return delegate.acquire<T>();
        }

        void clear() {
            for (pipeline_delegate& delegate : delegates_) {
                if (delegate) {
                    delegate.clear();
                }
            }
        }

        void dispatch() {
            for (pipeline_delegate& delegate : delegates_) {
                if (delegate) {
                    delegate.dispatch();
                }
            }
        }

    private:
        std::vector<pipeline_delegate> delegates_;
    };
}

#endif