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

static vector<lex_string> Tokens;
static map<char, uint> Attributes;

static map<string, uint> idn_tab;
static map<string, uint> key_tab;

void scanner(std::istream &fin, std::ostream &ferr);
void print_tokens_string(std::ostream &out);
void print_key_tab(std::ostream &out);
void print_idn_tab(std::ostream &out);

int search_key_tab(const string &tmp_token);
int search_idn_tab(const string &tmp_token);