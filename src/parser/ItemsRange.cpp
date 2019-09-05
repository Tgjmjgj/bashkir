#include <string>
#include <memory>
#include "parser/ItemsRange.h"

namespace bashkir::_inner
{
void ItemsRange::ItemsRangeIterator::nextItem()
{
    this->start = this->next;
    this->storedCurrentItem = std::string();
    this->length = 0;
    size_t i = this->start;
    for (; i < this->fullString->length(); ++i)
    {
        if ((*(this->fullString))[i] == ' ' && this->start == i)
        {
            this->start = this->next = i + 1;
        }
        else if ((*(this->fullString))[i] == ' ' && this->length == 0)
        {
            this->length = i - this->start;
        }
        else if (i != this->start && (*(this->fullString))[i - 1] == ' ' && (*(this->fullString))[i] != ' ')
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

ItemsRange::ItemsRangeIterator::ItemsRangeIterator(std::shared_ptr<std::string> &str_ptr, std::size_t index)
    : fullString(str_ptr), next(index), is_end(str_ptr->length() == index)
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
    this->storedCurrentItem = this->fullString->substr(this->start, this->length);
    return this->storedCurrentItem;
}

void ItemsRange::ItemsRangeIterator::setValue(const std::string &str)
{
    if (this->getValue() != str)
    {
        this->fullString->replace(this->start, this->length, str);
        this->next = this->length = this->start;
    }
}

ItemsRange::ItemsRange(const std::string &str)
{
    this->copy_str = std::shared_ptr<std::string>(new std::string(str));
}

ItemsRange::ItemsRangeIterator ItemsRange::begin() noexcept
{
    return ItemsRangeIterator(this->copy_str, 0);
}

ItemsRange::ItemsRangeIterator ItemsRange::end() noexcept
{
    return ItemsRangeIterator(this->copy_str, this->copy_str->length());
}

std::string ItemsRange::getCompletedCommandString() const
{
    return *(this->copy_str);
}

} // namespace bashkir::_inner
