/* Scanner for WomuYuroCompiler

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

%option flex
%option bison-complete
%option bison-locations
%option params="WomuYuro::Driver& drv"

%option lexer=Lexer

%option namespace=WomuYuro.yy
%option bison-cc-namespace=WomuYuro.yy
%option bison-cc-parser=Parser

%option unicode

%top { 
/* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>
# include <fmt/format.h>
# include "driver.hpp"
# include "parser.hpp"
# include "mudig_converter.hpp"
# include "enums.hpp"
%}

%{
#if defined __clang__
# define CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#endif

// Clang and ICC like to pretend they are GCC.
#if defined __GNUC__ && !defined __clang__ && !defined __ICC
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif

// Pacify warnings in yy_init_buffer (observed with Flex 2.6.4)
// and GCC 6.4.0, 7.3.0 with -O3.
#if defined GCC_VERSION && 600 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wnull-dereference"
#endif

// This example uses Flex's C back end, yet compiles it as C++.
// So expect warnings about C style casts and NULL.
#if defined CLANG_VERSION && 500 <= CLANG_VERSION
# pragma clang diagnostic ignored "-Wold-style-cast"
# pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#elif defined GCC_VERSION && 407 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#define FLEX_VERSION (YY_FLEX_MAJOR_VERSION * 100 + YY_FLEX_MINOR_VERSION)

// Old versions of Flex (2.5.35) generate an incomplete documentation comment.
//
//  In file included from src/scan-code-c.c:3:
//  src/scan-code.c:2198:21: error: empty paragraph passed to '@param' command
//        [-Werror,-Wdocumentation]
//   * @param line_number
//     ~~~~~~~~~~~~~~~~~^
//  1 error generated.
#if FLEX_VERSION < 206 && defined CLANG_VERSION
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

// Old versions of Flex (2.5.35) use 'register'.  Warnings introduced in
// GCC 7 and Clang 6.
#if FLEX_VERSION < 206
# if defined CLANG_VERSION && 600 <= CLANG_VERSION
#  pragma clang diagnostic ignored "-Wdeprecated-register"
# elif defined GCC_VERSION && 700 <= GCC_VERSION
#  pragma GCC diagnostic ignored "-Wregister"
# endif
#endif

#if FLEX_VERSION < 206
# if defined CLANG_VERSION
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wdocumentation"
#  pragma clang diagnostic ignored "-Wshorten-64-to-32"
#  pragma clang diagnostic ignored "-Wsign-conversion"
# elif defined GCC_VERSION
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
# endif
#endif

// Flex 2.6.4, GCC 9
// warning: useless cast to type 'int' [-Wuseless-cast]
// 1361 |   YY_CURRENT_BUFFER_LVALUE->yy_buf_size = (int) (new_size - 2);
//      |                                                 ^
#if defined GCC_VERSION && 900 <= GCC_VERSION
# pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
%}

%option noyywrap nounput noinput batch debug

%{
  // A number symbol corresponding to the value in S.
    WomuYuro::yy::Parser::symbol_type
  make_INTEGER (const std::string &s, const WomuYuro::yy::Parser::location_type& loc);
    WomuYuro::yy::Parser::symbol_type
  make_FLOAT (const std::string &s, const WomuYuro::yy::Parser::location_type& loc);
    WomuYuro::yy::Parser::symbol_type
  make_VALREF (const std::string &s, const WomuYuro::yy::Parser::location_type& loc);
%}

non_digit [kgsztdnhmrpbfvcjiauoeyw\u{0103}]|a\u{0306}
id_punc   [_]
digit     [KGSZTDNHMRPB]
id        {non_digit}({non_digit}|{digit}|{id_punc})*
int_pos   {digit}+
int_neg   ({digit}\u{305})+
int       {int_pos}|{int_neg}
frac_pos  ({digit}\u{323})+
frac_neg  ({digit}\u{305}\u{323})+
float     {int_pos}{frac_pos}|{int_neg}{frac_neg}
blank     [ \t\r]
refval    [$&]

%{
  # undef YY_USER_ACTION 
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}
%%
%{
  // A handy shortcut to the location held by the Driver.
  WomuYuro::yy::location& loc = drv.location_;
  // Code run each time yylex is called.
  loc.step ();
%}
{blank}+   loc.step ();
\n+        loc.lines (yyleng); loc.step ();

"se"         return WomuYuro::yy::Parser::make_NOUN_MARKER              (loc);
"ske"        return WomuYuro::yy::Parser::make_NOMINAL_ADJECTIVE_MARKER (loc);
"dizazukere" return WomuYuro::yy::Parser::make_CONST                    (loc);
"-"          return WomuYuro::yy::Parser::make_MINUS                    (loc);
"+"          return WomuYuro::yy::Parser::make_PLUS                     (loc);
\u{00d7}     return WomuYuro::yy::Parser::make_TIMES                    (loc);
"/"          return WomuYuro::yy::Parser::make_SLASH                    (loc);
"("          return WomuYuro::yy::Parser::make_LPAREN                   (loc);
")"          return WomuYuro::yy::Parser::make_RPAREN                   (loc);
\u{2190}     return WomuYuro::yy::Parser::make_ASSIGN                   (loc);
"\""         return WomuYuro::yy::Parser::make_DQUOTE                   (loc);
\u{00ab}     return WomuYuro::yy::Parser::make_LDAQUOTE                 (loc);
\u{00bb}     return WomuYuro::yy::Parser::make_RDAQUOTE                 (loc);
"ni"         return WomuYuro::yy::Parser::make_SUBJECT_POSTPOSITION     (loc);
"."          return WomuYuro::yy::Parser::make_PERIOD                   (loc);

{float}      return make_FLOAT (yytext, loc);
{int}        return make_INTEGER (yytext, loc);
{id}         return WomuYuro::yy::Parser::make_IDENTIFIER (yytext, loc);
{refval}     return make_VALREF (yytext, loc);
.            {
                 throw WomuYuro::yy::Parser::syntax_error
                 (loc, "invalid character: " + std::string(yytext));
             }
<<EOF>>      return WomuYuro::yy::Parser::make_YYEOF (loc);
%%

WomuYuro::yy::Parser::symbol_type
make_INTEGER (const std::string &s, const WomuYuro::yy::Parser::location_type& loc)
{
    int64_t n;
    try{
        n=WomuYuro::mudig_to_int(s);
    }catch(WomuYuro::NumberFormatError& e){
        throw WomuYuro::yy::Parser::syntax_error (loc, fmt::format("failed to parse integer: {}", e.what()));
    }
  return WomuYuro::yy::Parser::make_INTEGER (static_cast<int64_t>(n), loc);
}

WomuYuro::yy::Parser::symbol_type
make_FLOAT (const std::string &s, const WomuYuro::yy::Parser::location_type& loc)
{
    double n;
    try{
        n=WomuYuro::mudig_to_float(s);
    }catch(WomuYuro::NumberFormatError& e){
        throw WomuYuro::yy::Parser::syntax_error (loc, fmt::format("failed to parse fraction: {}", e.what()));
    }
    return WomuYuro::yy::Parser::make_FLOAT (static_cast<double>(n), loc);
}

WomuYuro::yy::Parser::symbol_type
make_VALREF (const std::string &s, const WomuYuro::yy::Parser::location_type& loc)
{
    if(s=="$"){
        return WomuYuro::yy::Parser::make_VALREF(static_cast<WomuYuro::ValRef>(WomuYuro::ValRef::VALUE),loc);
    }else if(s=="&"){
        return WomuYuro::yy::Parser::make_VALREF(static_cast<WomuYuro::ValRef>(WomuYuro::ValRef::REFERENCE),loc);
    }else{
        throw WomuYuro::yy::Parser::syntax_error (loc, fmt::format("invalid valref qualifier: {}", s));
    }
}

void
WomuYuro::Driver::scan_begin ()

{
  lexer_->set_debug(trace_scanning);
  if (file_.empty () || file_ == "-")
    lexer_->in() = stdin;
  else if (!(lexer_->in()= fopen (file_.c_str (), "r")))
    {
      std::cerr << "cannot open " << file_ << ": " << strerror (errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void
WomuYuro::Driver::scan_end ()
{
    fclose(lexer_->in());
}
