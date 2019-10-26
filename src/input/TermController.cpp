#include "input/TermController.h"
#include "global.h"

namespace bashkir
{

void TermController::moveCursorUp(size_t n)
{
    if (n > 0)
    {
        io.write("\033[" + std::to_string(n) + 'A');
    }
}

void TermController::moveCursorDown(size_t n)
{
    if (n > 0)
    {
        io.write("\033[" + std::to_string(n) + 'B');
    }
}

void TermController::moveCursorRight(size_t n)
{
    if (n > 0)
    {
        io.write("\033[" + std::to_string(n || 1) + 'C');
    }
}

void TermController::moveCursorLeft(size_t n)
{
    if (n > 0)
    {
        io.write("\033[" + std::to_string(n || 1) + 'D');
    }
}

void TermController::setPos(const Pos &pos)
{
    io.write("\033[" + std::to_string(pos.pos)  + ';' + std::to_string(pos.line) + 'H');
}

void TermController::clearLineLeft()
{
    io.write("\033[1J");
}

void TermController::clearLineRight()
{
    io.write("\033[0J");
}

void TermController::savePos()
{
    io.write("\033[s");
}

void TermController::restorePos()
{
    io.write("\033[u");
}

} // namespace bashkir
