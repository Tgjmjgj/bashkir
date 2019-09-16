#include <string.h>
#include <regex>
#include <cctype>
#include "parser/BashkirCmdParser.h"
#include "parser/ItemsRange.h"
#include "util/pathutil.h"
#include "util/convutil.h"

#include <iostream>

namespace bashkir
{

BashkirCmdParser::BashkirCmdParser(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history)
    : io(std::move(nc_io)), hist(std::move(history)) {}

std::vector<Command> BashkirCmdParser::parse(const std::string &input_str)
{
    std::vector<Command> cmds = std::vector<Command>();
    auto items = iterate_items(input_str);
    Command cmd;
    bool before_first_cmd = true;
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        std::string item = it.getValue();
        if (this->substitution(item))
        {
            it.setValue(item);
            // When value on iter changed, next incremented iter value will refer to new value
            continue;
        }
        // Env var setting can be only in begin of input
        if (before_first_cmd)
        {
            std::tuple<std::string, std::string> parts = util::splitInHalf(item, '=');
            if (std::get<1>(parts) != "")
            {
                EnvVar env;
                env.name = std::get<0>(parts).c_str();
                env.value = std::get<1>(parts).c_str();
                cmd.env.push_back(env);
                continue;
            }
        }
        if (item == "|")
        {
            cmd.io = PipeFlow::TO_RIGHT;
        }
        else if (item == ">")
        {
            cmd.io = PipeFlow::TO_FILE;
        }
        else if (item == ">>")
        {
            cmd.io = PipeFlow::TO_FILE_APPEND;
        }
        if (item == "&&" || item == "|" || item == ">" || item == ">>")
        {
            cmds.push_back(cmd);
            before_first_cmd = true;
            cmd = Command();
        }
        else if (cmd.args.empty() && cmd.exe.length() == 0)
        {
            before_first_cmd = false;
            cmd.exe = item;
        }
        else
        {
            cmd.args.push_back(item);
        }
    }
    if (cmd.exe.length() != 0)
    {
        cmds.push_back(cmd);
    }
    this->hist->push_back(items.getCompletedCommandString());
    this->postprocess(cmds);
    return cmds;
}

void BashkirCmdParser::postprocess(std::vector<Command> &cmds) const
{
    for (Command &cmd : cmds)
    {
        for (std::string &arg : cmd.args)
        {
            util::tryHomeRelToFull(arg);
        }
    }
}

bool BashkirCmdParser::substitution(std::string &argument) const
{
    bool is_changed1 = this->substituteHist(argument);
    bool is_changed2 = this->substituteEnv(argument);
    return is_changed1 || is_changed2;
}

bool BashkirCmdParser::substituteHist(std::string &argument) const
{
    std::size_t pos = argument.find('!');
    bool is_changed = false;
    while (pos != std::string::npos && pos != argument.length() - 1)
    {
        switch (argument[pos + 1])
        {
        case '!':
        {
            const std::string last_cmd = this->hist->back();
            argument.replace(pos, 2, last_cmd);
            is_changed = true;
            break;
        }
        default:
            break;
        }
        bool reverse = false;
        if (argument[pos + 1] == '-')
        {
            reverse = true;
        }
        const std::size_t rv_shift = reverse ? 2 : 1;
        if (std::isdigit(argument[pos + rv_shift]) != 0)
        {
            std::size_t end = pos + rv_shift;
            while (std::isdigit(argument[end]) != 0)
            {
                end++;
            }
            const std::string str_num = argument.substr(pos + rv_shift, end - pos - rv_shift);
            const std::size_t num = util::int2size_t(std::stoi(str_num));
            const std::size_t hist_index = reverse ? this->hist->size() - num : num - 1;
            if (hist_index < this->hist->size())
            {
                const std::string hist_item = (*(this->hist))[hist_index];
                argument.replace(pos, 1 + str_num.length(), hist_item);
                is_changed = true;
            }
            else
            {
                this->io->write("bashkir: command ");
                this->io->write(reverse ? "-" : "");
                this->io->writeStr(num + " is not found in history.");
            }
        }
        pos = argument.find('!', pos + 1);
    }
    return is_changed;
}

bool BashkirCmdParser::substituteEnv(std::string &argument) const
{
    bool is_changed = false;
    const std::regex re("\\$[A-Za-z_][A-Za-z0-9_]*");
    std::sregex_iterator sr_begin(argument.begin(), argument.end(), re), sr_end;
    std::vector<std::tuple<std::size_t, std::size_t>> poses;
    for (std::sregex_iterator i = sr_begin; i != sr_end; ++i)
    {
        const std::size_t start = util::li2size_t(i->position()) / sizeof(char);
        const std::size_t len = util::li2size_t(i->length()) / sizeof(char);
        poses.insert(poses.begin(), std::make_tuple(start, len));
    }
    for (auto &pos : poses)
    {
        const std::size_t start = std::get<0>(pos);
        const std::size_t len = std::get<1>(pos);
        const std::string env_name = argument.substr(start + 1, len - 1);
        char *env_val = getenv(env_name.c_str());
        const std::string env_str = env_val == nullptr ? std::string() : std::string(env_val);
        argument.replace(start, len, env_str);
    }
    return is_changed;
}

} // namespace bashkir
