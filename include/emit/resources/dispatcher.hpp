#pragma once

#ifdef EMIT_SUPPORTED

#include "../utility/type_index.hpp"
#include "generic_pipeline.hpp"

namespace emit {
    /**
     * @brief Connects listeners to events of any type
     *
     * A dispatcher can either trigger an immediate event or enqueue events for future processing.
     * Listeners are provided as member functions of an object or freestanding functions.
     */
    class dispatcher {
        using generic_pipeline = internal::generic_pipeline;

    public:
        dispatcher() = default;
        ~dispatcher() = default;

        dispatcher(const dispatcher&) = delete;
        dispatcher(dispatcher&&) noexcept = default;

        dispatcher& operator=(const dispatcher&) = delete;
        dispatcher& operator=(dispatcher&&) noexcept = default;

        /**
         * @brief Acquires a pipeline of the requested type.
         * @tparam T The event type of the desired pipeline.
         * @return A reference to the requested pipeline.
         */
        template <typename T>
        [[nodiscard]] pipeline<T>& acquire() {
            std::size_t index = internal::type_index<T>();

            if (index >= delegates_.size()) {
                delegates_.resize(index + 1);
            }

            generic_pipeline& delegate = delegates_[index];

            if (!delegate) {
                delegate = generic_pipeline::create<T>();
            }

            return delegate.acquire<T>();
        }

        /**
         * @brief Clears all listeners and events from all pipelines
         */
        void clear() {
            for (generic_pipeline& delegate : delegates_) {
                if (delegate) {
                    delegate.clear();
                }
            }
        }

        /**
         * @brief Dispatches all pipelines to process their queued events.
         */
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