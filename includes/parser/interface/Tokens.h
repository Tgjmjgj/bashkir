#pragma once
#include <string>
#include <vector>

namespace bashkir
{

    enum SeparatorType
    {
        NORMAL,
        TO_RIGHT,
        TO_FILE,
        TO_FILE_APPEND,
        FILE_FROM_RIGHT
    };

    struct EnvVar
    {
        std::string name;
        std::string value;
    };

    class CommandToken
    {
        std::vector<std::string> words;
        std::vector<EnvVar> env;
    };

    struct SeparatorToken
    {
        SeparatorToken sep;
    };

    inline bool isRedirToFile(const PipeFlow &pipe_mode) {
        return (pipe_mode == PipeFlow::TO_FILE || pipe_mode == PipeFlow::TO_FILE_APPEND);
    }

} // namespace bashkir
