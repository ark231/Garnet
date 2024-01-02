/* Driver for WomuYuroCompiler.

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

#ifndef DRIVER_HH
#define DRIVER_HH
#include "parser.hpp"
#include <map>
#include <string>

namespace WomuYuro::yy {
class Lexer;
}

namespace WomuYuro {
// Conducting the whole scanning and parsing of Calc++.
class Driver {
  friend WomuYuro::yy::Parser;
  friend WomuYuro::yy::Lexer;
  yy::Lexer *lexer_;
  int result_;

  std::map<std::string, int> variables_;

  // The name of the file being parsed.
  std::string file_;

  // Handling the scanner.
  void scan_begin();
  void scan_end();

  // The token's location used by the scanner.
  yy::location location_;

public:
  Driver();
  ~Driver();

  int result() { return result_; };

  // Run the parser on file F.  Return 0 on success.
  int parse(const std::string &f);
  // Whether to generate parser debug traces.
  bool trace_parsing;

  // Whether to generate scanner debug traces.
  bool trace_scanning;
};
} // namespace WomuYuro
#endif // ! DRIVER_HH
