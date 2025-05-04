/* Main for Garnet

   Copyright (C) 2005-2015, 2018-2021 Free Software Foundation, Inc.
   Copyright (C) 2023 ark231

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

#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

#include "driver.hpp"
#include "interpreter/exceptions.hpp"
#include "interpreter/interpreter.hpp"
#include "libs/utils/format.hpp"
#include "pretty_printer/pretty_printer.hpp"

namespace bpo = boost::program_options;
using namespace Garnet::operators;

void print_source(const Garnet::location::SourceRegion& loc) {
    std::ifstream file(loc.begin.source_file);
    std::string line;
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
int main(int argc, char* argv[]) {
    bpo::positional_options_description pos;
    pos.add("input-file", -1);
    bpo::options_description opt;
    opt.add_options()("help,h", "show this help")("trace-parsing,p", "enable debug output for parsing")(
        "trace-scanning,s", "enable debug output for scanning")("backtrace,b",
                                                                "show backtrace of interpreter on error")(
        "input-file", bpo::value<std::vector<std::string>>()->required(), "input file (positional)");
    bpo::variables_map varmap;
    bpo::store(bpo::command_line_parser(argc, argv).options(opt).positional(pos).run(), varmap);

    if (varmap.contains("help")) {
        std::cout << opt;
        std::exit(0);
    }
    bpo::notify(varmap);
    int res = 0;
    Garnet::Driver drv;
    if (varmap.contains("trace-parsing")) {
        drv.trace_parsing = true;
    }
    if (varmap.contains("trace-scanning")) {
        drv.trace_scanning = true;
    }
    bool show_backtrace = false;
    if (varmap.contains("backtrace")) {
        show_backtrace = true;
    }
    for (const auto& infilename : varmap["input-file"].as<std::vector<std::string>>()) {
        drv.parse(infilename);
    }
    auto ast = drv.result();
    Garnet::ast::PrettyPrinter printer;
    ast->accept(printer);
    Garnet::interpreter::Interpreter interpreter;
    try {
        ast->accept(interpreter);
    } catch (Garnet::interpreter::InterpreterError& e) {
        fmt::println(std::cerr, "interpreter error: {}", typeid(e));
        auto loc = e.location();
        fmt::println(std::cerr, "    at {}, line {}, col {}", loc.begin.source_file, loc.begin.line, loc.begin.column);
        print_source(loc);
        fmt::println(std::cerr, "    what(): {}", e.what());
        if (show_backtrace) {
            fmt::println(std::cerr, "    backtrace:");
            fmt::println(std::cerr, "{}", fmt::streamed(e.trace()));
        }
    }
    interpreter.debug_print();
    return res;
}
