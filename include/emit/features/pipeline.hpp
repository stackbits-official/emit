#pragma once

#ifdef EMIT_SUPPORTED

#include "../detail/connection.hpp"

#include <vector>

namespace emit {
    template <typename T>
    class pipeline {
        using connection = detail::connection<T>;

    public:
        pipeline() = default;
        ~pipeline() = default;

        pipeline(const pipeline&) = delete;
        pipeline(pipeline&&) noexcept = default;

        pipeline& operator=(const pipeline&) = delete;
        pipeline& operator=(pipeline&&) noexcept = default;

        template <typename U, auto Fn>
        void connect(U& instance) {
            connections_.push_back(connection::template create<U, Fn>(instance));
        }

        template <auto Fn>
        void connect() {
            connections_.push_back(connection::template create<Fn>());
        }

        template <typename U, auto Fn>
        void disconnect(U& instance) {
            connection target = connection::template create<U, Fn>(instance);

            remove(target);
        }

        template <auto Fn>
        void disconnect() {
            connection target = connection::template create<Fn>();

            remove(target);
        }

        template <typename... Args>
        void enqueue(Args&&... args) {
            queue_.emplace_back(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void trigger(Args&&... args) {
            T event = T(std::forward<Args>(args)...);

            for (const connection& connection : connections_) {
                connection(event);
            }
        }

        void dispatch() {
            for (const T& event : queue_) {
                for (const connection& connection : connections_) {
                    connection(event);
                }
            }

            queue_.clear();
        }

        void clear() {
            connections_.clear();
            queue_.clear();
        }

    private:
        std::vector<T> queue_;
        std::vector<connection> connections_;

        void remove(const connection& target) {
            for (std::size_t i = connections_.size(); i-- > 0;) {
                connection& candidate = connections_[i];

                if (candidate == target) {
                    std::swap(candidate, connections_.back());
                    connections_.pop_back();
                }
            }
        }
    };
}

#endif