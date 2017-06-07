#include "parser.h"
#include <deque>

uint program_name;
static vector<string> ErrorLog;
static vector<uint> gIDNs;
ofstream fout;

#define ntPROCEDURE_NAME_IDX		 1
#define ntPROGRAM_NAME_IDX			 1
#define tBEGIN_IDX					 1
#define tEND_IDX					 3
#define ntSTATEMENT_LST_IDX			 2
#define ntBLOCK_IDX					 3
#define ntPROGRAM_IDX				 0
#define ntDECLARATIONS_IDX			 0
#define ntPROC_DECLARATIONS_IDX		 0
#define ntPARAMETERS_LST_IDX		 2
#define ntDECLARATIONS_LST_IDX		 1
#define ntVARIABLE_IDN_IDX			 0
#define ntIDN_IDX				 	 0
#define tIDENTIFIER_IDX				 0
#define ntIDENTIFIER_LST			 1
#define ntATTRIBUTE_IDX				 3
#define ntATTRIBUTE_LST_IDX			 4
#define tATTRIBUTE_IDX				 0
#define ntEMPTY_IDX                  0

#define LEN_RETURN_ADDRESS			 4


void write_error(const string error) {
	ErrorLog.push_back(error);
}


bool search_glob_idn_tab(uint lex_code) {
	for (auto it = gIDNs.begin(); it != gIDNs.end(); ++it) 
		if (*it == lex_code)
			return true;
	return false;
}

void gen_idn(Node *&Tree, uint &args_count, deque<uint> &args) {
	if (Tree->nonTerminalCode == id_empty)
		return;
	if (Tree->nonTerminalCode == id_none)	//in case of ',' go to next token
		return;
	args_count++;

	if (std::find(args.begin(), args.end(), Tree->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.lex_code) != args.end()) {
		fout << "!!!Line " << Tree->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.line_num << ": ";
		fout << search_tabs(Tree->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.lex_code) << " is already defined...\n";
	}
	else
		args.push_back(Tree->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.lex_code);
}

void gen_idn_list(Node *&Tree, uint &args_count, deque<uint> &args) {
	for (auto it = Tree->children.begin(); it != Tree->children.end(); ++it) {
		gen_idn(*it, args_count, args);
	}
}

void gen_attrib(Node *&Tree, uint &type_count, deque<uint> &types) {
	if (Tree->nonTerminalCode == id_empty)
		return;
	if (search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "SIGNAL" ||
		search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "COMPLEX" ||
		search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "EXT")
		return;
	type_count++;

	if (search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "INTEGER")
		types.push_back(4);
	else
		if (search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "FLOAT")
			types.push_back(8);
		else
			if (search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) == "BLOCKFLOAT")
				types.push_back(16);
			else
				fout << "!!!Line " << Tree->children[tATTRIBUTE_IDX]->Token.line_num << ": unknown type '"
				<< search_tabs(Tree->children[tATTRIBUTE_IDX]->Token.lex_code) << "' " << endl;

}

void gen_attrib_list(Node *&Tree, uint &type_count, deque<uint> &types) {
	for (auto it = Tree->children.begin(); it != Tree->children.end(); ++it) {
		gen_attrib(*it, type_count, types);
	}

}

void gen_declaration(Node *&Tree) {
	if (Tree->nonTerminalCode == id_empty)
		return;
	deque<uint> args;
	deque<uint> types;
	uint types_count = 0, args_count = 0;

	gen_idn(Tree->children[ntVARIABLE_IDN_IDX], args_count, args);
	gen_idn_list(Tree->children[ntIDENTIFIER_LST], args_count, args);

	gen_attrib(Tree->children[ntATTRIBUTE_IDX], types_count, types);
	gen_attrib_list(Tree->children[ntATTRIBUTE_LST_IDX], types_count, types);

	if (args_count != types_count)
		fout << "!!!Line " << Tree->children[ntVARIABLE_IDN_IDX]->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.line_num
		<< ": " << "count of args and theirs corresponding types differ\n";
	else
	{
		uint curr_arg, curr_type;
		uint curr_shift = LEN_RETURN_ADDRESS;
		while (args.size()) {
			curr_arg = args.back(); 
			args.pop_back();
			
			curr_type = types.back(); 
			types.pop_back();
			
			fout << "\t" << search_tabs(curr_arg) << " equ [ESP + " << curr_shift << "]\n";
			curr_shift += curr_type;
		}
		curr_shift -= LEN_RETURN_ADDRESS;
		fout << "\tret " << curr_shift << endl;
	}
}


void gen_declarations_lst(Node *&Tree) {
	for (auto it = Tree->children.begin(); it != Tree->children.end(); ++it)
		gen_declaration(*it);
}


void gen_parameters_lst(Node *&Tree) {
	if (Tree->children[ntEMPTY_IDX]->nonTerminalCode != id_empty)
		gen_declarations_lst(Tree->children[ntDECLARATIONS_LST_IDX]);
	else
		fout << "\tret\n";
}


void gen_procedure(Node *&Tree) {
	if (Tree->nonTerminalCode == id_empty)
		return;

	uint procedure_name = Tree->children[ntPROCEDURE_NAME_IDX]->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.lex_code;
	if (search_glob_idn_tab(procedure_name)) {
		fout << "!!!Line " << Tree->children[ntPROCEDURE_NAME_IDX]->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.line_num << ": '"
			 << search_tabs(procedure_name) << "'name is already defined\n";
	}
	else {
		gIDNs.push_back(procedure_name);
		fout << "\n@" << search_tabs(procedure_name) << ":" << endl;
		gen_parameters_lst(Tree->children[ntPARAMETERS_LST_IDX]);
	}
}


void gen_proc_declarations(Node *&Tree) {
	for (auto it_child = Tree->children.begin(); it_child != Tree->children.end(); it_child++) 
		gen_procedure(*it_child);
}

void gen_declarations(Node *&Tree) {
	if (Tree->nonTerminalCode == id_declarations)
		gen_proc_declarations(Tree->children[ntPROC_DECLARATIONS_IDX]);
}



void gen_block(Node *&Tree) {
	if (Tree->nonTerminalCode == id_block) {
		fout << "CODE SEGMENT\n";
		gen_declarations(Tree->children[ntDECLARATIONS_IDX]);
		if (Tree->children[tBEGIN_IDX]->Token.lex_code == search_key_tab("BEGIN"))
			fout << "\n@" << search_tabs(program_name) << ":" << endl;
		if (Tree->children[ntSTATEMENT_LST_IDX]->nonTerminalCode == id_statement_lst)
			fout << "\tnop\n";
		if (Tree->children[tEND_IDX]->Token.lex_code == search_key_tab("END"))
			fout << "@" << search_tabs(program_name) << " ENDS" << endl;
		fout << "CODE ENDS\n";
	}
}


void gen_program(Node *&Tree) {	

	if (Tree->nonTerminalCode == id_program) {
		program_name = Tree->children[ntPROGRAM_NAME_IDX]->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.lex_code;
		if (search_glob_idn_tab(program_name)) {
			fout << "!!!Line " << Tree->children[ntPROGRAM_NAME_IDX]->children[ntIDN_IDX]->children[tIDENTIFIER_IDX]->Token.line_num << ": ";
			fout << search_tabs(program_name) << " is already defined\n";
		}
		else
			gIDNs.push_back(program_name);
	}
	else
		fout << "Unknown error occured\n";
	gen_block(Tree->children[ntBLOCK_IDX]);
}


void gen_sig_prog(Node *&Tree) {
	if (Tree->nonTerminalCode == id_signal_program)
		gen_program(Tree->children[ntPROGRAM_IDX]);
	else
		fout << "Unknown error occured\n";
}

void code_generator() {
	Node *Tree = NULL;

	string foutName = "Generated_code.txt";

	fout.open(foutName);
	try {
		parser(Tree);
		gen_sig_prog(Tree);

		cout << "\nCode generator is finished\n";
		cout << "Generated code: '" << foutName << "'" << endl;
		free_Tree(Tree);
	}
	catch (exception &excp) {
		cout << excp.what() << endl;
	}
}