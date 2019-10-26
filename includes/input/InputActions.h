#pragma once
#include "interface/IAutocomplete.h"

namespace bashkir
{

class InputHandler;

class InputActions
{
private:
    InputHandler &owner;
    std::unique_ptr<IAutocomplete> autoc;
public:
    InputActions(InputHandler &own);

    bool prevHistoryItem() const;
    bool nextHistoryItem() const;
    bool moveCursorToBegin() const;
    bool moveCursorLeft() const;
    bool moveCursorRight() const;
    bool moveCursorUp() const;
    bool moveCursorDown() const;
    bool removeFromLeft() const;
    bool removeFromRight() const;
    bool autocomplete() const;
    bool newLineOrSubmit() const;
private:
    void setHistoryItem(size_t n) const;
    bool addNewInputLine() const;
    bool removeInputLine() const;
    void moveCursorVertically(size_t from, size_t to) const;
};

} // namespace bashkir
