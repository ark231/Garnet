/* Driver for WomuYuroCompiler.

   Copyright (C) 2005-2015, 2018-2021 Free Software Foundation, Inc.
   Copyright (C) 2023 ark231

   This file is part of Bison, the GNU Compiler Compiler.
   This file is part of WomuYuroCompiler.

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

#include <memory>

#include "parser.hpp"
#include "scanner.hpp"

namespace WomuYuro {
Driver::Driver()
    : lexer_(new yy::Lexer()), result_(new ast::CompilationUnit()), trace_parsing(false), trace_scanning(false) {}

Driver::~Driver() { delete lexer_; }

int Driver::parse(const std::string &f) {
    file_ = f;
    location_.initialize(&file_);
    scan_begin();
    yy::Parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}
}  // namespace WomuYuro
