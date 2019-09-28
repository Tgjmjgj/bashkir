#include "io/ScopedOutputRedirect.h"

namespace bashkir
{

ScopedOutputRedirect::ScopedOutputRedirect()
{
    this->old = std::cout.rdbuf(this->ss.rdbuf());
}

ScopedOutputRedirect::~ScopedOutputRedirect()
{
    std::cout.rdbuf(this->old);
}

std::string ScopedOutputRedirect::CollectOutput()
{
    std::string ret = this->ss.str();
    this->ss.str(std::string());
    return ret;
}

} // namespace bashkir
