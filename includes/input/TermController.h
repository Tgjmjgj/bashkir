#pragma once
#include "deps.h"
#include "input/Position.h"

namespace bashkir
{

class TermController
{
public:
    static void moveCursorUp(size_t n = 1);
    static void moveCursorDown(size_t n = 1);
    static void moveCursorRight(size_t n = 1);
    static void moveCursorLeft(size_t n = 1);
    static void setPos(const Pos &pos);
    static void clearLineLeft();
    static void clearLineRight();
    static void savePos();
    static void restorePos();
};

} // namespace bashkir
