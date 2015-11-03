
#include "stdafx.h"
#include <iostream>
#include <string>
#include "lexer.h"
#include <sstream>
#include <vector>

using namespace std;

class ExprASTNode;

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

	TODO:
	SDecl.      Stmt ::= Type [Item] ";" ;
	SNoInit.    Item ::= Ident ;
	SInit.      Item ::= Ident "=" Expr ;
	SRet.       Stmt ::= "return" Expr ";" ;
	SVRet.      Stmt ::= "return" ";" ;
	SCond.      Stmt ::= "if" "(" Expr ")" Stmt  ;
	SCondElse.  Stmt ::= "if" "(" Expr ")" Stmt "else" Stmt  ;
	SWhile.     Stmt ::= "while" "(" Expr ")" Stmt ;
*/
/* Expressions:
	Var
	Lit Int
	*
	+
	/
	-
	TODO:
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
		if (left != NULL || right != NULL) {
			tabs++;
		}
		if (left != NULL) {
			ls = left->print();
		}
		if (right != NULL) {
			rs = right->print();
		}
		/*
		s << "(" << ls;
		s << " | " << "[Val: " << val << " Kind: " << kind << " StrVal: " << strVal << "]";
		s << " | " << rs << ")";
		*/
		//tabs++;
		s << std::string(t, ' ') << "[Val: " << val << " Kind: " << kind << " StrVal: " << strVal << "]" << endl;
		if (ls != "-")
			s << std::string(t, ' ') << ls ;
		if (rs != "-")
			s << std::string(t, ' ') << rs;

		tabs = t;
		return s.str();
	}
};

// Statement class
class StmASTNode {
public:
	StmASTNode() {

	}

	~StmASTNode() {
	}

	virtual void print() {

	}
};

typedef vector<StmASTNode*> stm_vec;

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

Token* tok;

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
	string assOp; // =, +=, -=, *= etc
	ExprASTNode* lhs;
	ExprASTNode* rhs;

	void print() {
		//tabs++;
		int t = tabs;
		//tabs++;
		string ls = "-";
		string rs = "-";
		if (lhs != NULL) {
			ls = lhs->print();
		}
		if (rhs != NULL) {
			rs = rhs->print();
		}
		cout << string(t, ' ') << "[StmAssignment, operator: " << assOp << "]" << endl;
		if (ls != "-") 
			cout << string(t, ' ') << ls;
		if (rs != "-")
			cout << string(t, ' ') << rs;
	}
};

class StmReturn : public StmASTNode {
public:
	ExprASTNode* retExpr;

	void print() {
		int t = tabs;
		string e = "-";
		if (retExpr != NULL) {
			e = retExpr->print();
		}
		cout << string(t, ' ') << "[StmReturn]" << endl;
		if (e != "-") {
			cout << string(t, ' ') << e;
		}
	}
};

class StmExpr : public StmASTNode {
public:
	ExprASTNode* expr;

	void print() {
		//tabs++;
		int t = tabs;
		//tabs++;
		string ls = "-";
		if (expr != NULL) {
			ls = expr->print();
		}
		//cout << string(t, ' ') << assOp << endl;
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
	//bool error = false;
	Program* prgm = new Program();

	//while (!error && (tok = getToken())) {
		//tok = getToken();
	while (true) {
		stm_vec stms = prgm->run();
		//StmASTNode* ast = parseStmt();
		for (auto stm : stms) {
			stm->print();
		}
		cout << endl << "Successful" << endl;
		tabs = 1;

	}
//}
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
}

StmASTNode* parseCommand() {
	string cmd = tok->value;
	int k = tok->kind;
	tok = getToken();

	if (cmd == "return") {
		StmReturn* stm = new StmReturn();
		if (tok->kind == Token::tok_eos) {
			return stm;
		} else {
			ExprASTNode* e = parsePrimaryExpr();
			stm->retExpr = e;
			return stm;
		}
	}
}

StmASTNode* parseDelimStmt() {
	//StmAssignment* stm = new StmAssignment();
	//return stm;
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

	if (tok->value == "(") {

	}
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
	} else {
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
			stm->assOp = tok->value;
			stm->lhs = e1;
			tok = getToken();
			stm->rhs = parsePrimaryExpr();
			return stm;
		}

		
		cout << "ERROR IN parseIdentStmt, no pattern match" << endl;
	}
}
/*
StmASTNode* parsePrimaryStm() {
	if (tok->kind == Token::tok_identifier) {
		ExprASTNode* node = parseIdentExpr();
	}
	if (tok->kind == Token::tok_number) {
		//return parseIdentExpr();
	}
	if (tok->kind == Token::tok_delim) {
		if (tok->value == "(") {
			//return parseIdentExpr();
		}
	}
}
*/
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
	cout << "ERROR IN parseIdentExpr" << endl;
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

	cout << "ERROR IN parseNumExpr" << endl;
}

