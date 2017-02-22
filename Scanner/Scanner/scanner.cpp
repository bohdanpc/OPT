#include <map>
#include <fstream>
#include <iostream>

using namespace std;

typedef enum {
	whitespace = 0,
	digit,
	letter,
	separator,
	multi_separator,
	comment,
	invalid_char
} ATTRIBUTE;

typedef struct {
	char value;
	unsigned int attr;
} t_symbol;

map<char, unsigned int> Attributes;

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

void scanFile() {
	int line_count = 0, row_count = 0, row_token_num = 0;
	t_symbol curr_symbol;
	string tmp_token;
	ifstream fin("program.signal");

	if (fin.eof())
		cerr << "The file is empty!\n";

	get_next_symbol(fin, curr_symbol);

	if (fin.eof())
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

		case digit:
			do {
				row_count++;
				tmp_token += curr_symbol.value;
				get_next_symbol(fin, curr_symbol);
			} while (curr_symbol.attr == digit && curr_symbol.value != EOF);
			//TODO insert to the appropriate table
			break;

		case letter:
			do {
				row_count;
				tmp_token += curr_symbol.value;
				get_next_symbol(fin, curr_symbol);
			} while ((curr_symbol.attr == digit || curr_symbol.attr == letter) && curr_symbol.value != EOF);
			//TODO insert to the appropriate table
			break;

		case separator:

		case multi_separator:
		case comment:
		case invalid_char:
		default:
			cerr << "Invalid char\n";
			get_next_symbol(fin, curr_symbol);
		}

	} while (curr_symbol.value != EOF);

}