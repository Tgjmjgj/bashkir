#pragma once
#include "deps.h"
// #include <string>

namespace bashkir
{

class IAutocomplete
{
public:
    IAutocomplete() {};
    virtual ~IAutocomplete() {};

    virtual std::string complete(const std::string &full_line, size_t pos) = 0;
};

} // namespace bashkir
