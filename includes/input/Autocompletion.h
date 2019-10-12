#pragma once
#include "deps.h"
#include "input/interface/IAutocomplete.h"

namespace bashkir
{

class Autocompletion : public IAutocomplete
{

public:
    Autocompletion();
    ~Autocompletion() = default;

    std::string complete(const std::string &full_line, size_t pos);

};

} // namespace bashkir
