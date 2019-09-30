#include "exceptions/ExitException.h"

namespace bashkir::exc
{

ExitException::ExitException(int code, const char* msg)
    : exit_code(code), message(msg) {}

int ExitException::exitCode() const noexcept
{
    return this->exit_code;
}

const char* ExitException::what() const noexcept
{
    return this->message;
}

} // namespace bashkir::exc
