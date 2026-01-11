#pragma once

#ifdef EMIT_SUPPORTED

#include "../resources/delegate.hpp"

#include <vector>

namespace emit {
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

        template <auto Fn, typename U>
        void connect(U& instance) {
            delegate& delegate = delegates_.emplace_back();

            delegate.template connect<Fn, U>(instance);
        }

        template <auto Fn>
        void connect() {
            delegate& delegate = delegates_.emplace_back();

            delegate.template connect<Fn>();
        }

        template <auto Fn, typename U>
        void disconnect(U& instance) {
            delegate target;

            target.template connect<Fn, U>(instance);

            remove(target);
        }

        template <auto Fn>
        void disconnect() {
            delegate target;

            target.template connect<Fn>();

            remove(target);
        }

        template <typename... Args>
        void enqueue(Args&&... args) {
            queue_.emplace_back(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void trigger(Args&&... args) {
            T event = T(std::forward<Args>(args)...);

            for (const delegate& delegate : delegates_) {
                delegate(event);
            }
        }

        void dispatch() {
            for (const T& event : queue_) {
                for (const delegate& delegate : delegates_) {
                    delegate(event);
                }
            }

            queue_.clear();
        }

        void clear() {
            delegates_.clear();
            queue_.clear();
        }

    private:
        std::vector<T> queue_;
        std::vector<delegate> delegates_;

        void remove(const delegate& target) {
            for (std::size_t i = delegates_.size(); i-- > 0;) {
                delegate& candidate = delegates_[i];

                if (candidate == target) {
                    std::swap(candidate, delegates_.back());
                    delegates_.pop_back();
                }
            }
        }
    };
}

#endif