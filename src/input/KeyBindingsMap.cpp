#include "input/KeyBindingsMap.h"
#include "input/interface/CsiSequences.h"
#include "input/interface/InputOption.h"
// #include <string>

namespace bashkir
{

KeyBindingsMap::KeyBindingsMap(InputHandler &owner)
    : actions(InputActions(owner))
{
    this->bindKeys();
}

std::optional<KeyBindingsMap::handler> KeyBindingsMap::get(const InputSpecialKey &key) const
{
    auto it = this->key_map.find(key);
    if (it == this->key_map.end())
    {
        return std::nullopt;
    }
    else
    {
        return (*it).second;
    }
}

void KeyBindingsMap::bindKeys()
{
    this->key_map = {
        { InputSpecialKey(csi::BS_KEY_ENTER), std::bind(&InputActions::addNewInputLine, &actions) },
        { InputSpecialKey(csi::BS_KEY_BACKSPACE), std::bind(&InputActions::removeFromLeft, &actions) },
        { InputSpecialKey(csi::BS_KEY_TAB), std::bind(&InputActions::autocomplete, &actions) },
        { InputSpecialKey(csi::BS_UP_ARROW), std::bind(&InputActions::prevHistoryItem, &actions) },
        { InputSpecialKey(csi::BS_UP_ARROW, InputOption::MULTILINE_MODE), std::bind(&InputActions::moveCursorUp, &actions) },
        { InputSpecialKey(csi::BS_UP_ARROW, { InputOption::MULTILINE_MODE, InputOption::SHIFT }), std::bind(&InputActions::prevHistoryItem, &actions) },
        { InputSpecialKey(csi::BS_DOWN_ARROW), std::bind(&InputActions::nextHistoryItem, &actions) },
        { InputSpecialKey(csi::BS_DOWN_ARROW , InputOption::MULTILINE_MODE), std::bind(&InputActions::moveCursorDown, &actions) },
        { InputSpecialKey(csi::BS_DOWN_ARROW , { InputOption::MULTILINE_MODE, InputOption::SHIFT }), std::bind(&InputActions::nextHistoryItem, &actions) },
        { InputSpecialKey(csi::BS_RIGHT_ARROW), std::bind(&InputActions::moveCursorRight, &actions) },
        { InputSpecialKey(csi::BS_LEFT_ARROW), std::bind(&InputActions::moveCursorLeft, &actions) },
        { InputSpecialKey(csi::BS_DELETE), std::bind(&InputActions::removeFromRight, &actions) }
    };
}

} // namespace bashkir
