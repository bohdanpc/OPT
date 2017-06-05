#include "codeGenerator.h"

int main() {
	try {
		code_generator();
	}
	catch (exception &excp) {
		cerr << excp.what();
	}

	return 0;
}