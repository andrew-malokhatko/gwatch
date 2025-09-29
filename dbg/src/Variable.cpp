#include "Variable.hpp"

namespace dbg
{
    Variable::Variable(const std::string& varName, bool varSigned)
        : name{varName},
          isSigned{varSigned}
    {
    }

    std::string Variable::toString() const
    {
        // clang-format off
        switch (size)
        {
        case 1:
            return isSigned
            ? std::to_string(static_cast<int8_t>(bytes))
            : std::to_string(static_cast<uint8_t>(bytes));

        case 2:
            return isSigned
            ? std::to_string(static_cast<int16_t>(bytes))
            : std::to_string(static_cast<uint16_t>(bytes));

        case 4:
            return isSigned
            ? std::to_string(static_cast<int32_t>(bytes))
            : std::to_string(static_cast<uint32_t>(bytes));

        case 8:
            return isSigned
            ? std::to_string(static_cast<int64_t>(bytes))
            : std::to_string(static_cast<uint64_t>(bytes));
        }
        // clang-format on

        return "undefined";
    }
}