// #include <iterator>
// #include <string.h>
// #include <unistd.h>
// #include <stack>
// #include <map>
// #include <experimental/filesystem>
// #include <algorithm>
#include "input/InputHandler.h"
#include "util/strutil.h"
#include "util/pathutil.h"
#include "global.h"

namespace bashkir
{

namespace fs = std::experimental::filesystem;

const std::string SEQ_UP_ARROW = "\033[A";
const std::string SEQ_DOWN_ARROW = "\033[B";
const std::string SEQ_RIGHT_ARROW = "\033[C";
const std::string SEQ_LEFT_ARROW = "\033[D";
const std::string SEQ_SHIFT_UP_ARROW = "\033[1;2A";
const std::string SEQ_SHIFT_DOWN_ARROW = "\033[1;2B";
const std::string SEQ_SHIFT_RIGHT_ARROW = "\033[1;2C";
const std::string SEQ_SHIFT_LEFT_ARROW = "\033[1;2D";
const std::string SEQ_DELETE = "\033[3~";

const char BS_KEY_ENTER = '\r';
const char BS_KEY_BACKSPACE = '\177';
const char BS_KEY_CTRL_C = '\3';

const uint8_t MIN_CSI_SEQ_LEN = 3;

const std::vector<std::string> CSI_seqs = {
    SEQ_UP_ARROW,
    SEQ_DOWN_ARROW,
    SEQ_RIGHT_ARROW,
    SEQ_LEFT_ARROW,
    SEQ_SHIFT_UP_ARROW,
    SEQ_SHIFT_DOWN_ARROW,
    SEQ_SHIFT_RIGHT_ARROW,
    SEQ_SHIFT_LEFT_ARROW,
    SEQ_DELETE
};

InputHandler::InputHandler(std::shared_ptr<std::vector<std::string>> history)
    : hist(std::move(history)) {}

PreParsedInput InputHandler::waitInput()
{
    this->cur = {0, 0};
    this->writePrefix();
    this->hist_ind = this->hist->size();
    this->end = false;
    this->blocks = AllBlocksData();
    this->runInputLoop();
    const PreParsedInput ret = compressInputData(this->blocks.getFullList(), this->input);
    this->input.clear();
    assert(std::accumulate(
            ret.blocks.begin(),
            ret.blocks.end(),
            0,
            [](int sum, const CompressedBlockData &block) { return sum + (block.is_start ? block.uid : -static_cast<int>(block.uid)); }
        ) == 0);
    return ret;
}

void InputHandler::writePrefix()
{
    this->input.push_back(Line());
    std::string cPath = fs::current_path().c_str();
    util::fullToHomeRel(cPath);
    this->input[0].prefix = "paradox> " + cPath + " $ ";
    io.write(this->input[0].prefix);
}

void InputHandler::runInputLoop()
{
    do
    {
        memset(this->tmp_buf, 0, sizeof(this->tmp_buf));
        read(STDIN_FILENO, &this->tmp_buf, sizeof(this->tmp_buf));
        std::size_t rlen = strlen(this->tmp_buf);
        for (std::size_t i = 0; i < rlen;)
        {
            bool found_csi = false;
            if (i + MIN_CSI_SEQ_LEN <= rlen && this->tmp_buf[i] == '\033' && this->tmp_buf[i + 1] == '[')
            {
                auto csi = this->lookForCSISequenceInPos(i);
                if (csi)
                {
                    found_csi = true;
                    if (log::Lev3()) log::to.Info(*csi);
                    this->pressCSIsequence(*csi);
                    i += csi->length();
                }
            }
            if (!found_csi)
            {
                char ch = this->tmp_buf[i];
                if (log::Lev3()) log::to.Info(ch);
                this->pressSimpleKey(ch);
                if (this->end)
                {
                    break;
                }
                ++i;
            }
        }
    } while (!this->end);
}

void InputHandler::rebuildBlocksData(const Pos &from_pos)
{
    this->blocks.eraseAfterPos(from_pos);
    for (size_t pos = from_pos.pos; pos < this->input[from_pos.line].real_length; ++pos)
    {
        detectBlocks(Pos(from_pos.line, pos));
    }
    for (size_t line_n = from_pos.line + 1; line_n < this->input.size(); ++line_n)
    {
        for (size_t pos = 0; pos < this->input[line_n].real_length; ++pos)
        {
            detectBlocks(Pos(line_n, pos));
        }
    }
}

void InputHandler::detectBlocks(const Pos &inpos)
{
    auto fopn = global::blocksMeta.searchStartBeforePos(this->input[inpos.line].data, inpos.pos);
    auto fcls = global::blocksMeta.searchEndBeforePos(this->input[inpos.line].data, inpos.pos);
    if (!fopn && !fcls)
    {
        return;
    }
    if (this->blocks.open.empty() && fopn)
    {
        size_t seq_start_pos = inpos.pos - fopn->start_seq.length();
        if (!this->isPosEscaped(seq_start_pos))
        {
            const Pos start_pos = Pos(inpos.line, seq_start_pos);
            this->blocks.open.push(OpenBlock(*fopn, start_pos, false));
            const OpenBlock &ref = this->blocks.open.top();
            // *This is crutial to pass into the `addOpen` method the same reference when a block opens and closes
            this->blocks.addOpen(start_pos, ref); // *
        }
    }
    else if (!this->blocks.open.empty())// When there are already opened blocks here
    {
        const OpenBlock &last = this->blocks.open.top();
        // Check if the user print closing of last-opened block
        if (fcls && last.block == *fcls && last.escaped == this->isPosEscaped(inpos.pos - fcls->end_seq.length()))
        {
            this->blocks.open.pop();
            this->blocks.addClose(Pos(inpos.line, inpos.pos - fcls->end_seq.length()), last); // *
        }
        else if (fopn) // Check restrictions for opening a nested block
        {
            size_t seq_start_pos = inpos.pos - fopn->start_seq.length();
            bool esc = this->isPosEscaped(seq_start_pos);
            auto r = last.block.rules; // Following the defined rules
            if (r.esc == esc && (r.all || std::find(r.allowed.begin(), r.allowed.end(), *fopn) != r.allowed.end()))
            {
                const Pos start_pos = Pos(inpos.line, seq_start_pos);
                this->blocks.open.push(OpenBlock(*fopn, start_pos, esc));
                const OpenBlock &ref = this->blocks.open.top();
                this->blocks.addOpen(start_pos, ref); // *
            }
        }
    }
}

void InputHandler::setPos(size_t p) noexcept
{
    this->cur.pos = p;
    this->mem = p;
}

bool InputHandler::isPosEscaped(size_t pos) const
{
    bool escaped = false;
    int tmp_pos = static_cast<int>(pos) - 1;
    while (tmp_pos >= 0 && this->input[this->cur.line].data[tmp_pos] == '\\')
    {
        escaped = !escaped;
        --tmp_pos;
    }
    return escaped;
}

bool InputHandler::isPosEscaped(size_t pos, size_t line) const
{
    bool escaped = false;
    size_t tmp_pos = pos - 1;
    while (tmp_pos >= 0 && this->input[line].data[tmp_pos] == '\\')
    {
        escaped = !escaped;
        --tmp_pos;
    }
    return escaped;
}

bool InputHandler::isCurPosEscaped() const
{
    return this->isPosEscaped(this->cur.pos);
}

std::optional<std::string> InputHandler::lookForCSISequenceInPos(size_t pos)
{
    size_t rlen = strlen(this->tmp_buf);
    if (pos >= rlen)
    {
        return std::nullopt;
    }
    for (const std::string &csi : CSI_seqs)
    {
        if (pos + csi.length() <= rlen)
        {
            bool eq = true;
            for (std::size_t j = 2; j < csi.length(); j++)
            {
                if (this->tmp_buf[pos + j] != csi[j])
                {
                    eq = false;
                    break;
                }
            }
            if (eq)
            {
                return csi;
            }
        }
    }
    return std::nullopt;
}

void InputHandler::pressCSIsequence(std::string csi_seq)
{
    if (csi_seq == SEQ_LEFT_ARROW)
    {
        this->moveCursorLeft();
    }
    else if (csi_seq == SEQ_RIGHT_ARROW)
    {
        this->moveCursorRight();
    }
    else if (csi_seq == SEQ_UP_ARROW)
    {
        // if (this->hist_ind > 0)
        // {
        //     this->hist_ind -= 1;
        //     this->setHistoryItem();
        // }
        this->moveCursorUp();
    }
    else if (csi_seq == SEQ_DOWN_ARROW)
    {
        // if (this->hist_ind < this->hist->size() - 1)
        // {
        //     this->hist_ind += 1;
        //     this->setHistoryItem();
        // }
        this->moveCursorDown();
    }
    else if (csi_seq == SEQ_DELETE)
    {
        this->removeFromRight();
    }
    else if (csi_seq == SEQ_SHIFT_UP_ARROW)
    {
        io.write("\033[33");
        io.write("\033[?33");
    }
}

void InputHandler::pressSimpleKey(char ch)
{
    switch (ch)
    {
    case BS_KEY_ENTER:
        this->rebuildBlocksData(Pos(0, 0));
        if (this->isCurPosEscaped() || !this->blocks.open.empty())
        {
            this->addNewInputLine();
        }
        else
        {
            io.write("\r\n");
            this->end = true;
        }
        break;
    case BS_KEY_BACKSPACE:
        this->removeFromLeft();
        break;
    case BS_KEY_CTRL_C:
        this->writeChars("^C");
        break;
    default:
        this->writeChars(std::string(1, ch));
        break;
    }
}

void InputHandler::writeChars(const std::string &chars)
{
    Line &line = this->input[this->cur.line];
    const std::size_t subs_len = line.real_length - this->cur.pos;
    std::string last_part = util::substr(std::string(line.data), this->cur.pos, subs_len);
    io.write(chars);
    io.write(last_part);
    io.write(std::string(subs_len, '\b'));
    for (std::size_t i = 0; i < subs_len; ++i)
    {
        line.data[line.real_length + chars.length() - i - 1] = line.data[line.real_length - i - 1];
    }
    for (std::size_t i = 0; i < chars.length(); ++i)
    {
        line.data[this->cur.pos + i] = chars[i];
    }
    this->setPos(this->cur.pos + chars.length());
    line.real_length += chars.length();
    assert(line.real_length == strlen(line.data));
}

void InputHandler::setHistoryItem()
{
    Line &line = this->input[this->cur.line];
    std::string hist_item = (*(this->hist))[this->hist_ind];
    io.write(std::string(this->cur.pos, '\b'));
    io.write(hist_item);
    std::size_t max_space = line.real_length > hist_item.length() ? line.real_length : hist_item.length();
    if (max_space == line.real_length)
    {
        std::size_t free_space_len = line.real_length - hist_item.length();
        io.write(std::string(free_space_len, ' '));
        io.write(std::string(free_space_len, '\b'));
    }
    for (std::size_t i = 0; i < max_space; ++i)
    {
        if (i < hist_item.length())
        {
            line.data[i] = hist_item[i];
        }
        else
        {
            line.data[i] = '\0';
        }
    }
    this->setPos(hist_item.length());
    this->input[this->cur.line].real_length = hist_item.length();
}

void InputHandler::addNewInputLine()
{
    // Update internal input buffer
    this->input.push_back(Line());
    for (size_t line = this->input.size() - 1; line != this->cur.line + 1; --line)
    {
        this->input[line] = this->input[line - 1];
    }
    if (this->cur.pos != this->input[this->cur.line].real_length)
    {
        if (this->cur.pos == 0)
        {
            this->input[this->cur.line + 1] = this->input[this->cur.line];
            this->input[this->cur.line] = Line();
        }
        else
        {
            auto parts = util::splitInHalf(std::string(this->input[this->cur.line].data), this->cur.pos);
            this->input[this->cur.line] = Line(std::get<0>(parts));
            this->input[this->cur.line + 1] = Line(std::get<1>(parts));
        }
    }
    
    // Update presentation on the screen
    io.write(std::string(this->input[this->cur.line + 1].real_length, ' ') + '\n');
    for (size_t line = this->cur.line + 1; line < this->input.size(); ++line)
    {
        io.write(this->input[line].prefix);
        io.write(this->input[line].data);
        if (line != this->input.size() - 1)
        {
            int delta_length = this->input[line + 1].real_length - this->input[line].real_length;
            if (delta_length > 0)
            {
                io.write(std::string(delta_length, ' '));
            }
        }
        if (line != this->input.size() - 1)
        {
            io.write('\n');
        }
    }
    for (size_t i = 0; i < this->input[this->input.size() - 1].real_length; ++i)
    {
        io.write(SEQ_LEFT_ARROW);
    }
    for (size_t i = 0; i < this->input.size() - this->cur.line - 2; ++i)
    {
        io.write(SEQ_UP_ARROW);
    }
    this->cur.line++;
    this->setPos(0);
}

bool InputHandler::removeInputLine()
{
    if (this->cur.line == 0 || this->input.size() == 1)
    {
        return false;
    }
    // Update internal input buffer
    Line &prev_l = this->input[this->cur.line - 1];
    Line &cur_l = this->input[this->cur.line];
    size_t orig_prev_len = prev_l.real_length;
    for (size_t i = 0; i < cur_l.real_length; ++i)
    {
        prev_l.data[prev_l.real_length + i] = cur_l.data[i];
    }
    prev_l.real_length += cur_l.real_length;
    std::string erased_prefix = this->input[this->input.size() - 1].prefix;
    for (size_t line = this->cur.line; line < this->input.size() - 1; ++line)
    {
        std::string tmp_pref = this->input[line].prefix;
        this->input[line] = this->input[line + 1];
        this->input[line].prefix = tmp_pref;
    }
    this->input.pop_back();

    // Update presentation on the screen
    io.write(SEQ_UP_ARROW);
    int delta_prefix_length = this->input[this->cur.line - 1].prefix.length() - this->input[this->cur.line].prefix.length();
    int delta_length = delta_prefix_length - this->cur.pos;
    for (size_t i = 0; i < delta_length; ++i)
    {  
        if (delta_length > 0)
        {
            io.write(SEQ_RIGHT_ARROW);
        }
        else
        {
            io.write(SEQ_LEFT_ARROW);
        }
    }
    io.write(this->input[this->cur.line - 1].data);
    io.write('\n');
    for (size_t line = this->cur.line; line < this->input.size(); ++line)
    {
        io.write(this->input[line].prefix);
        io.write(this->input[line].data);
        delta_length = (line == this->cur.line ? orig_prev_len : this->input[line - 1].real_length) - this->input[line].real_length;
        if (delta_length > 0)
        {
            io.write(std::string(delta_length, ' '));
        }
        io.write('\n');
    }
    io.write(std::string(erased_prefix.length(), ' '));
    io.write(std::string(this->input.back().real_length, ' '));
    delta_length = delta_prefix_length + orig_prev_len - this->input[this->input.size() - 1].real_length;
    for (size_t i = 0; i < static_cast<size_t>(std::abs(delta_length)); ++i)
    {
        if (delta_length > 0)
        {
            io.write(SEQ_RIGHT_ARROW);
        }
        else
        {
            io.write(SEQ_LEFT_ARROW);
        }
    }
    for (size_t i = 0; i < this->input.size() - this->cur.line + 1; ++i)
    {
        io.write(SEQ_UP_ARROW);
    }
    this->cur.line--;
    this->setPos(orig_prev_len);
    return true;
}

bool InputHandler::moveCursorLeft()
{
    if (!(this->cur.line == 0 && this->cur.pos == 0))
    {
        if (this->cur.pos > 0)
        {
            io.write('\b');
            this->setPos(this->cur.pos - 1);
        }
        else
        {
            // need to move to the end of prev line
            this->cur.line--;
            this->setPos(this->input[this->cur.line].real_length);
            io.write(SEQ_UP_ARROW);
            int delta_pref_len = this->input[this->cur.line].prefix.length() - this->input[this->cur.line + 1].prefix.length();
            size_t shift_num = this->cur.pos + delta_pref_len;
            for (size_t i = 0; i < shift_num; ++i)
            {
                io.write(SEQ_RIGHT_ARROW);
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool InputHandler::moveCursorRight()
{
    if (!(this->cur.line == this->input.size() - 1 && this->cur.pos == this->input.back().real_length))
    {
        if (this->cur.pos < this->input[this->cur.line].real_length)
        {
            io.write(SEQ_RIGHT_ARROW);
            this->setPos(this->cur.pos + 1);
        }
        else
        {
            // move to the begin (after prefix) of the next line
            this->cur.line++;
            this->setPos(0);
            io.write(SEQ_DOWN_ARROW);
            int delta_pref_len = this->input[this->cur.line - 1].prefix.length() - this->input[this->cur.line].prefix.length();
            size_t shift_num = this->input[this->cur.line - 1].real_length + delta_pref_len;
            for (size_t i = 0; i < shift_num; ++i)
            {
                io.write(SEQ_LEFT_ARROW);
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool InputHandler::moveCursorUp()
{
    if (this->cur.line > 0)
    {
        this->moveCursorVertically(this->cur.line, this->cur.line - 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool InputHandler::moveCursorDown()
{
    if (this->cur.line < this->input.size() - 1)
    {
        this->moveCursorVertically(this->cur.line, this->cur.line + 1);
        return true;
    }
    else
    {
        return false;
    }
}

void InputHandler::moveCursorVertically(size_t from_num, size_t to_num)
{
    io.write(from_num > to_num ? SEQ_UP_ARROW : SEQ_DOWN_ARROW);
    Line &from = this->input[from_num];
    Line &to = this->input[to_num];
    int prf = static_cast<int>(to.prefix.length()) - static_cast<int>(from.prefix.length());
    int len = static_cast<int>(to.real_length);
    int pos = static_cast<int>(this->cur.pos);
    int memp = static_cast<int>(this->mem);
    int incr = std::min(memp, len) - pos;
    int shift = incr + prf;
    if (shift < 0)
    {
        io.write(std::string(-shift, '\b'));
    }
    else
    {
        for (int i = 0; i < shift; ++i)
        {
            io.write(SEQ_RIGHT_ARROW);
        }
    }
    this->cur.pos += incr;
    this->cur.line += (from_num > to_num ? -1 : 1);
}

bool InputHandler::removeFromLeft()
{
    if (!(this->cur.line == 0 && this->cur.pos == 0))
    {
        if (this->cur.pos > 0)
        {
            Line &line = this->input[this->cur.line];
            std::size_t subs_len = line.real_length - this->cur.pos;
            std::string last_part = util::substr(std::string(line.data), this->cur.pos, subs_len);
            io.write('\b');
            io.write(last_part);
            io.write(" \b");
            io.write(std::string(subs_len, '\b'));
            line.real_length -= 1;
            this->setPos(this->cur.pos - 1);
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                line.data[this->cur.pos + i] = line.data[this->cur.pos + i + 1];
            }
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

bool InputHandler::removeFromRight()
{
    // Delete key can't remove new line, but backspace can
    Line &line = this->input[this->cur.line];
    if (this->cur.pos < line.real_length)
    {
        std::size_t subs_len = line.real_length - this->cur.pos - 1;
        std::string last_part = util::substr(std::string(line.data), this->cur.pos + 1, subs_len);
        io.write(last_part);
        io.write(' ');
        line.data[line.real_length] = '\0';
        line.real_length -= 1;
        io.write(std::string(subs_len + 1, '\b'));
        for (std::size_t i = 0; i <= subs_len; ++i)
        {
            line.data[this->cur.pos + i] = line.data[this->cur.pos + i + 1];
        }
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace bashkir
