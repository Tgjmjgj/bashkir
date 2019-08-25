#include <string>
#include <memory>
#include "util/strutil.h"

namespace bashkir::util::_inner
{
void ItemsRange::ItemsRangeIterator::nextItem()
{
    this->start = this->next;
    this->storedCurrentItem = std::string();
    this->length = 0;
    size_t i = this->start;
    for (; i < this->fullString.length(); ++i)
    {
        if (this->fullString[i] == ' ')
        {
            this->length = i - this->start;
        }
        else if (i != this->start && this->fullString[i - 1] == ' ' && this->fullString[i] != ' ')
        {
            break;
        }
    }
    this->next = i;
    if (this->start == this->next)
    {
        this->is_end = true;
    }
    else if (this->length == 0)
    {
        this->length = i - this->start;
    }
}

ItemsRange::ItemsRangeIterator::ItemsRangeIterator(std::string &str, std::size_t index)
    : fullString(str), next(index), is_end(str.length() == index)
{
    this->nextItem();
}

bool ItemsRange::ItemsRangeIterator::operator==(const ItemsRange::ItemsRangeIterator::iterator &iter) const
{
    if (this->fullString != iter.fullString)
    {
        return false;
    }
    if (this->is_end == true && iter.is_end == true)
    {
        return true;
    }
    return this->start == iter.start && this->length == iter.length && this->next == iter.next;
}

bool ItemsRange::ItemsRangeIterator::operator!=(const ItemsRange::ItemsRangeIterator::iterator &iter) const
{
    return !this->operator==(iter);
}

ItemsRange::ItemsRangeIterator::iterator ItemsRange::ItemsRangeIterator::operator++()
{
    this->nextItem();
    return *this;
}

std::string ItemsRange::ItemsRangeIterator::getValue()
{
    if (!this->storedCurrentItem.empty())
    {
        return this->storedCurrentItem;
    }
    this->storedCurrentItem = this->fullString.substr(this->start, this->length);
    return this->storedCurrentItem;
}

void ItemsRange::ItemsRangeIterator::setValue(const std::string &str)
{
    this->fullString.replace(this->start, this->length, str);
}

ItemsRange::ItemsRange(const std::string &str) : copy_str(std::string(str)) {}

ItemsRange::ItemsRangeIterator ItemsRange::begin() noexcept
{
    return ItemsRangeIterator(this->copy_str, 0);
}

ItemsRange::ItemsRangeIterator ItemsRange::end() noexcept
{
    return ItemsRangeIterator(this->copy_str, this->copy_str.length());
}

} // namespace bashkir::util::_inner
