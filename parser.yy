/* Parser for Garnet.

     Copyright (C) 2005-2015, 2018-2021 Free Software Foundation, Inc.
     Copyright (C) 2023 ark231

     This file is part of Garnet.

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.    If not, see <https://www.gnu.org/licenses/>.    */

%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.2"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%define api.namespace {Garnet::yy}
%define api.parser.class {Parser}

%code {
}

%code requires {
    # include <fmt/format.h>
    # include <fmt/ostream.h>
    # include <fmt/ranges.h>
    # include <fmt/std.h>
    # include <magic_enum/magic_enum_format.hpp>
    # include "enums.hpp"
    # include "compilation_unit.hpp"
    # include "concrete_decls.hpp"
    # include "concrete_expressions.hpp"
    # include "concrete_statements.hpp"
    # include "concrete_defs.hpp"
    # include "concrete_infos.hpp"
    # include <string>
    # include "format.hpp"
    # include <algorithm>
    # include "error_nodes.hpp"
    # include <optional>
    namespace Garnet{
        class Driver;
    }
    namespace GN=Garnet;
}


%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code requires{
    namespace Garnet::yy {
        class Lexer;    // Generated by reflex with namespace=yy lexer=Lexer lex=yylex
    }
}

%parse-param { GN::Driver& drv }    // Construct parser object with lexer

%lex-param { GN::Driver& drv }    // Construct lexr object with lexer

%code {
# include "driver.hpp"
# include "scanner.hpp"
# undef yylex
# define yylex drv.lexer_->yylex

namespace Garnet::yy{
Garnet::location::SourceRegion conv_loc(const location& l){
    return Garnet::location::SourceRegion{
        .begin = {*l.begin.filename, l.begin.line, l.begin.column},
        .end   = {*l.end.filename,   l.end.line,   l.end.column}
        };
}
}
}

%define api.token.prefix {TOK_}
%token
    ASSIGN                   "="
    MINUS                    "-"
    PLUS                     "+"
    DECREMENT                "--"
    INCREMENT                "++"
    LESSTHAN                 "<"
    GREATERTHAN              ">"
    LESSTHAN_EQUAL           "<="
    GREATERTHAN_EQUAL        ">="
    EQUAL                    "=="
    NOT_EQUAL                "!="
    ASTERISK                 "*"
    PERCENT                  "%"
    EXPONENTIATION           "^"
    PLUS_ASSIGN              "+="
    MINUS_ASSIGN             "-="
    MUL_ASSIGN               "*="
    DIV_ASSIGN               "/="
    MOD_ASSIGN               "%="
    IDIV_ASSIGN              "dslash_eq"
    SLASH                    "/"
    DOUBLE_SLASH             "dslash"
    BOOL_AND                 "and"
    BOOL_AND_SIM             "&&"
    BIT_AND_SIM              "&"
    BIT_AND                  "bit_and"
    BOOL_OR                  "or"
    BOOL_OR_SIM              "||"
    BIT_OR_SIM               "|"
    BIT_OR                   "bit_or"
    BOOL_NOT                 "not"
    BOOL_NOT_SIM             "!"
    BIT_NOT_SIM              "~"
    BIT_NOT                  "bit_not"
    // BOOL_XOR                 "xor"
    // BOOL_XOR_SIM             ""
    // BIT_XOR_SIM              ""
    BIT_XOR                  "xor"
    LEFT_SHIFT               "<<"
    RIGHT_SHIFT              ">>"
    LPAREN                   "("
    RPAREN                   ")"
    PERIOD                   "."
    LBRACKET                 "["
    RBRACKET                 "]"
    RARROW                   "->"
    COMMA                    ","
    LBRACE                   "{"
    RBRACE                   "}"
    SEMICOLON                ";"
    COLON                    ":"
    VAR                      "var"
    LET                      "let"
    FUNC                     "func"
    REF                      "ref"
    RETURN                   "return"
    IF                       "if"
    ELIF                     "elif"
    ELSE                     "else"
    WHILE                    "while"
    LOOP                     "loop"
    FOR                      "for"
    BREAK                    "break"
    CONTINUE                 "continue"
    ASSERT                   "assert"
    TRUE                     "true"
    FALSE                    "false"
    NIL                      "nil"
    DO                       "do"
;

%token <std::string>         IDENTIFIER  "identifier"
%token <int64_t>             INTEGER     "integer"
%token <double>              FLOAT       "floating point"
%token <GN::ValRef>          VALREF      "valref"
%token <std::string>         STRING      "string"

%nterm <GN::ConstMut> const
%nterm <std::shared_ptr<GN::ast::VariableReference>> variable_reference
%nterm <std::shared_ptr<GN::ast::CompilationUnit>> unit
%nterm <std::shared_ptr<GN::ast::Sentence>> sentence
%nterm <std::shared_ptr<GN::ast::DeclBase>> decl
%nterm <std::shared_ptr<GN::ast::Expression>> exp
%nterm <std::vector<std::shared_ptr<GN::ast::Expression>>> exp_list
%nterm <std::vector<std::shared_ptr<GN::ast::Expression>>> parameter_list
%nterm <std::shared_ptr<GN::ast::Statement>> stmt
%nterm <GN::ast::FunctionInfo> function_decl
%nterm <std::shared_ptr<GN::ast::VariableDecl>> variable_decl
%nterm <std::shared_ptr<GN::ast::VariableDecl>> variable_init
%nterm <std::shared_ptr<GN::ast::Block>> block
%nterm <std::vector<std::shared_ptr<GN::ast::Sentence>>> sentences
%nterm <std::shared_ptr<GN::ast::BinaryOperator>> binary_operator
%nterm <std::shared_ptr<GN::ast::UnaryOperator>> unary_operator
%nterm <std::shared_ptr<GN::ast::FloatingPointLiteral>> floating_point_literal
%nterm <std::shared_ptr<GN::ast::SignedIntegerLiteral>> signed_integer_literal
%nterm <std::shared_ptr<GN::ast::StringLiteral>> string_literal
%nterm <std::shared_ptr<GN::ast::BooleanLiteral>> boolean_literal
%nterm <std::shared_ptr<GN::ast::NilLiteral>> nil_literal
%nterm <std::shared_ptr<GN::ast::VariableDeclStatement>> variable_decl_statement
%nterm <std::tuple<GN::ValRef,GN::ast::SourceTypeIdentifier>> type_info
%nterm <GN::ast::VariableInfo> var_decl 
%nterm <std::vector<GN::ast::VariableInfo>> var_decl_list
%nterm <std::vector<GN::ast::VariableInfo>> parameter_decl_list
%nterm <std::shared_ptr<GN::ast::Base>> decl_or_def
%nterm <std::shared_ptr<GN::ast::FunctionDef>> function_def
%nterm <std::shared_ptr<GN::ast::FunctionCall>> function_call
%nterm <std::shared_ptr<GN::ast::ReturnStatement>> return_statement
%nterm <std::shared_ptr<GN::ast::LoopStatement>> loop_statement
%nterm <std::shared_ptr<GN::ast::IfStatement>> if_statement
%nterm <std::shared_ptr<GN::ast::IfStatement>> alone_if_statement
%nterm <std::vector<GN::ast::IfStatement::CondBlock>> elifs
%nterm <GN::ast::IfStatement::CondBlock> elif
%nterm <GN::ast::IfStatement::CondBlock> else
%nterm <std::shared_ptr<GN::ast::BreakStatement>> break_statement
%nterm <std::shared_ptr<GN::ast::AssertStatement>> assert_statement
%nterm <GN::ValRef> omittable_ref
%nterm <std::shared_ptr<GN::ast::Expression>> callable_exp
%nterm <std::shared_ptr<GN::ast::Expression>> uncallable_exp
%nterm <std::shared_ptr<GN::ast::Statement>> for_statement
%nterm <std::shared_ptr<GN::ast::Statement>> while_statement
%nterm <std::shared_ptr<GN::ast::Statement>> do_while_statement
%nterm <std::optional<std::shared_ptr<GN::ast::VariableDecl>>> omittable_variable_init
%nterm <std::optional<std::shared_ptr<GN::ast::Expression>>> omittable_exp



%printer { fmt::print(yyo,"{}",fmt::ptr($$)); } variable_reference unit sentence decl exp stmt variable_decl variable_init binary_operator unary_operator floating_point_literal signed_integer_literal variable_decl_statement decl_or_def function_def function_call return_statement block loop_statement if_statement alone_if_statement break_statement assert_statement callable_exp uncallable_exp string_literal boolean_literal nil_literal for_statement while_statement do_while_statement
%printer { 
    std::vector<const void*> ptrs;
    std::ranges::transform($$,std::back_inserter(ptrs),[](auto p){return fmt::ptr(p);});
    fmt::print(yyo,"{}",ptrs); 
} sentences exp_list parameter_list
%printer { 
    for(auto [cond, block]:$$){
        fmt::print(yyo,"{{cond: {},block: {}}}",fmt::ptr(cond),fmt::ptr(block));
    }
} elifs
%printer { 
    auto [cond, block] = $$;
    fmt::print(yyo,"{{cond: {},block: {}}}",fmt::ptr(cond),fmt::ptr(block));
} elif else
%printer { 
    if($$.has_value()){
        fmt::print(yyo,"{}",fmt::ptr($$.value())); 
    }else{
        fmt::print(yyo,"null");
    }
} omittable_variable_init omittable_exp
%printer { fmt::print(yyo,"{}",$$); } <*>

%%
%start unit;

unit:
  decl_or_def                     { drv.result_->add_child($1); }
| unit decl_or_def                { drv.result_->add_child($2); };

decl_or_def:
  decl ";"                 { $$ = std::dynamic_pointer_cast<GN::ast::Base>($1); }
| function_def             { $$ = std::dynamic_pointer_cast<GN::ast::Base>($1);}
| error ";"                { 
      yyclearin; 
      $$ = std::dynamic_pointer_cast<GN::ast::Base>(std::make_shared<GN::ast::ErrorNode>(conv_loc(@$)));
    }

decl:
    variable_decl            { $$ = std::dynamic_pointer_cast<GN::ast::DeclBase>($1); };

type_info:
    omittable_ref "identifier"  { $$ = {$1, GN::ast::SourceTypeIdentifier{$2}}; }

var_decl:
  const "identifier" ":" type_info{ 
        auto [valref,type] = $4;
        $$ = GN::ast::VariableInfo({$2},{type},valref,$1 == GN::ConstMut::CONST); 
        $$.set_location(conv_loc(@$));
      };

var_decl_list:
  var_decl                         { $$ = {$1}; }
| var_decl_list "," var_decl       { 
        $$ = std::move($1);
        $$.push_back($3); 
      };

parameter_decl_list:
  %empty                           { $$ = {}; }
| var_decl_list                    { $$ = std::move($1); }

function_decl:
  "func" "identifier" "(" parameter_decl_list ")" "->" type_info {
        auto [valref,type] = $7;
        GN::ast::VariableInfo return_info{{GN::RETURN_SPECIAL_VARNAME},type,valref};
        return_info.set_location(conv_loc(@7));
        $$ = GN::ast::FunctionInfo(
            GN::ast::SourceFunctionIdentifier{$2},std::move($4),return_info
           );
    };

function_def:
  function_decl block             { 
      $$ = std::make_shared<GN::ast::FunctionDef>($1,$2,conv_loc(@$)); 
    }

block:
  "{" sentences "}"        {
    $$ = std::make_shared<GN::ast::Block>(std::move($2),conv_loc(@$));
  }

sentences:
  %empty                   {
      $$ = {};
    }
| sentences sentence   { 
      $$ = std::move($1);
      $$.push_back($2);
    }

sentence:
  stmt                     { $$ = std::dynamic_pointer_cast<GN::ast::Sentence>($1); }
| exp ";"                  { $$ = std::dynamic_pointer_cast<GN::ast::Sentence>($1); }
| error ";"                { 
      yyclearin; 
      $$ = std::dynamic_pointer_cast<GN::ast::Sentence>(std::make_shared<GN::ast::ErrorSentence>(conv_loc(@$)));
    }

const:
  "var"           { $$ = GN::ConstMut::MUT; }
| "let"           { $$ = GN::ConstMut::CONST; };


variable_decl:
  var_decl {
      $$ = std::make_shared<GN::ast::VariableDecl>($1.name(), $1.type().name(),std::nullopt,conv_loc(@$));
    };

variable_init:
  var_decl "=" exp {
      $$ = std::make_shared<GN::ast::VariableDecl>($1.name(), $1.type().name(),$3,conv_loc(@$));
  }

omittable_ref:
  %empty             { $$ = GN::ValRef::VALUE; }
| "ref"              { $$ = GN::ValRef::REFERENCE; }


variable_reference:
  omittable_ref "identifier" { $$ = std::make_shared<GN::ast::VariableReference>(GN::ast::SourceVariableIdentifier($2),$1,conv_loc(@$)); };

%left "and" "&&" "or" "||";
%left "<" ">" "<=" ">=" "==" "!=";
%left "=" "+=" "-=" "*=" "/=" "%=";
%left "+" "-";
%left "*" "/" "%";
%left "<<" ">>" "bit_and" "&" "bit_or" "|" "xor";
%left UPLUS UMINUS "not" "!" "bit_not" "~";

binary_operator:
  exp "+" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::ADD,$1,$3,conv_loc(@$)); }
| exp "-" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::SUB,$1,$3,conv_loc(@$)); }
| exp "*" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::MUL,$1,$3,conv_loc(@$)); }
| exp "/" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::DIV,$1,$3,conv_loc(@$)); }
| exp "%" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::MOD,$1,$3,conv_loc(@$)); }
| exp "=" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::ASSIGN,$1,$3,conv_loc(@$)); }
| exp "<" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::LESS,$1,$3,conv_loc(@$)); }
| exp ">" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::GREATER,$1,$3,conv_loc(@$)); }
| exp "<=" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::LESS_EQUAL,$1,$3,conv_loc(@$)); }
| exp ">=" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::GREATER_EQUAL,$1,$3,conv_loc(@$)); }
| exp "==" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::EQUAL,$1,$3,conv_loc(@$)); }
| exp "!=" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::NOT_EQUAL,$1,$3,conv_loc(@$)); }
| exp "&&" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BOOL_AND,$1,$3,conv_loc(@$)); }
| exp "and" exp      { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BOOL_AND,$1,$3,conv_loc(@$)); }
| exp "&" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BIT_AND,$1,$3,conv_loc(@$)); }
| exp "bit_and" exp  { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BIT_AND,$1,$3,conv_loc(@$)); }
| exp "||" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BOOL_OR,$1,$3,conv_loc(@$)); }
| exp "or" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BOOL_OR,$1,$3,conv_loc(@$)); }
| exp "|" exp        { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BIT_OR,$1,$3,conv_loc(@$)); }
| exp "bit_or" exp   { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BIT_OR,$1,$3,conv_loc(@$)); }
| exp "xor" exp      { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::BIT_XOR,$1,$3,conv_loc(@$)); }
| exp "<<" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::LEFT_SHIFT,$1,$3,conv_loc(@$)); }
| exp ">>" exp       { $$ = std::make_shared<GN::ast::BinaryOperator>(GN::ast::BinaryOperator::OperatorType::RIGHT_SHIFT,$1,$3,conv_loc(@$)); }
| exp "+=" exp       { 
                        using GN::ast::BinaryOperator; using enum BinaryOperator::OperatorType;
                        auto right = std::make_shared<BinaryOperator>(ADD,$1,$3,conv_loc(@$));
                        $$ = std::make_shared<BinaryOperator>(ASSIGN,$1,right,conv_loc(@$)); 
                     }
| exp "-=" exp       { 
                        using GN::ast::BinaryOperator; using enum BinaryOperator::OperatorType;
                        auto right = std::make_shared<BinaryOperator>(SUB,$1,$3,conv_loc(@$));
                        $$ = std::make_shared<BinaryOperator>(ASSIGN,$1,right,conv_loc(@$)); 
                     }
| exp "*=" exp       { 
                        using GN::ast::BinaryOperator; using enum BinaryOperator::OperatorType;
                        auto right = std::make_shared<BinaryOperator>(MUL,$1,$3,conv_loc(@$));
                        $$ = std::make_shared<BinaryOperator>(ASSIGN,$1,right,conv_loc(@$)); 
                     }
| exp "/=" exp       { 
                        using GN::ast::BinaryOperator; using enum BinaryOperator::OperatorType;
                        auto right = std::make_shared<BinaryOperator>(DIV,$1,$3,conv_loc(@$));
                        $$ = std::make_shared<BinaryOperator>(ASSIGN,$1,right,conv_loc(@$)); 
                     }
| exp "%=" exp       { 
                        using GN::ast::BinaryOperator; using enum BinaryOperator::OperatorType;
                        auto right = std::make_shared<BinaryOperator>(MOD,$1,$3,conv_loc(@$));
                        $$ = std::make_shared<BinaryOperator>(ASSIGN,$1,right,conv_loc(@$)); 
                     }
;

unary_operator:
  "+" exp            { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::PLUS, $2, conv_loc(@$)); }
| "-" exp            { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::MINUS, $2, conv_loc(@$)); }
| "not" exp          { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::BOOL_NOT, $2, conv_loc(@$)); }
| "!" exp            { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::BOOL_NOT, $2, conv_loc(@$)); }
| "bit_not" exp      { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::BIT_NOT, $2, conv_loc(@$)); }
| "~" exp            { $$ = std::make_shared<GN::ast::UnaryOperator>(GN::ast::UnaryOperator::OperatorType::BIT_NOT, $2, conv_loc(@$)); }
;

floating_point_literal:
  "floating point"   { $$ = std::make_shared<GN::ast::FloatingPointLiteral>($1,conv_loc(@$)); };

signed_integer_literal:
  "integer"          { $$ = std::make_shared<GN::ast::SignedIntegerLiteral>($1,conv_loc(@$)); };

string_literal:
  "string"           { $$ = std::make_shared<GN::ast::StringLiteral>($1,conv_loc(@$)); };

boolean_literal:
  "true"             { $$ = std::make_shared<GN::ast::BooleanLiteral>(true,conv_loc(@$)); };
| "false"            { $$ = std::make_shared<GN::ast::BooleanLiteral>(false,conv_loc(@$)); };

nil_literal:
  "nil"              { $$ = std::make_shared<GN::ast::NilLiteral>(conv_loc(@$)); };

exp_list:
  exp                { $$ = {$1}; }
| exp_list "," exp   { 
      $$ = std::move($1); 
      $$.push_back($3);
    }

parameter_list:
  %empty             { $$ = {}; }
| exp_list           { $$ = std::move($1); }

function_call:
  callable_exp "(" parameter_list ")" { 
      $$ = std::make_shared<GN::ast::FunctionCall>($1,std::move($3),conv_loc(@$)); 
    }

callable_exp:
  variable_reference     { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| function_call          { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| "(" callable_exp ")"   { $$ = $2; }

uncallable_exp:
  floating_point_literal { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| signed_integer_literal { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| string_literal         { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| binary_operator        { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| unary_operator         { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| boolean_literal        { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| nil_literal            { $$ = std::dynamic_pointer_cast<GN::ast::Expression>($1); }
| "(" uncallable_exp ")" { $$ = $2; }
| "(" error ")"          { 
      yyclearin;
      $$ = std::dynamic_pointer_cast<GN::ast::Expression>(std::make_shared<GN::ast::ErrorExpression>(conv_loc(@$)));
    }

exp:
  callable_exp           { $$ = $1; }
| uncallable_exp         { $$ = $1; }

variable_decl_statement:
  variable_decl ";"      { $$ = std::make_shared<GN::ast::VariableDeclStatement>($1,conv_loc(@$)); }
| variable_init ";"      { $$ = std::make_shared<GN::ast::VariableDeclStatement>($1,conv_loc(@$)); }

return_statement:
  "return" exp ";"            { $$ = std::make_shared<GN::ast::ReturnStatement>($2,conv_loc(@$)); }

stmt:
  variable_decl_statement { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| return_statement        { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| loop_statement          { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| if_statement            { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| break_statement         { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| block                   { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| assert_statement        { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| for_statement           { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| while_statement         { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }
| do_while_statement      { $$ = std::dynamic_pointer_cast<GN::ast::Statement>($1); }

loop_statement:
  "loop" block            { $$ = std::make_shared<GN::ast::LoopStatement>($2,conv_loc(@$)); }

break_statement:
  "break" ";"             { $$ = std::make_shared<GN::ast::BreakStatement>(conv_loc(@$)); }

assert_statement:
  "assert" exp ";"              { $$ = std::make_shared<GN::ast::AssertStatement>($2,std::nullopt,conv_loc(@$)); }
| "assert" exp "," exp ";"      { $$ = std::make_shared<GN::ast::AssertStatement>($2,$4,conv_loc(@$)); }

if_statement:
  alone_if_statement            { $$ = $1; }
| alone_if_statement else       { 
                                    $$ = $1;
                                    $$->add_cond_block($2);
                                }
| alone_if_statement elifs else {
                                    $$ = $1;
                                    $$->add_cond_blocks(std::move($2));
                                    $$->add_cond_block($3);
                                }

alone_if_statement:
  "if" "(" exp ")" block  { $$ = std::make_shared<GN::ast::IfStatement>(std::vector{GN::ast::IfStatement::CondBlock{$3, $5}},conv_loc(@$)); }

elifs:
  elif        { $$ = {$1}; }
| elifs elif  {
                $$ = std::move($1);
                $$.push_back($2);
              }
elif:
  "elif" "(" exp ")" block { $$ = {$3,$5}; }
else:
  "else" block { $$ = {nullptr,$2}; }

omittable_variable_init:
  %empty         { $$ = std::nullopt; }
| variable_init  { $$ = $1; }

omittable_exp:
  %empty         { $$ = std::nullopt; }
| exp            { $$ = $1; }

for_statement:
  "for" "(" omittable_variable_init ";" omittable_exp ";" omittable_exp ")" block {
     using namespace GN::ast;
     auto result = std::make_shared<Block>(conv_loc(@$));

     auto counter_init = $3;

     auto loop_block = std::make_shared<Block>(conv_loc(@$));

     auto break_stmt = std::make_shared<BreakStatement>(conv_loc(@5));
     auto if_block = std::make_shared<Block>(std::vector<std::shared_ptr<Sentence>>{break_stmt}, conv_loc(@5));
     std::shared_ptr<Expression> cond;
     if($5.has_value()){
        cond = std::make_shared<UnaryOperator>(UnaryOperator::OperatorType::BOOL_NOT, $5.value(), conv_loc(@5));
     }else{
        cond = std::make_shared<BooleanLiteral>(true, conv_loc(@5));
     }
     auto if_stmt = std::make_shared<IfStatement>(std::vector{IfStatement::CondBlock{.cond=cond,.block=if_block}},conv_loc(@5));

     auto update_counter = $7;

     auto block = $9;

     loop_block->add_sentences({if_stmt,block});
     if(update_counter.has_value()){
        loop_block->add_sentence(update_counter.value());
     }
     auto loop_stmt = std::make_shared<LoopStatement>(loop_block,conv_loc(@$));

     if(counter_init.has_value()){
        auto counter_init_stmt = std::make_shared<VariableDeclStatement>(counter_init.value(), conv_loc(@3));
        result->add_sentence(counter_init_stmt);
     }
     result->add_sentence(loop_stmt);
     $$ = result;
  }

while_statement:
  "while" "("exp ")" block {
     using namespace GN::ast;
     auto result = std::make_shared<Block>(conv_loc(@$));

     auto loop_block = std::make_shared<Block>(conv_loc(@$));

     auto break_stmt = std::make_shared<BreakStatement>(conv_loc(@3));
     auto if_block = std::make_shared<Block>(std::vector<std::shared_ptr<Sentence>>{break_stmt}, conv_loc(@3));
     std::shared_ptr<Expression> cond;
     cond = std::make_shared<UnaryOperator>(UnaryOperator::OperatorType::BOOL_NOT, $3, conv_loc(@3));
     auto if_stmt = std::make_shared<IfStatement>(std::vector{IfStatement::CondBlock{.cond=cond,.block=if_block}},conv_loc(@3));

     auto block = $5;

     loop_block->add_sentences({if_stmt,block});
     auto loop_stmt = std::make_shared<LoopStatement>(loop_block,conv_loc(@$));

     result->add_sentence(loop_stmt);
     $$ = result;
  }

do_while_statement:
  "do" block "while" "(" exp ")" {
     using namespace GN::ast;
     auto result = std::make_shared<Block>(conv_loc(@$));

     auto loop_block = std::make_shared<Block>(conv_loc(@$));

     auto break_stmt = std::make_shared<BreakStatement>(conv_loc(@5));
     auto if_block = std::make_shared<Block>(std::vector<std::shared_ptr<Sentence>>{break_stmt}, conv_loc(@5));
     std::shared_ptr<Expression> cond;
     cond = std::make_shared<UnaryOperator>(UnaryOperator::OperatorType::BOOL_NOT, $5, conv_loc(@5));
     auto if_stmt = std::make_shared<IfStatement>(std::vector{IfStatement::CondBlock{.cond=cond,.block=if_block}},conv_loc(@5));

     auto block = $2;

     loop_block->add_sentences({block,if_stmt});
     auto loop_stmt = std::make_shared<LoopStatement>(loop_block,conv_loc(@$));

     result->add_sentence(loop_stmt);
     $$ = result;
  }

%%

void
GN::yy::Parser::error (const location_type& l, const std::string& m)
{
    drv.print_error(l,m);
}
