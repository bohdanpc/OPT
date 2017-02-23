#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

typedef enum {
	whitespace = 0,
	digit,
	letter,
	separator,
	invalid_char
} ATTRIBUTE;

typedef struct {
	char value;
	unsigned int attr;
} t_symbol;

typedef struct {
	int lex_code;
	int line_num;
	int row_num;
}lex_string;

static int key_word_idx = 401;
static int constant_idx = 501;
static int identifier_idx = 1001;

vector<lex_string> Tokens;
map<char, unsigned int> Attributes;

map<string, unsigned int> idn_tab;
map<string, unsigned int> const_tab;
map<string, unsigned int> key_tab;

void idn_tab_init() {
	idn_tab.insert(pair<string, unsigned int>("SIGNAL", identifier_idx++));
	idn_tab.insert(pair<string, unsigned int>("COMPLEX", identifier_idx++));
	idn_tab.insert(pair<string, unsigned int>("INTEGER", identifier_idx++));
	idn_tab.insert(pair<string, unsigned int>("FLOAT", identifier_idx++));
	idn_tab.insert(pair<string, unsigned int>("BLOCKFLOAT", identifier_idx++));
	idn_tab.insert(pair<string, unsigned int>("EXT", identifier_idx++));
}

void key_tab_init() {
	key_tab.insert(pair<string, unsigned int>("PROGRAM", key_word_idx++));
	key_tab.insert(pair<string, unsigned int>("BEGIN", key_word_idx++));
	key_tab.insert(pair<string, unsigned int>("END", key_word_idx++));
	key_tab.insert(pair<string, unsigned int>("PROCEDURE", key_word_idx++));
}

void const_tab_init() {

}


//need to add ':'
void fillAttributes() {
	//insert invalid ascii characters
	for (int i = 0; i < 8; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));

	//insert equivalent to whitespace chars
	for (int i = 8; i < 14; i++) 
		Attributes.insert(pair<char, unsigned int>((char)i, whitespace));

	//insert invalid ascii characters
	for (int i = 14; i < 31; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));

	//insert whitespace, ascii code 32
	Attributes.insert(pair<char, unsigned int>((char)32, whitespace));
	
	//insert invalid ascii characters
	for (int i = 33; i < 44; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));

	//insert separator ','
	Attributes.insert(pair<char, unsigned int>((char)44, separator));

	//insert invalid ascii characters
	for (int i = 45; i < 48; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));

	//insert constants => digits
	for (int i = 48; i < 58; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, digit));

	//insert invalid char
	Attributes.insert(pair<char, unsigned int>((char)58, invalid_char));

	//insert separator ';'
	Attributes.insert(pair<char, unsigned int>((char)59, separator));

	//insert invalid ascii characters
	for (int i = 60; i < 65; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));

	//insert ids => letters
	for (int i = 65; i < 91; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, letter));

	//insert invalid characters 
	for (int i = 91; i < 128; i++)
		Attributes.insert(pair<char, unsigned int>((char)i, invalid_char));
}

//get next character from the file
void get_next_symbol(ifstream &fin, t_symbol &res_symbol) {
	res_symbol.value = fin.get();
	res_symbol.attr = Attributes.at(res_symbol.value);
}

void add_new_token(int lex_code, int line_count, int row_token_num) {
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
		return constant_idx++;
}

/**
 * @param tmp_token - str to find in key_tab
 * @return key_tab_idx if found or -1 otherwise
*/
int search_key_tab(const string &tmp_token) {
	auto it = key_tab.find(tmp_token);
	if (it != const_tab.end())
		return it->second;
	else
		return -1;
}

/**
* @param tmp_token - str to find in idn_tab
* @return idn_tab_idx if found or next_idn_idx otherwise
*/
int search_idn_tab(const string &tmp_token) {
	auto it = idn_tab.find(tmp_token);
	if (it != idn_tab.end())
		return it->second;
	else
		return identifier_idx++;
}

void scanFile() {
	int line_count = 0, row_count = 0, row_token_num = 0;
	t_symbol curr_symbol;
	string tmp_token;
	ifstream fin("program.signal");
	int lex_code = 0;

	get_next_symbol(fin, curr_symbol);
	
	if (curr_symbol.value == EOF)
		cerr << "The file is empty!\n";
	
	//iterate through the file
	do {
		tmp_token.clear();
		row_token_num = row_count;

		switch (curr_symbol.attr) {
		case whitespace:
			if (curr_symbol.value == '\n') {
				line_count++;
				row_count = 0;
			}
			do {
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

			/*BEGIN insert to the appropriate table*/
			int lex_code = search_const_tab(tmp_token);
			add_new_token(lex_code, line_count, row_token_num);
			/*END insert to the appropriate table*/
			break;
		}

		case letter:
			row_token_num = row_count;
			do {
				row_count;
				tmp_token += curr_symbol.value;
				get_next_symbol(fin, curr_symbol);
			} while ((curr_symbol.attr == digit || curr_symbol.attr == letter) && curr_symbol.value != EOF);
			
			/*BEGIN insert to the appropriate table*/
			if ((lex_code = search_key_tab(tmp_token)) == -1)
				lex_code = search_idn_tab(tmp_token);

			add_new_token(lex_code, line_count, row_token_num);
			/*END insert to the appropriate table*/

			break;

		case separator:
			row_token_num = row_count;
			get_next_symbol(fin, curr_symbol);
			row_count++;

			lex_code = curr_symbol.attr;
			add_new_token(lex_code, line_count, row_token_num);
			break;

		case invalid_char:
		default:
			cerr << "Invalid char\n";
			get_next_symbol(fin, curr_symbol);
			row_count++;
		}

	} while (curr_symbol.value != EOF);

}