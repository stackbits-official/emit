#pragma once

#ifdef EMIT_SUPPORTED

namespace emit {
    template <typename T>
    class delegate;

    template <typename T, typename... Args>
    class delegate<T(Args...)> {

    public:
        delegate() = default;
        ~delegate() = default;

        delegate(const delegate&) = default;
        delegate(delegate&) noexcept = default;

        delegate& operator=(const delegate&) = default;
        delegate& operator=(delegate&&) noexcept = default;

        template <auto Fn, typename U>
        void connect(U& instance) {
            instance_ = &instance;
            function_ = member_invoke<Fn, U>;
        }

        template <auto Fn>
        void connect() {
            instance_ = nullptr;
            function_ = free_invoke<Fn>;
        }

        [[nodiscard]] bool operator==(const delegate& other) const {
            return instance_ == other.instance_ && function_ == other.function_;
        }

        [[nodiscard]] bool operator!=(const delegate& other) const {
            return !(*this == other);
        }

        T operator()(Args... args) const {
            function_(instance_, args...);
        }

        [[nodiscard]] explicit operator bool() const {
            return function_ != nullptr;
        }

    private:
        void* instance_ = nullptr;
        T (*function_)(void*, Args...) = nullptr;

        template <auto Fn, typename U>
        static T member_invoke(void* instance, Args... args) {
            return (reinterpret_cast<U*>(instance)->*Fn)(args...);
        }

        template <auto Fn>
        static T free_invoke(void*, Args... args) {
            return Fn(args...);
        }
    };
}

#endif