#ifndef LEXER_H
#define LEXER_H

class Token {
public:
	enum kinds {
		// End of file and error
		tok_error = -1,
		tok_eof = 1,

		// Commands (return, while, for, if, else)
		tok_command,

		// Operator symbols
		tok_binsymbol,
		tok_unisymbol,

		// Misc
		tok_delim,
		tok_identifier,
		tok_number,
		tok_comment,
		tok_eos
	};
	int kind;
	std::string value;
	std::string name;
};

Token* getToken();

#endif