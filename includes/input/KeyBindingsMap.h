#pragma once
#include "deps.h"
#include "input/KeyBinding.h"
#include "input/InputActions.h"
// #include <vector>
// #include <set>
// #include <optional>

namespace bashkir
{

class KeyBindingsMap
{
private:
    InputActions actions;
    std::vector<KeyBinding> bindings;

public:
    KeyBindingsMap(InputHandler &owner);
    std::optional<KeyBinding::handler> get(char ch) const;
    std::optional<KeyBinding::handler> get(char ch, InputOption opt) const;
    std::optional<KeyBinding::handler> get(char ch, const std::vector<InputOption> &opts) const;
    std::optional<KeyBinding::handler> get(const std::string &csi_seq) const;
    std::optional<KeyBinding::handler> get(const std::string &csi_seq, InputOption opt) const;
    std::optional<KeyBinding::handler> get(const std::string &csi_seq, const std::vector<InputOption> &opts) const;
private:
    std::optional<KeyBinding::handler> getImpl(const std::string &key_seq, const std::vector<InputOption> &opts) const;
    void bindKeys();
};

} // namespace bashkir
