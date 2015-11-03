
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lexer.h"

using namespace std;

int lastChar = ' ';

Token* getToken() {
	Token *tok = new Token();

	while (isspace(lastChar)) {
		lastChar = std::getchar();
	}

	if (isdigit(lastChar)) {
		string alpha;
		bool seperator = false;
		while (isdigit(lastChar) || (lastChar == '.' && !seperator)) {
			if (lastChar == '.')
				seperator = true;
			alpha += lastChar;
			lastChar = std::getchar();
		}

		tok->kind = Token::tok_number;
		tok->value = alpha;
		tok->name = "Lit";
		seperator = false;
		return tok;
	}

	if (isalpha(lastChar)) {
		string alpha;
		while ((isalnum(lastChar) || lastChar == '_') && lastChar != ' ') {
			alpha += lastChar;
			lastChar = std::getchar();
		}
		//std::cout << alpha; // Debug code
		// Check if alpha is equal to "int", etc

		if (alpha == "return") {
			tok->kind = Token::tok_command;
			tok->value = alpha;
			tok->name = "Return";
			return tok;
		}

		if (alpha == "while") {
			tok->kind = Token::tok_command;
			tok->value = alpha;
			tok->name = "While";
			return tok;
		}

		if (alpha == "if") {
			tok->kind = Token::tok_command;
			tok->value = alpha;
			tok->name = "If";
			return tok;
		}

		if (alpha == "else") {
			tok->kind = Token::tok_command;
			tok->value = alpha;
			tok->name = "Else";
			return tok;
		}

		tok->kind = Token::tok_identifier;
		tok->value = alpha;
		tok->name = "Ident";
		return tok;
	}



	// End of file
	if (lastChar == EOF) {
		tok->kind = Token::tok_eof;
		return tok;
	}

	int c = lastChar;
	lastChar = std::getchar();

	// Comments
	if (c == '/') {
		//lastChar = std::getchar();
		if (lastChar == '/') {
			// Comment
			do {
				lastChar = getchar();
			} while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

			if (lastChar != EOF)
				return getToken();
		}
		else if (lastChar == '*') {
			// Multiline comment
			char last;
			char tmp = lastChar;
			do {
				last = tmp;
				lastChar = getchar();
				tmp = lastChar;
			} while (lastChar != EOF && !(last == '*' && lastChar == '/'));
			lastChar = getchar();
			if (lastChar != EOF)
				return getToken();
		}
		//if (lastChar != EOF)
		//	tok->kind = Token::tok_error; return tok;
	}

	if ((c == '+' && lastChar=='+') || (c == '-' && lastChar == '-')) {
			// ++
			string s(1, c);
			s += (1, lastChar);
			tok->kind = Token::tok_unisymbol; tok->name = "UniSymbol"; tok->value = s;
			lastChar = std::getchar();
			return tok;
	}

	if ((c == '+' && lastChar == '=') || (c == '-' && lastChar == '=') || (c == '*' && lastChar == '=') || (c == '/' && lastChar == '=')) {
		// Different kind of assignment operators
		string s(1, c);
		s += (1, lastChar);
		tok->kind = Token::tok_binsymbol; tok->name = "BinSymbol"; tok->value = s;
		lastChar = std::getchar();
		return tok;
	}
	if (c == '=' || c == '*' || c == '-' || c == '/' || c == '+') {
		tok->kind = Token::tok_binsymbol;	tok->name = "BinSymbol" ; tok->value = c; return tok;
	}
	if (c == '(' || c == ')' || c == '{' || c == '}') {
		tok->kind = Token::tok_delim;	tok->name = "Delim"; tok->value = c; return tok;
	}
	if (c == ';') {
		tok->kind = Token::tok_eos; tok->name = "Delim"; tok->value = c; return tok;
}
	tok->kind = Token::tok_error;
	return tok;
}
