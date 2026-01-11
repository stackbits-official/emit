#pragma once

#ifdef EMIT_SUPPORTED

#include <tuple>

namespace emit {
    template <typename T>
    class delegate;

    /**
     * @brief Agnostically represents a callable function.
     *
     * Can reference freestanding functions or member functions of an instantiated class.
     *
     * @tparam Ret Return type of the referenced function.
     * @tparam Args Parameter types of the referenced function.
     */
    template <typename Ret, typename... Args>
    class delegate<Ret(Args...)> {
        using instance_type = void*;
        using function_type = Ret (*)(instance_type, Args...);

    public:
        using return_type = Ret;
        using parameter_types = std::tuple<Args...>;
        using signature = Ret(Args...);

        delegate() noexcept = default;
        ~delegate() noexcept = default;

        delegate(const delegate&) = default;
        delegate(delegate&) noexcept = default;

        delegate& operator=(const delegate&) = default;
        delegate& operator=(delegate&&) noexcept = default;

        /**
         * @brief Connects the delegate to a member function of an object instance.
         * @tparam Fn The function of the object to connect to.
         * @tparam U The type of the object instance.
         * @param instance Reference to the object instance.
         */
        template <auto Fn, typename U>
        void connect(U& instance) noexcept {
            instance_ = &instance;
            function_ = invoke<Fn, U>;
        }

        /**
         * @brief Connects the delegate to a freestanding function or lambda.
         * @tparam Fn The function or lambda to connect to.
         */
        template <auto Fn>
        void connect() noexcept {
            instance_ = nullptr;
            function_ = invoke<Fn>;
        }

        /**
         * @brief Compares the contents of two delegates for equality.
         * @param other The RHS of the operation.
         */
        [[nodiscard]] bool operator==(const delegate& other) const noexcept {
            return instance_ == other.instance_ && function_ == other.function_;
        }

        /**
         * @brief Compares the contents of two delegates for inequality.
         * @param other The RHS of the operation.
         */
        [[nodiscard]] bool operator!=(const delegate& other) const noexcept {
            return !(*this == other);
        }

        /**
         * @brief Invokes the delegated function.
         * @param args Arguments of the function call.
         * @return The result of calling the delegated function.
         */
        Ret operator()(Args... args) const noexcept {
            function_(instance_, args...);
        }

        /**
         * @brief Indicates if the delegate is valid.
         * @return If the stored function pointer is valid.
         */
        [[nodiscard]] explicit operator bool() const noexcept {
            return function_ != nullptr;
        }

    private:
        instance_type instance_ = nullptr;
        function_type function_ = nullptr;

        template <auto Fn, typename U>
        static Ret invoke(instance_type instance, Args... args) {
            return (reinterpret_cast<U*>(instance)->*Fn)(args...);
        }

        template <auto Fn>
        static Ret invoke(instance_type, Args... args) {
            return Fn(args...);
        }
    };
}

#endif