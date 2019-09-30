#pragma once
#include "deps.h"
// #include <exception>

namespace bashkir::exc
{

class ExitException : public std::exception
{
private:
    int exit_code;
    const char *message;
public:
    ExitException(int code, const char* what = "Exit the process");
    int exitCode() const noexcept;
    const char* what() const noexcept;
};

} // namespace bashkir::exc
