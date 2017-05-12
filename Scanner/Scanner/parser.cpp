#include "parser.h"
#include "scanner.h"

static map<uint, string> NonTerminals;

enum nonterminalCode{
	id_none = 0,
	id_signal_program = 1,
	id_program,
	id_procedure_identifier,
	id_identifier,
	id_block,
	id_declarations,
	id_statement_lst,
	id_procedure_declarations,
	//...etc
};

#define LEN 50
struct Node{
	lex_string Token;
	char nonTerminalCode;
	vector<Node*> children;
};

static Node *Tree = NULL;

Node* push_to_Tree(Node *&tree, lex_string &token, nonterminalCode ntCode) {
	Node *node = new Node;
	if (ntCode)
		node->nonTerminalCode = ntCode;
	else
		node->Token = token;

	Tree->children.push_back(node);
	return node;
}

void initNonTerminals() {
	int i = 1;
	NonTerminals.insert(pair<uint, string>(i++, "<signal_program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<program>"));
	NonTerminals.insert(pair<uint, string>(i++, "<procedure_identifier>"));
	//... etc
}

//finished
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

//finished one
void nonterminal_IDN(Node *&Tree, lex_string &curr_token) {
	push_to_Tree(Tree, curr_token, id_identifier);
	curr_token = get_next_token();
}

//finished one
void nonterminal_PROC_IDN(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROC_IDN = push_to_Tree(Tree, curr_token, id_procedure_identifier);
	nonterminal_IDN(node_ntPROC_IDN, curr_token);
}

void nonterminal_PROC_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	//add to tree
	//nonterminal_PROCEDURE();
	while (curr_token.lex_code == (uint)';') {
		curr_token = get_next_token();
		//nonterminal_PROCEDURE();
	}
}

//finished one
void nonterminal_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	Node *nt_proc_declarations = push_to_Tree(Tree, curr_token, id_procedure_declarations);
	nonterminal_PROC_DECLARATIONS(nt_proc_declarations, curr_token);
}

//finished one
void nonterminal_STATEMENT_LST(Node *&Tree, lex_string &curr_token) {
	push_to_Tree(Tree, curr_token, id_statement_lst);
}

//finished one
void nonterminal_BLOCK(Node *&Tree, lex_string &curr_token) {
	Node *node_ntBLOCK = push_to_Tree(Tree, curr_token, id_block);
	nonterminal_DECLARATIONS(node_ntBLOCK, curr_token);

	if (curr_token.lex_code != search_key_tab("BEGIN"))
		cerr << "'BEGIN' KEY_WORD EXPECTED\n";
	else {
		push_to_Tree(Tree, curr_token, id_identifier);
		curr_token = get_next_token();
		nonterminal_STATEMENT_LST(Tree, curr_token);

		if (curr_token.lex_code != search_key_tab("END"))
			cerr << "'END' KEY_WORD EXPECTED\n";
		else {
			push_to_Tree(Tree, curr_token, id_identifier);
			curr_token = get_next_token();
		}
	}
}

//finished one (1)
void nonterminal_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROGRAM = push_to_Tree(Tree, curr_token, id_program);

	if (curr_token.lex_code != search_key_tab("PROGRAM"))
		cerr << "'PROGRAM' KEY_WORD expected...";
	else {
		push_to_Tree(Tree, curr_token, id_none);
		curr_token = get_next_token();
		nonterminal_PROC_IDN(node_ntPROGRAM, curr_token);

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else {
			push_to_Tree(Tree, curr_token, id_none);
			curr_token = get_next_token();
			nonterminal_BLOCK(node_ntPROGRAM, curr_token);
		}

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else
			push_to_Tree(Tree, curr_token, id_none);
	}
}

//finished one (0)
void nonterminal_SIG_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Tree = new Node;
	Tree->nonTerminalCode = id_signal_program;
	nonterminal_PROGRAM(Tree, curr_token);
}







