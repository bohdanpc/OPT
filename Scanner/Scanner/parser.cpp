#include "parser.h"
#include "scanner.h"

static map<uint, string> NonTerminals;

enum {
	signal_program = 1,
	program,
	procedure_identifier,
	//...etc
} nonterminalCode;

void initNonTerminals() {
	int i = 1;
	NonTerminals.insert(pair<uint, string>(i++, "<signal_program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<procedure_identifier>"));
	//... etc
}

#define LEN 50
typedef struct tNode{
	lex_string Token;
	char nonTerminalName[LEN] = { 0 };
	vector<tNode> children;
}Node;


lex_string get_next_token() {
	static uint curr_lex_code = 0;
	try {
		return Tokens.at(curr_lex_code);
		curr_lex_code++;
	}
	catch (out_of_range excp) {
		throw excp;
	}
}


void nonterminal_IDN(lex_string &curr_token) {
	//
	curr_token = get_next_token();
}

void nonterminal_PROC_IDN(lex_string &curr_token) {
	nonterminal_IDN(curr_token);
}

void nonterminal_DECLARATIONS(lex_string &curr_token) {

}

void nonterminal_STATEMENT_LST(lex_string &curr_token) {

}

void nonterminal_BLOCK(lex_string &curr_token) {
	nonterminal_DECLARATIONS(curr_token);

	if (curr_token.lex_code != search_key_tab("BEGIN"))
		cerr << "'BEGIN' KEY_WORD EXPECTED\n";
	else {
		curr_token = get_next_token();
		nonterminal_STATEMENT_LST(curr_token);

		if (curr_token.lex_code != search_key_tab("END"))
			cerr << "'END' KEY_WORD EXPECTED\n";
	}
}

void nonterminal_PROGRAM(lex_string &curr_token) {
	if (curr_token.lex_code != search_key_tab("PROGRAM"))
		cerr << "'PROGRAM' KEY_WORD expected...";
	else {
		curr_token = get_next_token();
		nonterminal_PROC_IDN(curr_token);

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else {
			curr_token = get_next_token();
			nonterminal_BLOCK(curr_token);
		}

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
	}
}








