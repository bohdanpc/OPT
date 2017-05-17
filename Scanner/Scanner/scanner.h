#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

typedef unsigned int uint;

#define NO_LEXEM			 0
#define IDENTIFIER_IDX_START 1001
#define KEYWORD_IDX_START	 401

enum keyword_idx {
	idx_program = KEYWORD_IDX_START,
	idx_begin,
	idx_end,
	idx_procedure,
	idx_const
};

enum idn_idx {
	idx_signal = IDENTIFIER_IDX_START,
	idx_complex,
	idx_integer,
	idx_float,
	idx_blockfloat,
	idx_ext
};

typedef enum {
	whitespace = 0,
	digit,
	letter,
	separator,
	left_bracket,
	invalid_char
} ATTRIBUTE;

typedef struct {
	char value;
	uint attr;
} t_symbol;

typedef struct {
	uint lex_code;
	uint line_num;
	uint row_num;
}lex_string;

extern vector<lex_string> Tokens;
extern map<char, uint> Attributes;

extern map<string, uint> idn_tab;
extern map<string, uint> key_tab;

void scanner(std::istream &fin, std::ostream &ferr);
void print_tokens_string(std::ostream &out);
void print_key_tab(std::ostream &out);
void print_idn_tab(std::ostream &out);

int search_key_tab(const string &tmp_token);
int search_idn_tab(const string &tmp_token);

const string search_tabs(const uint lex_code);