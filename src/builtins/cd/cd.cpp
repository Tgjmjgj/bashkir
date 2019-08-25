#include <iostream>
#include "builtins/cd/cd.h"
#include "util/pathutil.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Cd::Cd()
{
    this->current_dir = this->prev_dir = fs::current_path().c_str();
}

int Cd::exec(const std::vector<std::string> &args)
{
    if (args.empty())
    {
        return 0;
    }
    const fs::path new_path = this->evaluatePath(args[0]);
    const int ret_code = this->changePath(new_path);
    if (args.size() > 1 and args[1] != "")
    {
        this->setCheckpoint(args[1]);
    }
    return ret_code;
}

fs::path Cd::evaluatePath(const std::string &path_arg) const
{
    if (path_arg == "-")
    {
        return this->prev_dir;
    }
    fs::path new_path;
    if (path_arg[0] == '/')
    {
        new_path = path_arg;
    }
    else
    {
        new_path = fs::current_path().append(path_arg.c_str());
    }
    if (path_arg[0] == '@')
    {
        if (fs::is_directory(new_path))
            return new_path;
        std::string cp_name = path_arg.substr(1);
        if (this->isCheckpoint(cp_name))
        {
            new_path = this->getCheckpoint(cp_name);
            return new_path;
        }
        else
        {
            std::cout << "There is no checkpoint with name '" << cp_name << "'." << std::endl;
            return this->current_dir;
        }
    }
    return new_path;
}

int Cd::changePath(const fs::path &togo)
{
    std::error_code err;
    fs::current_path(togo, err);
    if (err.value() != 0)
    {
        std::cout << err.message() << std::endl;
        return -1;
    }
    this->prev_dir = this->current_dir;
    this->current_dir = togo;
    return 0;
}

void Cd::setCheckpoint(const std::string &name)
{
    std::string cp_name = name.substr(name[0] == '@' ? 1 : 0);
    this->checkpoints.insert_or_assign(cp_name, this->current_dir);
}

fs::path Cd::getCheckpoint(const std::string &name) const noexcept(false)
{
    return this->checkpoints.at(name);
}

bool Cd::isCheckpoint(const std::string &name) const
{
    return this->checkpoints.find(name) != this->checkpoints.end();
}

} // namespace bashkir::builtins
