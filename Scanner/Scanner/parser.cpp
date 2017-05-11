#include "parser.h"
#include "scanner.h"

static map<uint, string> NonTerminals;

enum nonterminalCode{
	signal_program = 1,
	program,
	procedure_identifier,
	//...etc
};

#define LEN 50
typedef struct tNode {
	lex_string Token;
	char nonTerminalCode;
	vector<tNode*> children;
}Node;

static Node *Tree = NULL;


void initNonTerminals() {
	int i = 1;
	NonTerminals.insert(pair<uint, string>(i++, "<signal_program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<procedure_identifier>"));
	//... etc
}


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


void nonterminal_IDN(Node *&Tree, lex_string &curr_token) {
	//
	curr_token = get_next_token();
}

void nonterminal_PROC_IDN(Node *&Tree, lex_string &curr_token) {
	nonterminal_IDN(Tree, curr_token);
}

void nonterminal_DECLARATIONS(Node *&Tree, lex_string &curr_token) {

}

void nonterminal_STATEMENT_LST(Node *&Tree, lex_string &curr_token) {

}

void nonterminal_BLOCK(Node *&Tree, lex_string &curr_token) {
	nonterminal_DECLARATIONS(Tree, curr_token);

	if (curr_token.lex_code != search_key_tab("BEGIN"))
		cerr << "'BEGIN' KEY_WORD EXPECTED\n";
	else {
		curr_token = get_next_token();
		nonterminal_STATEMENT_LST(Tree, curr_token);

		if (curr_token.lex_code != search_key_tab("END"))
			cerr << "'END' KEY_WORD EXPECTED\n";
	}
}

void nonterminal_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROGRAM = new Node;
	node_ntPROGRAM->nonTerminalCode = program;
	Tree->children.push_back(node_ntPROGRAM);

	if (curr_token.lex_code != search_key_tab("PROGRAM"))
		cerr << "'PROGRAM' KEY_WORD expected...";
	else {
		curr_token = get_next_token();
		nonterminal_PROC_IDN(node_ntPROGRAM, curr_token);

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else {
			curr_token = get_next_token();
			nonterminal_BLOCK(node_ntPROGRAM, curr_token);
		}

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
	}
}

void nonterminal_SIG_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Tree = new Node;
	Tree->nonTerminalCode = signal_program;
	nonterminal_PROGRAM(Tree, curr_token);
}







