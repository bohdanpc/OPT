#include "parser.h"

static map<uint, string> NonTerminals;

enum nonterminalCode{
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
	id_attribute,
};

bool search_idn_tab(uint lex_code) {
	if (lex_code > IDENTIFIER_IDX_START)
		return true;
	return false;
}

Node* push_to_Tree(Node *&tree, lex_string &token, nonterminalCode ntCode) {
	Node *node = new Node;
	if (ntCode)
		node->nonTerminalCode = ntCode;
	else
		node->Token = token;

	tree->children.push_back(node);
	return node;
}

void initNonTerminals() {
	NonTerminals.insert(pair<uint, string>(id_empty, "<empty>"));
	NonTerminals.insert(pair<uint, string>(id_signal_program, "<signal_program>"));
	NonTerminals.insert(pair<uint, string>(id_program, "<program>"));
	NonTerminals.insert(pair<uint, string>(id_procedure_identifier, "<procedure_identifier>"));
	NonTerminals.insert(pair<uint, string>(id_identifier, "<identifier>"));
	NonTerminals.insert(pair<uint, string>(id_block, "<block>"));
	NonTerminals.insert(pair<uint, string>(id_declarations, "<declarations>"));
	NonTerminals.insert(pair<uint, string>(id_statement_lst, "<statement_lst>"));
	NonTerminals.insert(pair<uint, string>(id_procedure_declarations, "<procedure_declarations>"));
	NonTerminals.insert(pair<uint, string>(id_procedure, "<procedure>"));
	NonTerminals.insert(pair<uint, string>(id_params_lst, "<params_lst>"));
	NonTerminals.insert(pair<uint, string>(id_declarations_lst, "<declarations_lst>"));
	NonTerminals.insert(pair<uint, string>(id_declaration, "<declaration>"));
	NonTerminals.insert(pair<uint, string>(id_variable_identifier, "<variable_identifier>"));
	NonTerminals.insert(pair<uint, string>(id_identifiers_lst, "<identifiers_lst>"));
	NonTerminals.insert(pair<uint, string>(id_attributes_lst, "<attributes_lst>"));
	NonTerminals.insert(pair<uint, string>(id_attribute, "<attribute>"));
}

void print_Tree(const Node *Tree, ostream &out, uint tab) {
	if (Tree) {
		if (Tree->nonTerminalCode != id_none)
			out << string(tab, '\t') << NonTerminals.at(Tree->nonTerminalCode) << endl;
		else
			out << string(tab, '\t') << search_tabs(Tree->Token.lex_code) << endl;

		for (auto it = Tree->children.begin(); it != Tree->children.end(); it++)
			print_Tree(*it, out, tab + 1);
	}
}


//finished
lex_string get_next_token() {
	static uint curr_lex_code = 0;
	try {
		lex_string res = Tokens.at(curr_lex_code);
		curr_lex_code++;
		return res;
	}
	catch (out_of_range excp) {
		throw excp;
	}
}

//finished one
void nt_IDN(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_IDENTIFIER = push_to_Tree(Tree, curr_token, id_identifier);
	push_to_Tree(node_nt_IDENTIFIER, curr_token, id_none);
	curr_token = get_next_token();
}

//finished one
void nt_PROC_IDN(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROC_IDN = push_to_Tree(Tree, curr_token, id_procedure_identifier);
	nt_IDN(node_ntPROC_IDN, curr_token);
}

//finished
void nt_VARIABLE_ID(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_VARIABLE_ID = push_to_Tree(Tree, curr_token, id_variable_identifier);
	nt_IDN(node_nt_VARIABLE_ID, curr_token);
}

//finished one
void nt_IDENTIFIERS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_IDENTIFIERS_LST = push_to_Tree(Tree, curr_token, id_procedure_declarations);
	while (curr_token.lex_code == (uint)',') {
		push_to_Tree(node_nt_IDENTIFIERS_LST, curr_token, id_none);
		curr_token = get_next_token();
		nt_VARIABLE_ID(node_nt_IDENTIFIERS_LST, curr_token);
	}
	push_to_Tree(node_nt_IDENTIFIERS_LST, curr_token, id_empty);
}

//finished
bool nt_ATTRIBUTE(Node *&Tree, lex_string &curr_token) {
	if (curr_token.lex_code != search_idn_tab("SIGNAL") &&
		curr_token.lex_code != search_idn_tab("COMPLEX") &&
		curr_token.lex_code != search_idn_tab("INTEGER") &&
		curr_token.lex_code != search_idn_tab("FLOAT") &&
		curr_token.lex_code != search_idn_tab("BLOCKFLOAT") &&
		curr_token.lex_code != search_idn_tab("EXT"))
		return false;

	Node *node_nt_ATTRIBUTE = push_to_Tree(Tree, curr_token, id_attribute);
	push_to_Tree(node_nt_ATTRIBUTE, curr_token, id_none);
	curr_token = get_next_token();
	return true;
}

//finished ???
void nt_ATTRIBUTE_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_ATTRIBUTE_LST = push_to_Tree(Tree, curr_token, id_procedure_declarations);
	while (nt_ATTRIBUTE(node_nt_ATTRIBUTE_LST, curr_token));

	push_to_Tree(node_nt_ATTRIBUTE_LST, curr_token, id_empty);
}

//MAYBE FINISHED
bool nt_DECLARATION(Node *&Tree, lex_string &curr_token) {
	if (!search_idn_tab(curr_token.lex_code))
		return false;

	Node *node_nt_DECLARATION = push_to_Tree(Tree, curr_token, id_declaration);
	nt_VARIABLE_ID(node_nt_DECLARATION, curr_token);
	nt_IDENTIFIERS_LST(node_nt_DECLARATION, curr_token);

	if (curr_token.lex_code != (uint)':')
		cerr << "':' Expected\n";
	else {
		nt_ATTRIBUTE(node_nt_DECLARATION, curr_token);
		nt_ATTRIBUTE_LST(node_nt_DECLARATION, curr_token);

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else {
			push_to_Tree(node_nt_DECLARATION, curr_token, id_none);
			curr_token = get_next_token();
		}
	}
	return true;
}

//finished one
void nt_DECLARATIONS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_ntDECLARATIONS_LST = push_to_Tree(Tree, curr_token, id_declarations_lst);
	while (nt_DECLARATION(node_ntDECLARATIONS_LST, curr_token));

	push_to_Tree(node_ntDECLARATIONS_LST, curr_token, id_empty);
}

//finished one
void nt_PARAMS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPARAMS_LST = push_to_Tree(Tree, curr_token, id_params_lst);

	if (curr_token.lex_code != (uint)'(')
		push_to_Tree(node_ntPARAMS_LST, curr_token, id_empty);
	else {
		push_to_Tree(node_ntPARAMS_LST, curr_token, id_none);
		curr_token = get_next_token();
		nt_DECLARATIONS_LST(node_ntPARAMS_LST, curr_token);

		if (curr_token.lex_code != (uint)')')
			cerr << "')' EXPECTED\n";
		else {
			push_to_Tree(node_ntPARAMS_LST, curr_token, id_none);
			curr_token = get_next_token();
		}
	}
}

//finished one
bool nt_PROCEDURE(Node *&Tree, lex_string &curr_token) {
	if (curr_token.lex_code != search_key_tab("PROCEDURE"))
		return false;

	Node *node_ntPROCEDURE = push_to_Tree(Tree, curr_token, id_procedure);
	push_to_Tree(node_ntPROCEDURE, curr_token, id_none);
	curr_token = get_next_token();
	nt_PROC_IDN(node_ntPROCEDURE, curr_token);
	nt_PARAMS_LST(node_ntPROCEDURE, curr_token);

	if (curr_token.lex_code != (uint)';')
		cerr << "';' EXPECTED\n";
	else {
		push_to_Tree(node_ntPROCEDURE, curr_token, id_none);
		curr_token = get_next_token();
	}
	return true;
}

//MAYBE finished
void nt_PROC_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	Node *nt_proc_declarations = push_to_Tree(Tree, curr_token, id_procedure_declarations);
	while (nt_PROCEDURE(nt_proc_declarations, curr_token));

	push_to_Tree(nt_proc_declarations, curr_token, id_empty);
}

//finished one
void nt_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	Node *nt_declarations = push_to_Tree(Tree, curr_token, id_declarations);
	nt_PROC_DECLARATIONS(nt_declarations, curr_token);
}

//finished one
void nt_STATEMENT_LST(Node *&Tree, lex_string &curr_token) {
	Node *nt_STATEMENT_LST = push_to_Tree(Tree, curr_token, id_statement_lst);
	push_to_Tree(nt_STATEMENT_LST, curr_token, id_empty);
}

//finished one
void nt_BLOCK(Node *&Tree, lex_string &curr_token) {
	Node *node_ntBLOCK = push_to_Tree(Tree, curr_token, id_block);
	nt_DECLARATIONS(node_ntBLOCK, curr_token);

	if (curr_token.lex_code != search_key_tab("BEGIN"))
		cerr << "'BEGIN' KEY_WORD EXPECTED\n";
	else {
		push_to_Tree(node_ntBLOCK, curr_token, id_identifier);
		curr_token = get_next_token();
		nt_STATEMENT_LST(node_ntBLOCK, curr_token);

		if (curr_token.lex_code != search_key_tab("END"))
			cerr << "'END' KEY_WORD EXPECTED\n";
		else {
			push_to_Tree(node_ntBLOCK, curr_token, id_identifier);
			curr_token = get_next_token();
		}
	}
}

//finished one (1)
void nt_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROGRAM = push_to_Tree(Tree, curr_token, id_program);

	if (curr_token.lex_code != search_key_tab("PROGRAM"))
		cerr << "'PROGRAM' KEY_WORD expected...";
	else {
		push_to_Tree(Tree, curr_token, id_none);
		curr_token = get_next_token();
		nt_PROC_IDN(node_ntPROGRAM, curr_token);

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else {
			push_to_Tree(Tree, curr_token, id_none);
			curr_token = get_next_token();
			nt_BLOCK(node_ntPROGRAM, curr_token);
		}

		if (curr_token.lex_code != (uint)';')
			cerr << "';' EXPECTED\n";
		else
			push_to_Tree(node_ntPROGRAM, curr_token, id_none);
	}
}

//finished one (0)
void nt_SIG_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Tree = new Node;
	Tree->nonTerminalCode = id_signal_program;
	curr_token = get_next_token();
	nt_PROGRAM(Tree, curr_token);
}







