#pragma once
#include "deps.h"
// #include <string>
// #include <memory>

namespace bashkir
{
namespace _inner
{

class ItemsRange
{
public:
    class ItemsRangeIterator
    {
    private:
        std::shared_ptr<std::string> fullString;
        std::size_t start;
        std::size_t length;
        std::size_t next;
        std::string storedCurrentItem;
        bool is_end;
    public:
        using iterator = ItemsRangeIterator;
        using category = std::forward_iterator_tag;
        using value_type = std::string;

        ItemsRangeIterator(std::shared_ptr<std::string> &str_ptr, std::size_t index);
        bool operator==(const iterator &iter) const;
        bool operator!=(const iterator &iter) const;
        iterator operator++();
        std::string getValue();
        void setValue(const std::string &str);
    private:
        void nextItem();
    };

private:
    std::shared_ptr<std::string> copy_str;

public:
    ItemsRange(const std::string &str);
    ItemsRangeIterator begin() noexcept;
    ItemsRangeIterator end() noexcept;
    std::string getCompletedCommandString() const;
};

}   // namespace bashkir::_inner

inline _inner::ItemsRange iterate_items(const std::string &str)
{
    return _inner::ItemsRange(str);
}

} // namespace bashkir
