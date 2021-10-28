#pragma once

#define TEST_COUNTER(Name)                                                                                   \
    namespace {                                                                                              \
        class Name {                                                                                         \
        public:                                                                                              \
            static inline int aliveCount = 0;                                                                \
            static inline int copyCtors = 0;                                                                 \
            static inline int moveCtors = 0;                                                                 \
            static inline int copyAssigns = 0;                                                               \
            static inline int moveAssigns = 0;                                                               \
                                                                                                             \
            int data;                                                                                        \
                                                                                                             \
            Name() noexcept : data{0}                                                                        \
            {                                                                                                \
                aliveCount++;                                                                                \
            }                                                                                                \
                                                                                                             \
            Name(int inData) noexcept : Name()                                                               \
            {                                                                                                \
                data = inData;                                                                               \
            }                                                                                                \
                                                                                                             \
            Name(const Name& other) noexcept : Name()                                                        \
            {                                                                                                \
                data = other.data;                                                                           \
                copyCtors++;                                                                                 \
            }                                                                                                \
                                                                                                             \
            Name(Name&& other) noexcept : Name()                                                             \
            {                                                                                                \
                data = other.data;                                                                           \
                moveCtors++;                                                                                 \
            }                                                                                                \
                                                                                                             \
            Name&                                                                                            \
            operator=(const Name& other) noexcept                                                            \
            {                                                                                                \
                data = other.data;                                                                           \
                copyAssigns++;                                                                               \
            }                                                                                                \
                                                                                                             \
            Name&                                                                                            \
            operator=(Name&& other) noexcept                                                                 \
            {                                                                                                \
                data = other.data;                                                                           \
                moveAssigns++;                                                                               \
            }                                                                                                \
                                                                                                             \
            ~Name() noexcept                                                                                 \
            {                                                                                                \
                aliveCount--;                                                                                \
            }                                                                                                \
        };                                                                                                   \
    }\
