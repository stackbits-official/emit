#pragma once

#ifdef EMIT_SUPPORTED

namespace emit::detail {
    template <typename T>
    class connection {
    public:
        connection() = default;
        ~connection() = default;

        connection(const connection&) = default;
        connection(connection&) noexcept = default;

        connection& operator=(const connection&) = default;
        connection& operator=(connection&&) noexcept = default;

        template <auto Fn, typename U>
        [[nodiscard]] static connection create(U& instance) {
            connection connection;

            connection.instance_ = &instance;
            connection.function_ = member_invoke<Fn, U>;

            return connection;
        }

        template <auto Fn>
        [[nodiscard]] static connection create() {
            connection connection;

            connection.instance_ = nullptr;
            connection.function_ = free_invoke<Fn>;

            return connection;
        }

        [[nodiscard]] bool operator==(const connection& other) const {
            return instance_ == other.instance_ && function_ == other.function_;
        }

        [[nodiscard]] bool operator!=(const connection& other) const {
            return !(*this == other);
        }

        void operator()(const T& event) const {
            function_(instance_, event);
        }

        [[nodiscard]] explicit operator bool() const {
            return function_ != nullptr;
        }

    private:
        void* instance_ = nullptr;
        void (*function_)(void*, const T&) = nullptr;

        template <auto Fn, typename U>
        static void member_invoke(void* instance, const T& event) {
            (reinterpret_cast<U*>(instance)->*Fn)(event);
        }

        template <auto Fn>
        static void free_invoke(void*, const T& event) {
            Fn(event);
        }
    };
}

#endif