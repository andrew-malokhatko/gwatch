#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace dbg
{
struct Variable
{
    // set by client
    std::string name;
    bool isSigned;

    // set by debugger
    uintptr_t address = 0;
    size_t size = 0;
    uint64_t bytes = 0;

    Variable() = default;
    Variable(const std::string& varName, bool varSigned = false);

    [[nodiscard]] std::string toString() const;

    template <typename T>
    [[nodiscard]] T get() const
    {
        if (sizeof(T) != size)
        {
            throw std::runtime_error("Size mismatch in Variable::get");
        }

        if (isSigned)
        {
            switch (size)
            {
            case 1: return static_cast<T>(static_cast<int8_t>(bytes));
            case 2: return static_cast<T>(static_cast<int16_t>(bytes));
            case 4: return static_cast<T>(static_cast<int32_t>(bytes));
            case 8: return static_cast<T>(static_cast<int64_t>(bytes));
            }
        }
        else
        {
            switch (size)
            {
            case 1: return static_cast<T>(static_cast<uint8_t>(bytes));
            case 2: return static_cast<T>(static_cast<uint16_t>(bytes));
            case 4: return static_cast<T>(static_cast<uint32_t>(bytes));
            case 8: return static_cast<T>(static_cast<uint64_t>(bytes));
            }
        }

        throw std::runtime_error("Unsupported variable size in Variable::get()");
    }
};

} // namespace dbg