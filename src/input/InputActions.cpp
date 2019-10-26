#include "input/InputActions.h"
#include "global.h"
#include "util/strutil.h"
#include "input/TermController.h"
#include "input/Autocompletion.h"

namespace bashkir
{

InputActions::InputActions(InputHandler &own)
    : owner(own)
{
    this->autoc = std::make_unique<Autocompletion>();
}

bool InputActions::prevHistoryItem() const
{
    if (this->owner.hist_ind > 0)
    {
        this->owner.hist_ind -= 1;
        this->setHistoryItem(this->owner.hist_ind);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::nextHistoryItem() const
{
    if (this->owner.hist_ind < this->owner.hist->size() - 1)
    {
        this->owner.hist_ind += 1;
        this->setHistoryItem(this->owner.hist_ind);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::moveCursorToBegin() const
{
    if (this->owner.cur.line == 0 && this->owner.cur.pos == 0)
    {
        return false;
    }
    while (this->moveCursorUp());
    while (this->moveCursorLeft());
    return true;
}

bool InputActions::moveCursorLeft() const
{
    if (this->owner.cur.line == 0 && this->owner.cur.pos == 0)
    {
        return false;
    }
    if (this->owner.cur.pos > 0)
    {
        TermController::moveCursorLeft();
        this->owner.setPos(this->owner.cur.pos - 1);
    }
    else
    {
        // need to move to the end of prev line
        this->owner.cur.line--;
        this->owner.setPos(this->owner.input[this->owner.cur.line].real_length);
        TermController::moveCursorUp();
        int delta_pref_len = this->owner.input[this->owner.cur.line].prefix.length() - this->owner.input[this->owner.cur.line + 1].prefix.length();
        size_t shift_num = this->owner.cur.pos + delta_pref_len;
        TermController::moveCursorRight(shift_num);
    }
    return true;
}

bool InputActions::moveCursorRight() const
{
    if (this->owner.cur.line == this->owner.input.size() - 1 && this->owner.cur.pos == this->owner.input.back().real_length)
    {
        return false;
    }
    if (this->owner.cur.pos < this->owner.input[this->owner.cur.line].real_length)
    {
        TermController::moveCursorRight();
        this->owner.setPos(this->owner.cur.pos + 1);
    }
    else
    {
        // move to the begin (after prefix) of the next line
        this->owner.cur.line++;
        this->owner.setPos(0);
        TermController::moveCursorDown();
        int delta_pref_len = this->owner.input[this->owner.cur.line - 1].prefix.length() - this->owner.input[this->owner.cur.line].prefix.length();
        size_t shift_num = this->owner.input[this->owner.cur.line - 1].real_length + delta_pref_len;
        TermController::moveCursorLeft(shift_num);
    }
    return true;
}

bool InputActions::moveCursorUp() const
{
    if (this->owner.cur.line > 0)
    {
        this->moveCursorVertically(this->owner.cur.line, this->owner.cur.line - 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::moveCursorDown() const
{
    if (this->owner.cur.line < this->owner.input.size() - 1)
    {
        this->moveCursorVertically(this->owner.cur.line, this->owner.cur.line + 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::removeFromLeft() const
{
    if (!(this->owner.cur.line == 0 && this->owner.cur.pos == 0))
    {
        if (this->owner.cur.pos > 0)
        {
            Line &line = this->owner.input[this->owner.cur.line];
            std::size_t subs_len = line.real_length - this->owner.cur.pos;
            std::string last_part = util::substr(std::string(line.data), this->owner.cur.pos, subs_len);
            TermController::moveCursorLeft();
            io.write(last_part);
            io.write(' ');
            TermController::moveCursorLeft(subs_len + 1);
            line.real_length -= 1;
            this->owner.setPos(this->owner.cur.pos - 1);
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                line.data[this->owner.cur.pos + i] = line.data[this->owner.cur.pos + i + 1];
            }
            this->owner.untouched = false;
        }
        else
        {
            this->removeInputLine();
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::removeFromRight() const
{
    // Delete key can't remove new line, but backspace can
    Line &line = this->owner.input[this->owner.cur.line];
    if (this->owner.cur.pos < line.real_length)
    {
        std::size_t subs_len = line.real_length - this->owner.cur.pos - 1;
        std::string last_part = util::substr(std::string(line.data), this->owner.cur.pos + 1, subs_len);
        io.write(last_part);
        io.write(' ');
        line.data[line.real_length] = '\0';
        line.real_length -= 1;
        io.write(std::string(subs_len + 1, '\b'));
        for (std::size_t i = 0; i <= subs_len; ++i)
        {
            line.data[this->owner.cur.pos + i] = line.data[this->owner.cur.pos + i + 1];
        }
        this->owner.untouched = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool InputActions::autocomplete() const
{
    std::string add = this->autoc->complete(this->owner.input[this->owner.cur.line].data, this->owner.cur.pos);
    this->owner.writeChars(add);
}

bool InputActions::newLineOrSubmit() const
{
    this->owner.rebuildBlocksData(Pos(0, 0));
    if (this->owner.isCurPosEscaped() || !this->owner.blocks.open.empty())
    {
        this->addNewInputLine();
    }
    else
    {
        io.write("\r\n");
        this->owner.untouched = false;
        this->owner.end = true;
    }
}
void InputActions::setHistoryItem(size_t n) const
{
    Line &line = this->owner.input[this->owner.cur.line];
    std::string hist_item = (*(this->owner.hist))[n];
    std::vector<std::string> hist_lines = util::split(hist_item, '\n');
    // Update presentation on the screen
    this->moveCursorToBegin();
    size_t line_len = hist_lines[0].length();
    io.write(hist_lines[0]);
    size_t end_pos = this->owner.input[0].prefix.length() + hist_lines[0].length();
    if (this->owner.input[0].real_length > line_len)
    {
        io.write(std::string(this->owner.input[0].real_length - line_len, ' '));
    }
    size_t bound = std::max(this->owner.input.size(), hist_lines.size());
    for (size_t i = 1; i < bound; ++i)
    {
        line_len = 0;
        io.write('\n');
        if (i < hist_lines.size())
        {
            io.write(new_line_prefix);
            io.write(hist_lines[i]);
            line_len = hist_lines[i].length();
            end_pos = new_line_prefix.size() + line_len;
        }
        else
        {
            io.write(std::string(this->owner.input[i].prefix.length(), ' '));
        }
        if (i < this->owner.input.size() && this->owner.input[i].real_length > line_len)
        {
            io.write(std::string(this->owner.input[i].real_length - line_len, ' '));
        }
    }
    size_t cur_pos = (bound == 1 ? this->owner.input[0].prefix.length() : new_line_prefix.length());
    if (hist_lines.size() > this->owner.input.size())
    {
        cur_pos += hist_lines.back().length();
    }
    else if (hist_lines.size() < this->owner.input.size())
    {
        cur_pos += this->owner.input.back().real_length;
    }
    else
    {
        cur_pos += std::max(hist_lines.back().length(), this->owner.input.back().real_length);
    }
    TermController::moveCursorUp(bound - hist_lines.size());
    int delta = static_cast<int>(end_pos) - static_cast<int>(cur_pos);
    if (delta >= 0 )
    {
        TermController::moveCursorRight(std::abs(delta));
    }
    else
    {
        TermController::moveCursorLeft(-delta);
    }
    // Update internal input buffer
    this->owner.clearInput();
    strcpy(this->owner.input[0].data, hist_lines[0].c_str());
    this->owner.input[0].real_length = hist_lines[0].length();
    for (size_t i = 1; i < hist_lines.size(); ++i)
    {
        this->owner.input.push_back(Line(hist_lines[i]));
    }
    if (hist_lines.size() > 1)
    {
        this->owner.mode = InputHandler::Mode::MULTILINE;
    }
    else
    {
        this->owner.mode = InputHandler::Mode::SINGLELINE;
    }
    size_t pref_len = (hist_lines.size() == 1 ? this->owner.input[0].prefix.length() : new_line_prefix.length());
    this->owner.cur.line = hist_lines.size() - 1;
    this->owner.setPos(end_pos - pref_len);
    this->owner.untouched = true;
}

bool InputActions::addNewInputLine() const
{
    this->owner.mode = InputHandler::Mode::MULTILINE;
    // Update internal input buffer
    this->owner.input.push_back(Line());
    for (size_t line = this->owner.input.size() - 1; line != this->owner.cur.line + 1; --line)
    {
        this->owner.input[line] = this->owner.input[line - 1];
    }
    if (this->owner.cur.pos != this->owner.input[this->owner.cur.line].real_length)
    {
        if (this->owner.cur.pos == 0)
        {
            this->owner.input[this->owner.cur.line + 1] = this->owner.input[this->owner.cur.line];
            this->owner.input[this->owner.cur.line] = Line();
        }
        else
        {
            auto parts = util::splitInHalf(std::string(this->owner.input[this->owner.cur.line].data), this->owner.cur.pos);
            this->owner.input[this->owner.cur.line] = Line(std::get<0>(parts));
            this->owner.input[this->owner.cur.line + 1] = Line(std::get<1>(parts));
        }
    }
    
    // Update presentation on the screen
    io.write(std::string(this->owner.input[this->owner.cur.line + 1].real_length, ' ') + '\n');
    for (size_t line = this->owner.cur.line + 1; line < this->owner.input.size(); ++line)
    {
        io.write(this->owner.input[line].prefix);
        io.write(this->owner.input[line].data);
        if (line != this->owner.input.size() - 1)
        {
            int delta_length = this->owner.input[line + 1].real_length - this->owner.input[line].real_length;
            if (delta_length > 0)
            {
                io.write(std::string(delta_length, ' '));
            }
        }
        if (line != this->owner.input.size() - 1)
        {
            io.write('\n');
        }
    }
    TermController::moveCursorLeft(this->owner.input[this->owner.input.size() - 1].real_length);
    TermController::moveCursorUp(this->owner.input.size() - this->owner.cur.line - 2);
    this->owner.cur.line++;
    this->owner.setPos(0);
    this->owner.untouched = false;
    return true;
}

bool InputActions::removeInputLine() const
{
    if (this->owner.cur.line == 0 || this->owner.input.size() == 1)
    {
        return false;
    }
    if (this->owner.input.size() == 2)
    {
        this->owner.mode = InputHandler::Mode::SINGLELINE;
    }
    // Update internal input buffer
    Line &prev_l = this->owner.input[this->owner.cur.line - 1];
    Line &cur_l = this->owner.input[this->owner.cur.line];
    size_t orig_prev_len = prev_l.real_length;
    for (size_t i = 0; i < cur_l.real_length; ++i)
    {
        prev_l.data[prev_l.real_length + i] = cur_l.data[i];
    }
    prev_l.real_length += cur_l.real_length;
    std::string erased_prefix = this->owner.input[this->owner.input.size() - 1].prefix;
    for (size_t line = this->owner.cur.line; line < this->owner.input.size() - 1; ++line)
    {
        std::string tmp_pref = this->owner.input[line].prefix;
        this->owner.input[line] = this->owner.input[line + 1];
        this->owner.input[line].prefix = tmp_pref;
    }
    this->owner.input.pop_back();

    // Update presentation on the screen
    TermController::moveCursorUp(1);
    int delta_prefix_length = this->owner.input[this->owner.cur.line - 1].prefix.length() - this->owner.input[this->owner.cur.line].prefix.length();
    int delta_length = delta_prefix_length - this->owner.cur.pos;
    if (delta_length >= 0)
    {
        TermController::moveCursorRight(delta_length);
    }
    else
    {
        TermController::moveCursorLeft(-delta_length);
    }
    io.write(this->owner.input[this->owner.cur.line - 1].data);
    io.write('\n');
    for (size_t line = this->owner.cur.line; line < this->owner.input.size(); ++line)
    {
        io.write(this->owner.input[line].prefix);
        io.write(this->owner.input[line].data);
        delta_length = (line == this->owner.cur.line ? orig_prev_len : this->owner.input[line - 1].real_length) - this->owner.input[line].real_length;
        if (delta_length > 0)
        {
            io.write(std::string(delta_length, ' '));
        }
        io.write('\n');
    }
    io.write(std::string(erased_prefix.length(), ' '));
    io.write(std::string(this->owner.input.back().real_length, ' '));
    delta_length = delta_prefix_length + orig_prev_len - this->owner.input[this->owner.input.size() - 1].real_length;
    if (delta_length >= 0)
    {
        TermController::moveCursorRight(delta_length);
    }
    else
    {
        TermController::moveCursorLeft(-delta_length);
    }
    TermController::moveCursorUp(this->owner.input.size() - this->owner.cur.line + 1);
    this->owner.cur.line--;
    this->owner.setPos(orig_prev_len);
    this->owner.untouched = false;
    return true;
}

void InputActions::moveCursorVertically(size_t from_num, size_t to_num) const
{
    if (from_num > to_num)
    {
        TermController::moveCursorUp();
    }
    else
    {
        TermController::moveCursorDown();
    }
    Line &from = this->owner.input[from_num];
    Line &to = this->owner.input[to_num];
    int prf = static_cast<int>(to.prefix.length()) - static_cast<int>(from.prefix.length());
    int len = static_cast<int>(to.real_length);
    int pos = static_cast<int>(this->owner.cur.pos);
    int memp = static_cast<int>(this->owner.mem);
    int incr = std::min(memp, len) - pos;
    int shift = incr + prf;
    if (shift < 0)
    {
        io.write(std::string(-shift, '\b'));
    }
    else
    {
        TermController::moveCursorRight(shift);
    }
    this->owner.cur.pos += incr;
    this->owner.cur.line += (from_num > to_num ? -1 : 1);
}

} // namespace bashkir
