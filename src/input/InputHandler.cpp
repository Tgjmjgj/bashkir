#include "input/InputHandler.h"

namespace bashkir
{
InputHandler::InputHandler(std::shared_ptr<NCurses> ncurses, std::shared_ptr<std::vector<std::string>> history)
    : nc(std::move(ncurses)), hist(std::move(history)) {}

std::string InputHandler::waitInput()
{
    std::string bf = nc->readStr();
    return bf;
}

} // namespace bashkir
