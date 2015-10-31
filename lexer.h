#ifndef LEXER_H
#define LEXER_H

class Token {
public:
	enum kinds {
		// End of file and error
		tok_eof = 1,
		tok_error = -1,

		// Operator symbols
		tok_binsymbol = 2,
		tok_unisymbol = 3,

		// Misc
		tok_delim = 5,
		tok_identifier = 6,
		tok_number = 7,
		tok_comment = 8,
		tok_eos = 9
	};
	int kind;
	std::string value;
	std::string name;
};

Token* getToken();

#endif