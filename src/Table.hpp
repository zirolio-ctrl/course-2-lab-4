#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

enum class Alignment { Left, Right, Center };

class Table {
public:
    void add_column(const std::string& name, Alignment align = Alignment::Left) {
        headers_.push_back(name);
        alignments_.push_back(align);
    }

    template<typename... Args>
    void add_row(Args&&... args) {
        std::vector<std::string> row;
        (row.push_back(to_string_format(std::forward<Args>(args))), ...);
        rows_.push_back(row);
    }

    void print(std::ostream& os = std::cout) const {
        if (headers_.empty()) return;

        size_t cols = headers_.size();
        std::vector<size_t> widths(cols, 0);

        for (size_t j = 0; j < cols; ++j) {
            widths[j] = utf8_length(headers_[j]);
        }

        for (const auto& row : rows_) {
            for (size_t j = 0; j < cols && j < row.size(); ++j) {
                widths[j] = std::max(widths[j], utf8_length(row[j]));
            }
        }

        os << "|";
        for (size_t j = 0; j < cols; ++j) {
            os << " " << format_cell(headers_[j], widths[j], Alignment::Left) << " |";
        }
        os << "\n";

        os << "|";
        for (size_t j = 0; j < cols; ++j) {
            std::string sep;
            bool left = (alignments_[j] == Alignment::Left || alignments_[j] == Alignment::Center);
            bool right = (alignments_[j] == Alignment::Right || alignments_[j] == Alignment::Center);
            
            sep += (left ? ":" : "-");
            sep.append(widths[j], '-');
            sep += (right ? ":" : "-");
            os << sep << "|";
        }
        os << "\n";

        for (const auto& row : rows_) {
            os << "|";
            for (size_t j = 0; j < cols; ++j) {
                std::string val = (j < row.size()) ? row[j] : "";
                os << " " << format_cell(val, widths[j], alignments_[j]) << " |";
            }
            os << "\n";
        }
    }

private:
    std::vector<std::string> headers_;
    std::vector<Alignment> alignments_;
    std::vector<std::vector<std::string>> rows_;

    template<typename T>
    std::string to_string_format(const T& val) const {
        std::ostringstream ss;
        ss << val;
        return ss.str();
    }

    static size_t utf8_length(const std::string& str) {
        size_t len = 0;
        for (char c : str) {
            if ((c & 0xC0) != 0x80) {
                len++;
            }
        }
        return len;
    }

    static std::string format_cell(const std::string& text, size_t width, Alignment align) {
        size_t current_len = utf8_length(text);
        if (current_len >= width) return text;

        size_t padding = width - current_len;
        if (align == Alignment::Left) {
            return text + std::string(padding, ' ');
        } else if (align == Alignment::Right) {
            return std::string(padding, ' ') + text;
        } else { // Center
            size_t left_pad = padding / 2;
            size_t right_pad = padding - left_pad;
            return std::string(left_pad, ' ') + text + std::string(right_pad, ' ');
        }
    }
};