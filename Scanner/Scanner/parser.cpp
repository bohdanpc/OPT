#include "parser.h"

static map<uint, string> NonTerminals;
static map<uint, uint> ErrorLog;

enum errorCode{
	err_tPROGRAM = 1,
	err_tBEGIN,
	err_tEND,
	err_dmSEMICOLON,
	err_dmCOLON,
	err_dmRIGHT_BRACKET,
	err_NO_MORE_TOKENS
};

const char* getErrorMsg(uint code) {
	switch (code) {
	case err_tPROGRAM:
		return "'PROGRAM' KEY_WORD expected...\n";
	case err_tBEGIN:
		return "'BEGIN' keyword expected...\n";
	case err_tEND:
		return "'END' keyword expected...\n";
	case err_dmSEMICOLON:
		return "';' expected...\n";
	case err_dmCOLON:
		return "':' expected...\n";
	case err_dmRIGHT_BRACKET:
		return "')' expected...\n";
	case err_NO_MORE_TOKENS:
		return "No more tokens left to parse...\n";
	default:
		return "UNKNOWN error";
	}
}

bool errorsProcessing(ofstream &fout) {
	if (ErrorLog.empty())
		return false;

	for (auto it = ErrorLog.begin(); it != ErrorLog.end(); it++) 
		fout << "Line " << it->second << ": " << getErrorMsg(it->first);
	return true;
}

bool search_idn_tab(uint lex_code) {
	if (lex_code > IDENTIFIER_IDX_START)
		return true;
	return false;
}

Node* push_to_Tree(Node *&tree, lex_string &token, nonterminalCode ntCode) {
	Node *node = new Node;
	if (ntCode == id_none)
		node->Token = token;
	node->nonTerminalCode = ntCode;

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
			out << string(tab, ' ') << NonTerminals.at(Tree->nonTerminalCode) << endl;
		else
			out << string(tab, ' ') << search_tabs(Tree->Token.lex_code) << endl;

		for (auto it = Tree->children.begin(); it != Tree->children.end(); it++)
			print_Tree(*it, out, tab + 4);
	}
}


void free_Tree(Node *Tree) {
		if (!Tree->children.empty())
			for (auto it = Tree->children.begin(); it != Tree->children.end(); it++)
				free_Tree(*it);
		delete Tree;
}


lex_string get_next_token() {
	static uint curr_lex_code = 0;
	lex_string res;
	try {
		res = Tokens.at(curr_lex_code);
		curr_lex_code++;
	}
	catch (exception &) {
		res = Tokens.back();
		res.lex_code = NO_LEXEM;
	}
		return res;
}

void nt_IDN(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_IDENTIFIER = push_to_Tree(Tree, curr_token, id_identifier);
	push_to_Tree(node_nt_IDENTIFIER, curr_token, id_none);
	curr_token = get_next_token();
}


void nt_PROC_IDN(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROC_IDN = push_to_Tree(Tree, curr_token, id_procedure_identifier);
	nt_IDN(node_ntPROC_IDN, curr_token);
}


void nt_VARIABLE_ID(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_VARIABLE_ID = push_to_Tree(Tree, curr_token, id_variable_identifier);
	nt_IDN(node_nt_VARIABLE_ID, curr_token);
}


void nt_IDENTIFIERS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_IDENTIFIERS_LST = push_to_Tree(Tree, curr_token, id_identifiers_lst);
	while (curr_token.lex_code == (uint)',') {
		push_to_Tree(node_nt_IDENTIFIERS_LST, curr_token, id_none);
		curr_token = get_next_token();
		nt_VARIABLE_ID(node_nt_IDENTIFIERS_LST, curr_token);
	}
	push_to_Tree(node_nt_IDENTIFIERS_LST, curr_token, id_empty);
}


bool nt_ATTRIBUTE(Node *&Tree, lex_string &curr_token) {
	if (curr_token.lex_code != idx_signal &&
		curr_token.lex_code != idx_complex &&
		curr_token.lex_code != idx_integer &&
		curr_token.lex_code != idx_float &&
		curr_token.lex_code != idx_blockfloat &&
		curr_token.lex_code != idx_ext)
		return false;

	Node *node_nt_ATTRIBUTE = push_to_Tree(Tree, curr_token, id_attribute);
	push_to_Tree(node_nt_ATTRIBUTE, curr_token, id_none);
	curr_token = get_next_token();
	return true;
}


void nt_ATTRIBUTE_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_nt_ATTRIBUTE_LST = push_to_Tree(Tree, curr_token, id_attributes_lst);
	while (nt_ATTRIBUTE(node_nt_ATTRIBUTE_LST, curr_token));

	push_to_Tree(node_nt_ATTRIBUTE_LST, curr_token, id_empty);
}


bool nt_DECLARATION(Node *&Tree, lex_string &curr_token) {
	if (!search_idn_tab(curr_token.lex_code))
		return false;

	Node *node_nt_DECLARATION = push_to_Tree(Tree, curr_token, id_declaration);
	nt_VARIABLE_ID(node_nt_DECLARATION, curr_token);
	nt_IDENTIFIERS_LST(node_nt_DECLARATION, curr_token);

	if (curr_token.lex_code != (uint)':')
		ErrorLog.insert(pair<uint, uint>(err_dmCOLON, curr_token.line_num));
	else {
		push_to_Tree(node_nt_DECLARATION, curr_token, id_none);
		curr_token = get_next_token();
		nt_ATTRIBUTE(node_nt_DECLARATION, curr_token);
		nt_ATTRIBUTE_LST(node_nt_DECLARATION, curr_token);

		if (curr_token.lex_code != (uint)';')
			ErrorLog.insert(pair<uint, uint>(err_dmSEMICOLON, curr_token.line_num));
		else {
			push_to_Tree(node_nt_DECLARATION, curr_token, id_none);
			curr_token = get_next_token();
		}
	}
	return true;
}


void nt_DECLARATIONS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_ntDECLARATIONS_LST = push_to_Tree(Tree, curr_token, id_declarations_lst);
	while (nt_DECLARATION(node_ntDECLARATIONS_LST, curr_token));

	push_to_Tree(node_ntDECLARATIONS_LST, curr_token, id_empty);
}


void nt_PARAMS_LST(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPARAMS_LST = push_to_Tree(Tree, curr_token, id_params_lst);

	if (curr_token.lex_code != (uint)'(')
		push_to_Tree(node_ntPARAMS_LST, curr_token, id_empty);
	else {
		push_to_Tree(node_ntPARAMS_LST, curr_token, id_none);
		curr_token = get_next_token();
		nt_DECLARATIONS_LST(node_ntPARAMS_LST, curr_token);

		if (curr_token.lex_code != (uint)')')
			ErrorLog.insert(pair<uint, uint>(err_dmRIGHT_BRACKET, curr_token.line_num));
		else {
			push_to_Tree(node_ntPARAMS_LST, curr_token, id_none);
			curr_token = get_next_token();
		}
	}
}


bool nt_PROCEDURE(Node *&Tree, lex_string &curr_token) {
	if (curr_token.lex_code != idx_procedure)
		return false;

	Node *node_ntPROCEDURE = push_to_Tree(Tree, curr_token, id_procedure);
	push_to_Tree(node_ntPROCEDURE, curr_token, id_none);
	curr_token = get_next_token();
	nt_PROC_IDN(node_ntPROCEDURE, curr_token);
	nt_PARAMS_LST(node_ntPROCEDURE, curr_token);

	if (curr_token.lex_code != (uint)';')
		ErrorLog.insert(pair<uint, uint>(err_dmSEMICOLON, curr_token.line_num));
	else {
		push_to_Tree(node_ntPROCEDURE, curr_token, id_none);
		curr_token = get_next_token();
	}
	return true;
}


void nt_PROC_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	Node *nt_proc_declarations = push_to_Tree(Tree, curr_token, id_procedure_declarations);
	while (nt_PROCEDURE(nt_proc_declarations, curr_token));

	push_to_Tree(nt_proc_declarations, curr_token, id_empty);
}


void nt_DECLARATIONS(Node *&Tree, lex_string &curr_token) {
	Node *nt_declarations = push_to_Tree(Tree, curr_token, id_declarations);
	nt_PROC_DECLARATIONS(nt_declarations, curr_token);
}


void nt_STATEMENT_LST(Node *&Tree, lex_string &curr_token) {
	Node *nt_STATEMENT_LST = push_to_Tree(Tree, curr_token, id_statement_lst);
	push_to_Tree(nt_STATEMENT_LST, curr_token, id_empty);
}


void nt_BLOCK(Node *&Tree, lex_string &curr_token) {
	Node *node_ntBLOCK = push_to_Tree(Tree, curr_token, id_block);
	nt_DECLARATIONS(node_ntBLOCK, curr_token);

	if (curr_token.lex_code != idx_begin)
		ErrorLog.insert(pair<uint, uint>(err_tBEGIN, curr_token.line_num));
	else {
		push_to_Tree(node_ntBLOCK, curr_token, id_none);
		curr_token = get_next_token();
		nt_STATEMENT_LST(node_ntBLOCK, curr_token);

		if (curr_token.lex_code != idx_end)
			ErrorLog.insert(pair<uint, uint>(err_tEND, curr_token.line_num));
		else {
			push_to_Tree(node_ntBLOCK, curr_token, id_none);
			curr_token = get_next_token();
		}
	}
}


void nt_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Node *node_ntPROGRAM = push_to_Tree(Tree, curr_token, id_program);

	if (curr_token.lex_code != idx_program)
		ErrorLog.insert(pair<uint, uint>(err_tPROGRAM, curr_token.line_num));
	else {
		push_to_Tree(node_ntPROGRAM, curr_token, id_none);
		curr_token = get_next_token();
		nt_PROC_IDN(node_ntPROGRAM, curr_token);

		if (curr_token.lex_code != (uint)';')
			ErrorLog.insert(pair<uint, uint>(err_dmSEMICOLON, curr_token.line_num));
		else {
			push_to_Tree(node_ntPROGRAM, curr_token, id_none);
			curr_token = get_next_token();
			nt_BLOCK(node_ntPROGRAM, curr_token);
		}

		if (curr_token.lex_code != (uint)';')
			ErrorLog.insert(pair<uint, uint>(err_dmSEMICOLON, curr_token.line_num));
		else 
			push_to_Tree(node_ntPROGRAM, curr_token, id_none);
	}
}


void nt_SIG_PROGRAM(Node *&Tree, lex_string &curr_token) {
	Tree = new Node;
	Tree->nonTerminalCode = id_signal_program;
	curr_token = get_next_token();
	nt_PROGRAM(Tree, curr_token);
}


void parser(Node *&Tree) {
	try {
		const char *tokens_txt = "Tokens.txt";
		const char *tables_txt = "Tables.txt";
		const char *err_txt_scanner = "Err_trace_scanner.txt";
		const char *err_txt_parser = "Err_trace_parser.txt";
		const char *tree_txt = "Tree_txt.txt";

		ofstream out_tokens(tokens_txt);
		ofstream out_tables(tables_txt);
		ofstream out_tree(tree_txt);
		ofstream err_trace_scanner(err_txt_scanner);
		ofstream err_trace_parser(err_txt_parser);

#ifdef __DEBUG__
		ifstream fin("Input_code8.sig");
#else
		string input_code;
		cout << "Enter file_name:";
		cin >> input_code;
		ifstream fin(input_code);
#endif
		scanner(fin, err_trace_scanner);
		print_tokens_string(out_tokens);
		print_key_tab(out_tables);
		print_idn_tab(out_tables);

		lex_string curr_token;
		initNonTerminals();
		try {
			nt_SIG_PROGRAM(Tree, curr_token);
		}
		catch (out_of_range &) {
			ErrorLog.insert(pair<uint, uint>(err_NO_MORE_TOKENS, curr_token.line_num));
		}
		print_Tree(Tree, out_tree);

		cout << "The tree is formed in '" << tree_txt << "'\n";
		if (errorsProcessing(err_trace_parser))
			cout << "Some errors occured...\n";
		else
			cout << "No errors occured...\n";
	}
	catch (exception &excp) {
		cerr << excp.what();
	}
}

