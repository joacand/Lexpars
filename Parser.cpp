
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lexer.h"
#include <sstream>
#include <vector>

using namespace std;

class ExprASTNode;
class StmASTNode;

StmASTNode* parseStmt();
StmASTNode* parseDelimStmt();
StmASTNode* parseCommand();
StmASTNode* parseIdentStmt();
StmASTNode* parseNumStmt();
ExprASTNode* parsePrimaryExpr();
ExprASTNode* parseIdentExpr();
ExprASTNode* parseNumExpr();
ExprASTNode* parseBinarySymbol(ExprASTNode* lft, string op);
ExprASTNode* parseUnarySymbol(ExprASTNode* lft, string op);

typedef vector<StmASTNode*> stm_vec;
Token* tok;

int tabs = 1;

/* Statements:
	Assignment (Ident = Expr;)
	SExpression (Extr;)
	SEmpty.     Stmt ::= ";" ;
	SAss.       Stmt ::= Ident "=" Expr  ";" ;
	SBlock.     Block ::= "{" [Stmt] "}" ;
	SBStmt.     Stmt ::= Block ;
	SIncr.      Stmt ::= Ident "++"  ";" ;
	SDecr.      Stmt ::= Ident "--"  ";" ;
	SRet.       Stmt ::= "return" Expr ";" ;
	SVRet.      Stmt ::= "return" ";" ;
	SWhile.     Stmt ::= "while" "(" Expr ")" Stmt ;
	SCond.      Stmt ::= "if" "(" Expr ")" Stmt  ;

	TODO:
	SDecl.      Stmt ::= Type [Item] ";" ;
	SNoInit.    Item ::= Ident ;
	SInit.      Item ::= Ident "=" Expr ;

 Expressions TODO:
	true
	false
	string?
	-(neg)
	!(not)
	&&
	||
	>=
	<=
	>
	<
	!=
	==
	%
*/

// Expression class
class ExprASTNode {
public:
	int kind;
	double val;
	string strVal;
	ExprASTNode* left;
	ExprASTNode* right;

	ExprASTNode() {
		kind = -1;
		val = 0;
		strVal = "";
	}

	~ExprASTNode() {
		delete left;
		delete right;
	}

	string print() {
		int t = tabs;
		stringstream s;
		string ls = "-";
		string rs = "-";

		if (left != NULL || right != NULL)
			tabs++;
		if (left != NULL)
			ls = left->print();
		if (right != NULL)
			rs = right->print();


		s << std::string(t, ' ') << "[Val: " << val << " Kind: " << kind << " StrVal: " << strVal << "]" << endl;
		if (ls != "-")
			s << std::string(t, ' ') << ls;
		if (rs != "-")
			s << std::string(t, ' ') << rs;

		tabs = t;
		return s.str();
	}
};

// Base statement class
class StmASTNode {
public:
	StmASTNode() {

	}

	~StmASTNode() {
	}

	virtual void print() {

	}
};


class StmBlock : public StmASTNode {
public:
	stm_vec stms;

	void addStm(StmASTNode* stm) {
		stms.push_back(stm);
	}

	void print() {
		int t = tabs;

		cout << string(t, ' ') << "[BlockStm]" << endl;
		tabs++;
		for (auto stm : stms) {
			stm->print();
		}
		tabs = t;
	}
};

class StmAssignment : public StmASTNode {
public:
	string assignOp;
	ExprASTNode* lhs;
	ExprASTNode* rhs;

	StmAssignment() {
	}

	~StmAssignment() {
		delete lhs;
		delete rhs;
	}

	void print() {
		int t = tabs;
		string ls = "-";
		string rs = "-";
		if (lhs != NULL)
			ls = lhs->print();
		if (rhs != NULL)
			rs = rhs->print();

		cout << string(t, ' ') << "[StmAssignment, operator: " << assignOp << "]" << endl;
		if (ls != "-")
			cout << string(t, ' ') << ls;
		if (rs != "-")
			cout << string(t, ' ') << rs;
	}
};

class StmReturn : public StmASTNode {
public:
	ExprASTNode* retExpr;

	StmReturn() {
	}

	~StmReturn() {
		delete retExpr;
	}

	void print() {
		int t = tabs;
		string e = "-";
		if (retExpr != NULL)
			e = retExpr->print();

		cout << string(t, ' ') << "[StmReturn]" << endl;

		if (e != "-")
			cout << string(t, ' ') << e;
	}
};

class StmWhile : public StmASTNode {
public:
	string stmType = "[StmWhile]";
	ExprASTNode* expr;
	StmASTNode* stmt;

	StmWhile() {
	}

	StmWhile(ExprASTNode* e, StmASTNode* s) {
		expr = e;
		stmt = s;
	}

	~StmWhile() {
		delete expr;
		delete stmt;
	}

	void print() {
		int t = tabs;
		string es = expr->print();
		tabs++;


		cout << string(t, ' ') << stmType << endl;
		cout << string(t, ' ') << es;
		stmt->print();
		tabs = t;
	}
};

class StmIfElseCond : public StmASTNode {
public:
	string stmType;
	ExprASTNode* expr;
	StmASTNode* stmt;
	StmASTNode* elseStmt;

	StmIfElseCond(ExprASTNode* e, StmASTNode* s, StmASTNode* es) {
		expr = e;
		stmt = s;
		elseStmt = es;
		stmType = "[IfElseCond]";
	}

	~StmIfElseCond() {
		delete expr;
		delete stmt;
	}

	void print() {
		int t = tabs;
		string es = expr->print();
		tabs++;


		cout << string(t, ' ') << stmType << endl;
		cout << string(t, ' ') << es;
		stmt->print();
		elseStmt->print();
		tabs = t;
	}
};

class StmExpr : public StmASTNode {
public:
	ExprASTNode* expr;

	StmExpr() {
	}

	~StmExpr() {
		delete expr;
	}

	void print() {
		int t = tabs;
		string ls = "-";
		if (expr != NULL)
			ls = expr->print();

		if (ls != "-")
			cout << string(t, ' ') << ls;
	}
};

class Program {
public:
	Program() {
	}

	~Program() {
	}

	stm_vec run() {
		stm_vec stms;
		bool error = false;
		while (!error && (tok = getToken())) {
			stms.push_back(parseStmt());
			error = true; // Remove later
		}

		return stms;
	}
};

void parse() {
	Program* prgm = new Program();

	while (true) {
		stm_vec stms = prgm->run();
		for (auto stm : stms) {
			stm->print();
		}
		cout << endl << "AST created successfully" << endl << endl;
		tabs = 1;
	}

	return;
}

StmASTNode* parseStmt() {
	if (tok->kind == Token::tok_eos) {
		// Empty statement
		StmAssignment* stm = new StmAssignment();
		return stm;
	}
	if (tok->kind == Token::tok_identifier) {
		return parseIdentStmt();
	}
	if (tok->kind == Token::tok_number) {
		return parseNumStmt();
	}
	if (tok->kind == Token::tok_delim) {
		if (tok->value == "}") {
			return NULL;
		}
		return parseDelimStmt();
	}
	if (tok->kind == Token::tok_command) {
		return parseCommand();
	}

	cout << "ERROR IN parseStmt, No patterns match!";
}

StmASTNode* parseCommand() {
	string cmd = tok->value;
	int k = tok->kind;
	tok = getToken();

	if (cmd == "return") {
		StmReturn* stm = new StmReturn();
		if (tok->kind == Token::tok_eos) {
			return stm;
		}
		else {
			ExprASTNode* e = parsePrimaryExpr();
			stm->retExpr = e;
			return stm;
		}
	}
	if (cmd == "while") {
		if (tok->kind == Token::tok_delim && tok->value == "(") {
			tok = getToken();
			ExprASTNode* expr = parsePrimaryExpr();

			if (tok->kind == Token::tok_delim && tok->value == ")") {
				tok = getToken();
				StmASTNode* stmt = parseStmt();

				StmWhile* sWhile = new StmWhile(expr, stmt);
				return sWhile;
			}
		}

		// Else something went wrong with while syntax
		cout << "ERROR IN parseCommand, Wrong syntax after while statement";
	}

	// If then else, If without else not yet implemented
	if (cmd == "if") {
		if (tok->kind == Token::tok_delim && tok->value == "(") {
			tok = getToken();
			ExprASTNode* expr = parsePrimaryExpr();

			if (tok->kind == Token::tok_delim && tok->value == ")") {
				tok = getToken();
				StmASTNode* stmt = parseStmt();
				
				tok = getToken();
				if (tok->kind == Token::tok_command && tok->value == "else") {
					tok = getToken();
					StmASTNode* elseStmt = parseStmt();

					StmIfElseCond* sIfElse = new StmIfElseCond(expr, stmt, elseStmt);
					return sIfElse;
				}
				else {
					cout << "ERROR IN IfElse, No else detected";
				}
			}
		}
	}

	cout << "ERROR IN parseCommand, No patterns match!";
}

StmASTNode* parseDelimStmt() {

	if (tok->value == "{") {
		StmBlock* bstm = new StmBlock();
		tok = getToken();
		while (tok->value != "}") {
			StmASTNode* s = parseStmt();
			if (s != NULL) {
				bstm->addStm(s);
			}

			tok = getToken();
		}

		return bstm;
	}

	cout << "ERROR IN parseDelimStmt, No patterns match";
}

StmASTNode* parseNumStmt() {
	string identStr = tok->value;
	int k = tok->kind;

	StmAssignment* stm = new StmAssignment();
	stm->lhs = parsePrimaryExpr();
	return stm;

	cout << "ERROR IN parseNumStmt - No patterns match";
}

StmASTNode* parseIdentStmt() {
	string identStr = tok->value;
	int k = tok->kind;
	tok = getToken();

	if (tok->kind == Token::tok_delim) {
		// Function call
		cout << "ERROR IN parseIdentStmt, Function call not yet implemented" << endl;
	}
	else {
		// Not a function call

		ExprASTNode* e1 = new ExprASTNode();
		e1->strVal = identStr;
		e1->kind = k;

		// Unary symbol operation
		if (tok->kind == Token::tok_unisymbol && (tok->value == "++" || tok->value == "--")) {
			StmExpr* stm = new StmExpr();
			stm->expr = parseUnarySymbol(e1, tok->value);
			return stm;
		}
		// Simple end of statement
		if (tok->kind == Token::tok_eos) {
			StmAssignment* stm = new StmAssignment();
			stm->lhs = e1;
			return stm;
		}

		// Binary symbol operation, so far only = += -= etc
		if (tok->kind == Token::tok_binsymbol && (tok->value == "=" || tok->value == "+=" || tok->value == "-=" || tok->value == "*=" || tok->value == "/=")) {
			// Assignment
			StmAssignment* stm = new StmAssignment();
			stm->assignOp = tok->value;
			stm->lhs = e1;
			tok = getToken();
			stm->rhs = parsePrimaryExpr();
			return stm;
		}

		cout << "ERROR IN parseIdentStmt, no pattern match" << endl;
	}
}

ExprASTNode* parsePrimaryExpr() {
	if (tok->kind == Token::tok_identifier) {
		ExprASTNode* node = parseIdentExpr();
		return node;
	}
	if (tok->kind == Token::tok_number) {
		ExprASTNode* node = parseNumExpr();
		return node;
	}

	cout << "ERROR IN parsePrimaryExpr, no pattern match" << endl;
}

ExprASTNode* parseBinarySymbol(ExprASTNode* lft, string op) {
	ExprASTNode* var = new ExprASTNode();
	var->strVal = op;
	var->kind = tok->kind;
	var->left = lft;
	tok = getToken();
	var->right = parsePrimaryExpr();
	return var;
}

ExprASTNode* parseUnarySymbol(ExprASTNode* lft, string op) {
	ExprASTNode* var = new ExprASTNode();
	var->strVal = op;
	var->kind = tok->kind;
	var->left = lft;
	tok = getToken();
	return var;
}

ExprASTNode* parseIdentExpr() {
	string identStr = tok->value;
	int k = tok->kind;

	ExprASTNode* lft = new ExprASTNode();
	lft->strVal = identStr;
	lft->kind = k;

	tok = getToken();

	if (tok->kind == Token::tok_delim) {

	}

	if (tok->kind == Token::tok_eos) {
		return lft;
	}
	if (tok->kind == Token::tok_binsymbol) {
		return parseBinarySymbol(lft, tok->value);
	}
	if (tok->kind == Token::tok_unisymbol) {
		return parseUnarySymbol(lft, tok->value);
	}

	return lft;

	cout << "ERROR IN parseIdentExpr, No patterns match" << endl;
}

ExprASTNode* parseNumExpr() {
	int value = stoi(tok->value);
	int k = tok->kind;

	ExprASTNode* lft = new ExprASTNode();
	lft->val = value;
	lft->kind = k;

	tok = getToken();

	if (tok->kind == Token::tok_eos) {
		return lft;
	}
	if (tok->kind == Token::tok_binsymbol) {
		return parseBinarySymbol(lft, tok->value);
	}
	if (tok->kind == Token::tok_unisymbol) {
		cout << "ERROR IN parseNumExpr - Unary symbols not yet implemented" << endl;
		//return parseBinarySymbol(lft, tok->value);
	}

	return lft;

	cout << "ERROR IN parseNumExpr, No patterns match" << endl;
}
