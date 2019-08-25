#pragma once
#include <algorithm>
#include <string>
#include <memory>

namespace bashkir::util
{

inline std::string &ltrim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
                  return !std::isspace(ch);
              }));
    return str;
}

inline std::string &rtrim(std::string &str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
                  return !std::isspace(ch);
              })
                  .base(),
              str.end());
    return str;
}

inline void trim(std::string &str)
{
    ltrim(str);
    rtrim(str);
}

inline bool startswith(const std::string &str, const std::string &&prefix)
{
    return str.rfind(prefix) == 0;
}

inline bool startswith(const std::string &str, const std::string &prefix)
{
    return str.rfind(prefix) == 0;
}

namespace _inner
{

class ItemsRange
{
public:
    class ItemsRangeIterator
    {
    private:
        std::string fullString;
        std::size_t start;
        std::size_t length;
        std::size_t next;
        std::string storedCurrentItem;
        bool is_end;
    public:
        using iterator = ItemsRangeIterator;
        using category = std::forward_iterator_tag;
        using value_type = std::string;

        ItemsRangeIterator(std::string &str, std::size_t index);
        bool operator==(const iterator &iter) const;
        bool operator!=(const iterator &iter) const;
        iterator operator++();
        std::string getValue();
        void setValue(const std::string &str);
    private:
        void nextItem();
    };

private:
    std::string copy_str;

public:
    ItemsRange(const std::string &str);
    ItemsRangeIterator begin() noexcept;
    ItemsRangeIterator end() noexcept;
};

} // namespace bashkir::util::_inner

inline _inner::ItemsRange items(const std::string &str)
{
    return _inner::ItemsRange(str);
}

} // namespace bashkir::util
