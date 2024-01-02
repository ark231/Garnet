%skeleton "lalr1.cc"
%require "3.8.2"
%defines
%define api.parser.class {WomuYuroParser}

%define api.namespace {WomuYuro}

%define api.value.type  variant

%define api.token.constructor

%define parse.trace
%define parse.error detailed
%define parse.lac full


%token <int64_t> SIGNED_INTEGER "signed integer"

%nterm <int64_t> exp

%printer { yyo << $$; } <*>;

%%
%start unit;
unit: exp {yyo<<$1;};
    
exp: "signed integer";

%%
