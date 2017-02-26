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

static uint key_word_idx = 401;
static uint constant_idx = 501;
static uint identifier_idx = 1001;

vector<lex_string> Tokens;
map<char, uint> Attributes;

map<string, uint> idn_tab;
map<string, uint> key_tab;

map<string, uint> const_tab;

void idn_tab_init() {
	idn_tab.insert(pair<string, uint>("SIGNAL", identifier_idx++));
	idn_tab.insert(pair<string, uint>("COMPLEX", identifier_idx++));
	idn_tab.insert(pair<string, uint>("INTEGER", identifier_idx++));
	idn_tab.insert(pair<string, uint>("FLOAT", identifier_idx++));
	idn_tab.insert(pair<string, uint>("BLOCKFLOAT", identifier_idx++));
	idn_tab.insert(pair<string, uint>("EXT", identifier_idx++));
}

void key_tab_init() {
	key_tab.insert(pair<string, uint>("PROGRAM", key_word_idx++));
	key_tab.insert(pair<string, uint>("BEGIN", key_word_idx++));
	key_tab.insert(pair<string, uint>("END", key_word_idx++));
	key_tab.insert(pair<string, uint>("PROCEDURE", key_word_idx++));
}

void const_tab_init() {

}


void fill_Attributes() {
	//insert invalid ascii characters
	for (int i = 0; i < 8; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));

	//insert equivalent to whitespace chars
	for (int i = 8; i < 14; i++) 
		Attributes.insert(pair<char, uint>((char)i, whitespace));

	//insert invalid ascii characters
	for (int i = 14; i < 32; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));

	//insert whitespace, ascii code 32
	Attributes.insert(pair<char, uint>((char)32, whitespace));
	
	//insert invalid ascii characters
	for (int i = 33; i < 40; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));

	//insert separators '(',  ')'
	for (int i = 40; i < 42; i++)
		Attributes.insert(pair<char, uint>((char)i, separator));

	//insert invalid ascii characters
	for (int i = 42; i < 44; i++)
		Attributes.insert(pair<char, uint>((char)43, invalid_char));

	//insert separator ','
	Attributes.insert(pair<char, uint>((char)44, separator));

	//insert invalid ascii characters
	for (int i = 45; i < 48; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));

	//insert constants => digits
	for (int i = 48; i < 58; i++)
		Attributes.insert(pair<char, uint>((char)i, digit));

	//insert separators ':',  ';'
	for (int i = 58; i < 60; i++)
		Attributes.insert(pair<char, uint>((char)i, separator));

	//insert invalid ascii characters
	for (int i = 60; i < 65; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));

	//insert ids => letters
	for (int i = 65; i < 91; i++)
		Attributes.insert(pair<char, uint>((char)i, letter));

	//insert invalid characters 
	for (int i = 91; i < 128; i++)
		Attributes.insert(pair<char, uint>((char)i, invalid_char));
}

//get next character from the file
void get_next_symbol(ifstream &fin, t_symbol &res_symbol) {
	res_symbol.value = fin.get();
	try {
		res_symbol.attr = Attributes.at(res_symbol.value);
	}
	//if symbol was not found in Attributes table then make it invalid
	catch(...) {
		res_symbol.attr = invalid_char;
	}
}


void add_new_token(uint lex_code, uint line_count, uint row_token_num) {
	lex_string tmp_lex_string;

	tmp_lex_string.lex_code = lex_code;
	tmp_lex_string.line_num = line_count;
	tmp_lex_string.row_num = row_token_num;
	Tokens.push_back(tmp_lex_string);
}


int search_const_tab(const string &tmp_token) {
	auto it = const_tab.find(tmp_token);
	if (it != const_tab.end())
		return it->second;
	else
		return -1;
}

/**
 * @param tmp_token - str to find in key_tab
 * @return key_tab_idx if found or -1 otherwise
*/
int search_key_tab(const string &tmp_token) {
	auto it = key_tab.find(tmp_token);
	if (it != key_tab.end())
		return it->second;
	else
		return -1;
}

/**
* @param tmp_token - str to find in idn_tab
* @return idn_tab_idx if found or -1 otherwise
*/
int search_idn_tab(const string &tmp_token) {
	auto it = idn_tab.find(tmp_token);
	if (it != idn_tab.end())
		return it->second;
	else
		return -1;
}

void add_to_idn_tab(const string &idn, const uint code) {
	idn_tab.insert(pair<string, uint>(idn, code));
}

void scanFile(ifstream &fin) {
	uint line_count = 1, row_count = 1, row_token_num = 1;
	t_symbol curr_symbol;
	string tmp_token;
	uint lex_code = 0;

	if (!fin)
		throw exception("the stream is bad!");

	get_next_symbol(fin, curr_symbol);
	
	if (curr_symbol.value == EOF)
		throw exception("The file is empty!\n");
	
	//iterate through the file
	do {
		tmp_token.clear();

		switch (curr_symbol.attr) {
		case whitespace:
			do {
				if (curr_symbol.value == '\n') {
					line_count++;
					row_count = 0;
				}
				row_count++;
				get_next_symbol(fin, curr_symbol);
			} while (curr_symbol.attr == whitespace && curr_symbol.value != EOF);
			break;

		case digit: {
			row_token_num = row_count;
			do {
				row_count++;
				tmp_token += curr_symbol.value;
				get_next_symbol(fin, curr_symbol);
			} while (curr_symbol.attr == digit && curr_symbol.value != EOF);

			/*insert new token to 'Tokens vector' table*/
			int lex_code = search_const_tab(tmp_token);
			add_new_token(lex_code, line_count, row_token_num);
			break;
		}

		case letter:
			row_token_num = row_count;
			do {
				row_count++;
				tmp_token += curr_symbol.value;
				get_next_symbol(fin, curr_symbol);
			} while ((curr_symbol.attr == digit || curr_symbol.attr == letter) && curr_symbol.value != EOF);
			
			/*insert new token to 'Tokens vector' table*/
			if ((lex_code = search_key_tab(tmp_token)) == -1)
				if ((lex_code = search_idn_tab(tmp_token)) == -1) {
					lex_code = identifier_idx++;
					add_to_idn_tab(tmp_token, lex_code);
				}
			add_new_token(lex_code, line_count, row_token_num);
			break;

		case separator:
			lex_code = curr_symbol.value;
			add_new_token(lex_code, line_count, row_count);
			get_next_symbol(fin, curr_symbol);
			row_count++;
			break;

		case invalid_char:
		default:
			cerr << "Invalid char\n";
			get_next_symbol(fin, curr_symbol);
			row_count++;
		} //switch

	} while (curr_symbol.value != EOF);
}


void print_tokens_string(ofstream &out) {
	if (!out)
		throw exception("output stream for tokens_string is bad");
	out << "---Tokens string---\n\n";
	out << left << setw(10) << "Lex_code" << setw(10) << "Line_num" << setw(10) << "Row_num" << endl;
	for (auto it = Tokens.begin(); it != Tokens.end(); it++) {
		out << setw(10) << it->lex_code;
		out << setw(10) << it->line_num;
		out << setw(10) << it->row_num;
		out << endl;
	}
}


void print_key_tab(ofstream &out) {
	if (!out)
		throw exception("output stream for key_tab is bad");
	out << "---Key table---\n\n";
	out << left << setw(15) << "Key word" << setw(15) << "Code" << endl;
	for (auto it = key_tab.begin(); it != key_tab.end(); it++) {
		out << setw(15) << it->first;
		out << setw(15) << it->second;
		out << endl;
	}
}


void print_idn_tab(ofstream &out) {
	if (!out)
		throw exception("output stream for idn_Tab is bad");
	out << "---Identifier table---\n\n";
	out << left << setw(15) << "Identifier" << setw(15) << "Code" << endl;
	for (auto it = idn_tab.begin(); it != idn_tab.end(); it++) {
		out << setw(15) << it->first;
		out << setw(15) << it->second;
		out << endl;
	}
}


void scanner(ifstream &fin) {
	idn_tab_init();
	key_tab_init();
	fill_Attributes();
	scanFile(fin);
}