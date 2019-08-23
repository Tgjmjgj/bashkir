#include <iostream>
#include "builtins/cd/cd.h"
#include "util/pathutil.h"

namespace fs = std::experimental::filesystem;

namespace bashkir::builtins
{

Cd::Cd()
{
    this->currentDir = this->prevDir = fs::current_path().c_str();
}

int Cd::exec(std::vector<std::string> args)
{
    if (args.empty())
        return 0;
    fs::path newPath = this->evaluatePath(args[0]);
    int retCode = this->changePath(newPath);
    if (args.size() > 1 and args[1] != "") 
        this->setCheckpoint(args[1]);
    // int retCode = this->specialCases(args);
    // fs::path newPath = fs::current_path().append(args[0].c_str());
    return retCode;
}

fs::path Cd::evaluatePath(std::string pathArg)
{
    if (pathArg == "-")
    {
        return this->prevDir;
    }
    fs::path newPath;
    if (pathArg[0] == '~')
    {
        newPath = utils::homeRelToFull(pathArg);
    }
    else if (pathArg[0] == '/')
    {
        newPath = pathArg;
    }
    else
    {
        newPath = fs::current_path().append(pathArg.c_str());
    }
    if (pathArg[0] == '@')
    {
        if (fs::is_directory(newPath))
            return newPath;
        std::string cpName = pathArg.substr(1);
        if (this->isCheckpoint(cpName))
        {
            newPath = this->getCheckpoint(cpName);
            return newPath;
        }
        else
        {
            std::cout << "There is no checkpoint with name '" << cpName << "'." << std::endl;
            return this->currentDir;
        }
    }
    return newPath;
}

int Cd::changePath(fs::path toGo)
{
    std::error_code err;
    fs::current_path(toGo, err);
    if (err.value() != 0)
    {
        std::cout << err.message() << std::endl;
        return -1;
    }
    this->prevDir = this->currentDir;
    this->currentDir = toGo;
    return 0;
}

void Cd::setCheckpoint(std::string name)
{
    if (name[0] == '@')
        name = name.substr(1);
    this->checkpoints.insert_or_assign(name, this->currentDir);
}

fs::path Cd::getCheckpoint(std::string name)
{
    return this->checkpoints[name];
}

bool Cd::isCheckpoint(std::string name)
{
    return this->checkpoints.find(name) != this->checkpoints.end();
}

} // namespace builtins
