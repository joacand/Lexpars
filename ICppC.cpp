// ICppC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "lexer.h"
#include <string>
#include "parser.h"

using namespace std;

int main() {

	parse();

	/*
	Token* tok;

	while (tok = getToken()) {
		if (tok->kind == -1) {
			cout << "LEXER ERROR - KIND RETURNED -1 - EXITING" << endl << endl;
			system("pause");
			return -1;
		}
		cout << " " << tok->kind << " : " << tok->name << " : " << tok->value << endl;
	}
	*/
	system("pause");
    return 0;
}

