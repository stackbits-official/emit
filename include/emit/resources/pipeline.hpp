#pragma once

#ifdef EMIT_SUPPORTED

#include "delegate.hpp"
#include <vector>

namespace emit {
    /**
     * @brief Connects listeners to their events.
     *
     * Provides functionality for connect/disconnect of listeners.
     * Also allows immediate dispatch and queued processing of events.
     *
     * @tparam T The event type the pipeline is intended to process.
     */
    template <typename T>
    class pipeline {
        using delegate = delegate<void(const T&)>;

    public:
        pipeline() = default;
        ~pipeline() = default;

        pipeline(const pipeline&) = delete;
        pipeline(pipeline&&) noexcept = default;

        pipeline& operator=(const pipeline&) = delete;
        pipeline& operator=(pipeline&&) noexcept = default;

        /**
         * @brief Connects a member function listener to the pipeline.
         *
         * Does not protect against duplicate connections.
         *
         * @tparam Fn The function of the object to connect to.
         * @tparam U The type of the object instance.
         * @param instance Reference to the object instance.
         */
        template <auto Fn, typename U>
        void connect(U& instance) {
            delegate& delegate = delegates_.emplace_back();

            delegate.template connect<Fn, U>(instance);
        }

        /**
         * @brief Connects a freestanding function listener to the pipeline.
         *
         * Does not protect against duplicate connections.
         *
         * @tparam Fn The function or lambda to connect to.
         */
        template <auto Fn>
        void connect() {
            delegate& delegate = delegates_.emplace_back();

            delegate.template connect<Fn>();
        }

        /**
         * @brief Disconnects a member function listener from the pipeline.
         *
         * Removes the last listener that matches the input parameters.
         *
         * @tparam Fn The function of the object to disconnect.
         * @tparam U The type of the object instance.
         * @param instance Reference to the object instance.
         */
        template <auto Fn, typename U>
        void disconnect(U& instance) {
            delegate target;

            target.template connect<Fn, U>(instance);

            remove(target);
        }

        /**
         * @brief Disconnects a freestanding function listener from the pipeline.
         *
         * Removes the last listener that matches the input parameters.
         *
         * @tparam Fn The function or lambda to disconnect.
         */
        template <auto Fn>
        void disconnect() {
            delegate target;

            target.template connect<Fn>();

            remove(target);
        }

        /**
         * @brief Identifies if a listener that matches the provided signature already exists.
         * @tparam Fn The function of the object to find.
         * @tparam U The type of the object instance.
         * @param instance Reference to the object instance.
         */
        template <auto Fn, typename U>
        [[nodiscard]] bool connected(U& instance) const noexcept {
            delegate target;

            target.template connect<Fn, U>(instance);

            return contains(target);
        }

        /**
         * @brief Identifies if a listener that matches the provided signature already exists.
         * @tparam Fn The function of the object to find.
         */
        template <auto Fn>
        [[nodiscard]] bool connected() const noexcept {
            delegate target;

            target.template connect<Fn>();

            return contains(target);
        }

        /**
         * @brief Enqueues an event for future processing.
         * @tparam Args The types of the event's construction parameters.
         * @param args The values needed to construct the event.
         */
        template <typename... Args>
        void enqueue(Args&&... args) {
            queue_.emplace_back(std::forward<Args>(args)...);
        }

        /**
         * @brief Immediately dispatches all listeners with the provided event.
         * @tparam Args The types of the event's construction parameters.
         * @param args The values needed to construct the event.
         */
        template <typename... Args>
        void trigger(Args&&... args) {
            T event = T(std::forward<Args>(args)...);

            for (const delegate& delegate : delegates_) {
                delegate(event);
            }
        }

        /**
         * @brief Dispatches all listeners with the queued events.
         *
         * Automatically clears the event queue after processing is complete.
         */
        void dispatch() {
            for (const T& event : queue_) {
                for (const delegate& delegate : delegates_) {
                    delegate(event);
                }
            }

            queue_.clear();
        }

        /**
         * @brief Clears all listeners and queued events.
         */
        void clear() {
            delegates_.clear();
            queue_.clear();
        }

    private:
        std::vector<T> queue_;
        std::vector<delegate> delegates_;

        [[nodiscard]] bool contains(const delegate& target) const noexcept {
            for (std::size_t i = delegates_.size(); i-- > 0;) {
                const delegate& candidate = delegates_[i];

                if (candidate == target) {
                    return true;
                }
            }

            return false;
        }

        void remove(const delegate& target) noexcept {
            for (std::size_t i = delegates_.size(); i-- > 0;) {
                delegate& candidate = delegates_[i];

                if (candidate == target) {
                    std::swap(candidate, delegates_.back());
                    delegates_.pop_back();

                    break;
                }
            }
        }
    };
}

#endif