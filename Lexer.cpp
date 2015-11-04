
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lexer.h"

int lastChar = ' ';

Token* getToken() {
	Token *tok = new Token();

	while (isspace(lastChar)) {
		lastChar = std::getchar();
	}

	if (isdigit(lastChar)) {
		std::string alpha;
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
		std::string alpha;
		while ((isalnum(lastChar) || lastChar == '_') && lastChar != ' ') {
			alpha += lastChar;
			lastChar = std::getchar();
		}
		//std::cout << alpha; // Debug code

		// If it's a reserved word / command
		if (alpha == "return" || alpha == "while" || alpha == "if" || alpha == "else" || alpha == "for" || alpha == "do") {
			tok->kind = Token::tok_command;
			tok->value = alpha;
			tok->name = alpha;
			return tok;
		}

		// Else it's an identifier
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
		if (lastChar == '/') {
			// One line comment
			do {
				lastChar = getchar();
			} while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

			if (lastChar != EOF)
				return getToken();
		}
		else if (lastChar == '*') {
			// Multi line comment
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
	}

	// ++ and -- expression
	if ((c == '+' && lastChar == '+') || (c == '-' && lastChar == '-')) {
		std::string s(1, c);
		s += (1, lastChar);
		tok->kind = Token::tok_unisymbol; tok->name = "UniSymbol"; tok->value = s;
		lastChar = std::getchar();
		return tok;
	}

	// Different kinds of assignment operators
	if ((c == '+' && lastChar == '=') || (c == '-' && lastChar == '=') || (c == '*' && lastChar == '=') || (c == '/' && lastChar == '=')) {
		std::string s(1, c);
		s += (1, lastChar);
		tok->kind = Token::tok_binsymbol; tok->name = "BinSymbol"; tok->value = s;
		lastChar = std::getchar();
		return tok;
	}

	// Binary operators
	if (c == '=' || c == '*' || c == '-' || c == '/' || c == '+') {
		tok->kind = Token::tok_binsymbol;	tok->name = "BinSymbol"; tok->value = c;
		return tok;
	}

	// Delimiters
	if (c == '(' || c == ')' || c == '{' || c == '}') {
		tok->kind = Token::tok_delim;	tok->name = "Delim"; tok->value = c;
		return tok;
	}

	// End of statement
	if (c == ';') {
		tok->kind = Token::tok_eos; tok->name = "Delim"; tok->value = c;
		return tok;
	}

	// Else, return lexer error
	tok->kind = Token::tok_error;
	return tok;
}
