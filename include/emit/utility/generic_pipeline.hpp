#pragma once

#ifdef EMIT_SUPPORTED

#include "../resources/pipeline.hpp"

#include "memory.hpp"

#include <new>

namespace emit::detail {
    class generic_pipeline {
        using false_pipeline = pipeline<void*>;

    public:
        generic_pipeline() = default;

        ~generic_pipeline() {
            if (destructor_ != nullptr) {
                destructor_(&memory_);

                destructor_ = nullptr;
                clear_ = nullptr;
                dispatch_ = nullptr;
                move_ = nullptr;
            }
        }

        generic_pipeline(const generic_pipeline&) = delete;

        generic_pipeline(generic_pipeline&& other) noexcept {
            if (other) {
                other.move_(&memory_, &other.memory_);

                destructor_ = other.destructor_;
                clear_ = other.clear_;
                dispatch_ = other.dispatch_;
                move_ = other.move_;

                other.destructor_ = nullptr;
                other.clear_ = nullptr;
                other.dispatch_ = nullptr;
                other.move_ = nullptr;
            }
        }

        generic_pipeline& operator=(const generic_pipeline&) = delete;

        generic_pipeline& operator=(generic_pipeline&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (destructor_ != nullptr) {
                destructor_(&memory_);
            }

            if (other) {
                other.move_(&memory_, &other.memory_);

                destructor_ = other.destructor_;
                clear_ = other.clear_;
                dispatch_ = other.dispatch_;
                move_ = other.move_;

                other.destructor_ = nullptr;
                other.clear_ = nullptr;
                other.dispatch_ = nullptr;
                other.move_ = nullptr;
            }

            return *this;
        }

        template <typename T>
        [[nodiscard]] static generic_pipeline create() {
            generic_pipeline delegate;

            ::new (&delegate.memory_) pipeline<T>();

            delegate.destructor_ = destructor<T>;
            delegate.clear_ = clear<T>;
            delegate.dispatch_ = dispatch<T>;
            delegate.move_ = move<T>;

            return delegate;
        }

        template <typename T>
        [[nodiscard]] pipeline<T>& acquire() {
            return cast<T>(&memory_);
        }

        template <typename T>
        [[nodiscard]] const pipeline<T>& acquire() const {
            return cast<T>(&memory_);
        }

        void clear() {
            clear_(&memory_);
        }

        void dispatch() {
            dispatch_(&memory_);
        }

        [[nodiscard]] explicit operator bool() const {
            return destructor_ != nullptr;
        }

    private:
        memory_of<false_pipeline> memory_;

        void (*destructor_)(void*) = nullptr;
        void (*clear_)(void*) = nullptr;
        void (*dispatch_)(void*) = nullptr;
        void (*move_)(void*, void*) = nullptr;

        template <typename T>
        static pipeline<T>& cast(void* memory) {
            return *std::launder(reinterpret_cast<pipeline<T>*>(memory));
        }

        template <typename T>
        static const pipeline<T>& cast(const void* memory) {
            return *std::launder(reinterpret_cast<const pipeline<T>*>(memory));
        }

        template <typename T>
        static void destructor(void* memory) {
            pipeline<T>& pipeline = cast<T>(memory);

            pipeline.~pipeline<T>();
        }

        template <typename T>
        static void clear(void* memory) {
            pipeline<T>& pipeline = cast<T>(memory);

            pipeline.clear();
        }

        template <typename T>
        static void dispatch(void* memory) {
            pipeline<T>& pipeline = cast<T>(memory);

            pipeline.dispatch();
        }

        template <typename T>
        static void move(void* destination, void* source) {
            new (destination) pipeline<T>(std::move(cast<T>(source)));
        }
    };
}

#endif