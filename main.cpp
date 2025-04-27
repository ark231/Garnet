/* Main for WomuYuroCompiler

   Copyright (C) 2005-2015, 2018-2021 Free Software Foundation, Inc.
   Copyright (C) 2023 ark231

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

#include <fmt/core.h>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <iostream>

#include "driver.hpp"
#include "interpreter/interpreter.hpp"
#include "libs/utils/format.hpp"
#include "pretty_printer/pretty_printer.hpp"

namespace bpo = boost::program_options;
using namespace WomuYuro::operators;

int main(int argc, char* argv[]) {
    bpo::positional_options_description pos;
    pos.add("input-file", -1);
    bpo::options_description opt;
    opt.add_options()("help,h", "show this help")("trace-parsing,p", "enable debug output for parsing")(
        "trace-scanning,s", "enable debug output for scanning")(
        "input-file", bpo::value<std::vector<std::string>>()->required(), "input file (positional)");
    bpo::variables_map varmap;
    bpo::store(bpo::command_line_parser(argc, argv).options(opt).positional(pos).run(), varmap);

    if (varmap.count("help")) {
        std::cout << opt;
        std::exit(0);
    }
    bpo::notify(varmap);
    int res = 0;
    WomuYuro::Driver drv;
    if (varmap.count("trace-parsing")) {
        drv.trace_parsing = true;
    }
    if (varmap.count("trace-scanning")) {
        drv.trace_scanning = true;
    }
    for (const auto& infilename : varmap["input-file"].as<std::vector<std::string>>()) {
        drv.parse(infilename);
    }
    auto ast = drv.result();
    WomuYuro::ast::PrettyPrinter printer;
    ast->accept(printer);
    WomuYuro::interpreter::Interpreter interpreter;
    ast->accept(interpreter);
    interpreter.debug_print();
    return res;
}
