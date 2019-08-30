#pragma once
#include <memory>
#include <vector>
#include <string>
#include "wrappers/NCurses.h"

namespace bashkir
{
    
class InputHandler
{
private:
    std::shared_ptr<NCurses> nc;
    std::shared_ptr<std::vector<std::string>> hist;
public:
    InputHandler(std::shared_ptr<NCurses> ncurses, std::shared_ptr<std::vector<std::string>> history);
    std::string waitInput();
};

} // namespace bashkir
