#pragma once
#include "scanner.h"

#define LEN 50

enum nonterminalCode {
	id_none = 0,
	id_empty,
	id_signal_program,
	id_program,
	id_procedure_identifier,
	id_identifier,
	id_block,
	id_declarations,
	id_statement_lst,
	id_procedure_declarations,
	id_procedure,
	id_params_lst,
	id_declarations_lst,
	id_declaration,
	id_variable_identifier,
	id_identifiers_lst,
	id_attributes_lst,
	id_attribute
};

struct Node {
	lex_string Token;
	uint nonTerminalCode;
	vector<Node*> children;
};

void nt_SIG_PROGRAM(Node *&Tree, lex_string &curr_token);
void print_Tree(const Node *Tree, ostream &out, uint tab = 0);
void free_Tree(Node *Tree);
void parser(Node *&Tree);