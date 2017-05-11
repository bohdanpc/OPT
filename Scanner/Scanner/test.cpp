#include "scanner.h"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
	const char *tokens_txt = "Tokens.txt";
	const char *tables_txt = "Tables.txt";
	const char *err_txt = "Err_trace.txt";
	ofstream out_tokens(tokens_txt);
	ofstream out_tables(tables_txt);
	ofstream err_trace(err_txt);
	ifstream fin("Input_code.sig");

	try {
		cout << "Start lexical analyzer...\n";
		scanner(fin, err_trace);
		print_tokens_string(out_tokens);
		cout << "Tokens string is written in \'" << tokens_txt << "\'" << endl;

		print_key_tab(out_tables);
		print_idn_tab(out_tables);
		cout << "Tables is written in \'" << tables_txt << "\'" << endl;
	}
	catch (exception &excp) {
		cerr << excp.what();
	}

	return 0;
}