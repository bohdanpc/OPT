#include "parser.h"

int main() {
	try {
		parser();
	}
	catch (exception &excp) {
		cerr << excp.what();
	}

	return 0;
}