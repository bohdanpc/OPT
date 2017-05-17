#pragma once
#include "scanner.h"

#define LEN 50

struct Node {
	lex_string Token;
	uint nonTerminalCode;
	vector<Node*> children;
};

void nt_SIG_PROGRAM(Node *&Tree, lex_string &curr_token);
void print_Tree(const Node *Tree, ostream &out, uint tab = 0);
void parser();