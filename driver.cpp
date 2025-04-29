/* Driver for Garnet.

   Copyright (C) 2005-2015, 2018-2021 Free Software Foundation, Inc.
   Copyright (C) 2023 ark231

   This file is part of Bison, the GNU Compiler Compiler.
   This file is part of Garnet.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "driver.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

#include "parser.hpp"
#include "scanner.hpp"

namespace Garnet {
Driver::Driver()
    : lexer_(new yy::Lexer()), result_(new ast::CompilationUnit()), trace_parsing(false), trace_scanning(false) {}

Driver::~Driver() { delete lexer_; }

int Driver::parse(const std::string& f) {
    file_ = f;
    location_.initialize(&file_);
    scan_begin();
    yy::Parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}
void Driver::print_error(const yy::location& loc, const std::string& msg) {
    std::ifstream file(file_);
    std::string line;
    fmt::println(stderr, "{}: {}", fmt::streamed(loc), msg);
    for (auto i = 1; i <= loc.begin.line - 1; i++) {
        std::getline(file, line);
    }
    for (auto i = loc.begin.line; i <= loc.end.line; i++) {
        std::getline(file, line);
        std::string_view pre_colored, colored, post_colored;
        decltype(line.cbegin()) colored_begin = line.cbegin(), colored_end = line.cend();
        if (i == loc.begin.line) {
            pre_colored = {line.cbegin(), line.cbegin() + loc.begin.column - 1};
            colored_begin = line.cbegin() + loc.begin.column - 1;
        }
        if (i == loc.end.line) {
            colored_end = line.cbegin() + loc.end.column - 1;
            if (static_cast<size_t>(loc.end.column) < line.size()) {
                post_colored = {line.cbegin() + loc.end.column - 1, line.cend()};
            }
        }
        colored = {colored_begin, colored_end};
        fmt::println(stderr, "{}{}{}", pre_colored, fmt::styled(colored, fmt::fg(fmt::color::red)), post_colored);
    }
}
}  // namespace Garnet
