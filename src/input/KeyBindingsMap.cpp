#include "input/KeyBindingsMap.h"
#include "input/CsiSequences.h"
#include "input/interface/InputOption.h"
// #include <string>

namespace bashkir
{

KeyBindingsMap::KeyBindingsMap(InputHandler &owner)
    : actions(InputActions(owner))
{
    this->bindKeys();
}

void KeyBindingsMap::bindKeys()
{
    this->bindings.push_back(KeyBinding(csi::BS_KEY_ENTER, std::bind(&InputActions::newLineOrSubmit, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_KEY_BACKSPACE, std::bind(&InputActions::removeFromLeft, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_KEY_TAB, std::bind(&InputActions::autocomplete, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_UP_ARROW, std::bind(&InputActions::prevHistoryItem, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_UP_ARROW, InputOption::MULTILINE_MODE_ACTIVE, std::bind(&InputActions::moveCursorUp, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_SHIFT_UP_ARROW, InputOption::MULTILINE_MODE_ACTIVE, std::bind(&InputActions::prevHistoryItem, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_DOWN_ARROW, std::bind(&InputActions::nextHistoryItem, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_DOWN_ARROW , InputOption::MULTILINE_MODE_ACTIVE, std::bind(&InputActions::moveCursorDown, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_SHIFT_DOWN_ARROW , InputOption::MULTILINE_MODE_ACTIVE, std::bind(&InputActions::nextHistoryItem, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_RIGHT_ARROW, std::bind(&InputActions::moveCursorRight, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_LEFT_ARROW, std::bind(&InputActions::moveCursorLeft, &actions)));
    this->bindings.push_back(KeyBinding(csi::BS_DELETE, std::bind(&InputActions::removeFromRight, &actions)));
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(char ch) const
{
    return this->getImpl(std::string(1, ch), std::vector<InputOption>());
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(char ch, InputOption opt) const
{
    return this->getImpl(std::string(1, ch), { opt });
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(char ch, const std::vector<InputOption> &opts) const
{
    return this->getImpl(std::string(1, ch), opts);
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(const std::string &csi_seq) const
{
    return this->getImpl(csi_seq, std::vector<InputOption>());
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(const std::string &csi_seq, InputOption opt) const
{
    return this->getImpl(csi_seq, { opt });
}

std::optional<KeyBinding::handler> KeyBindingsMap::get(const std::string &csi_seq, const std::vector<InputOption> &opts) const
{
    return this->getImpl(csi_seq, opts);
}

std::optional<KeyBinding::handler> KeyBindingsMap::getImpl(const std::string &key_seq, const std::vector<InputOption> &opts) const
{
    for (const KeyBinding &bind : this->bindings)
    {
        if (bind.match(key_seq, opts))
        {
            return bind.action;
        }
    }
    return std::nullopt;
}

} // namespace bashkir
