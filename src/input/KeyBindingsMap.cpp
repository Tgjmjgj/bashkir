#include "input/KeyBindingsMap.h"
#include "input/interface/CsiSequences.h"

namespace bashkir
{

KeyBindingsMap::KeyBindingsMap(InputHandler &owner)
    : actions(InputActions(owner))
{
    this->bindKeys();
}

std::optional<KeyBindingsMap::handler> KeyBindingsMap::operator[](char key) const
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

std::optional<KeyBindingsMap::handler> KeyBindingsMap::operator[](const std::string &key) const
{
    auto it = this->seq_map.find(key);
    if (it == this->seq_map.end())
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
        { csi::BS_KEY_ENTER, std::bind(&InputActions::addNewInputLine, &actions) },
        { csi::BS_KEY_BACKSPACE, std::bind(&InputActions::removeFromLeft, &actions) },
        { csi::BS_KEY_TAB, std::bind(&InputActions::autocomplete, &actions) }
    };

    this->seq_map = {
        { csi::BS_UP_ARROW, std::bind(&InputActions::prevHistoryItem, &actions) },
        { csi::BS_DOWN_ARROW, std::bind(&InputActions::nextHistoryItem, &actions) },
        { csi::BS_RIGHT_ARROW, std::bind(&InputActions::moveCursorRight, &actions) },
        { csi::BS_LEFT_ARROW, std::bind(&InputActions::moveCursorLeft, &actions) },
        { csi::BS_DELETE, std::bind(&InputActions::removeFromRight, &actions) }
    };
}

} // namespace bashkir
