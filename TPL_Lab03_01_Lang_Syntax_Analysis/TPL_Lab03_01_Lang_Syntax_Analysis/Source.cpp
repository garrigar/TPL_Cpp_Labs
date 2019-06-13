#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

enum LexType { KwDo, KwWhile, KwLoop, 
	Id, InvalidId, Number, 
	LogicNot, LogicAnd, LogicOr, CompareOp, Semicolon, Assignment, ArithOp, 
	Minus, WrongCh	/*, leftParen, rightParen*/ }; //lexeme types

// lexeme container 
struct Lexeme {
	LexType type;	// lexeme type
	char* str;		// lexeme's string
	int pos;
	int line;
};

// lexeme parser on FSM
class LexParserFSM {
private:
	enum AState { S, A, B, E, C, D, G, H, I, J, K, L, M, N,		F, R }; // automata states

	// transition table
	
	const AState table[11][14] = 
	{
	//	S	A	B	E	C	D	G	H	I	J	K	L	M	N
		S,	F,	F,	F,	F,	F,	F,	F,	F,	F,	F,	F,	F,	F, 
		A,	A,	E,	E,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		B,	A,	B,	E,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		C,	R,	R,	R,	D,	R,	I,	I,	R,	R,	R,	R,	R,	R,
		G,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		H,	R,	R,	R,	R,	R,	I,	R,	R,	R,	R,	R,	R,	R,
		J,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		K,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		L,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		M,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,
		N,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R,	R
	};

	// get table row by char
	int getRow(char c) {
		if (isspace(c) || c == '\0') return 0;
		if (isalpha(c)) return 1;
		if (isdigit(c)) return 2;
		if (c == '+' || c == '-' || c == '*' || c == '/') return 6;
		switch (c)
		{
		case '=':
			return 3;
		case '<':
			return 4;
		case '>':
			return 5;
		case ';':
			return 7;
		case '(':
			return 8;
		case ')':
			return 9;
		default:
			return 10;
		}
	}

public:
	// Params: string to analyze, vector to save answer
	void analyze(const char* str, vector<Lexeme>& ans) 
	{
		int position = 0;					// current position
		AState curState = S, prevState;		// current and prev states
		
		Lexeme lex;		// current lexeme
		int beginPos;	// begin position

		int line = 0;
		
		do {
			char currentChar = str[position];
			
			// init when not space
			if (curState == AState::S && !isspace(currentChar)) {
				beginPos = position;
			}

			// transition
			prevState = curState;
			curState = table[getRow(currentChar)][curState];

			// lexeme saving
			if (curState == AState::F || curState == AState::R) {
				int len = position - beginPos;

				lex.str = new char[len + 1];
				strncpy_s(&lex.str[0], len + 1, &str[0] + beginPos, len); // lexeme copy and save

				lex.pos = beginPos;
				lex.line = line + 1;

				// lex state by automata state
				switch (prevState) {
				case A:
					lex.type = Id;
					break;
				case B:
					lex.type = Number;
					break;
				case E:
					lex.type = InvalidId;
					break;
				case C:
					lex.type = Assignment;
					break;
				case D: case G: case H: case I:
					lex.type = CompareOp;
					break;
				case J:
					lex.type = ArithOp;
					break;
				case K:
					lex.type = Semicolon;
					break;
				/*case L:
					lex.type = leftParen;
					break;
				case M:
					lex.type = rightParen;
					break;*/
				case N:		case L: case M:
					lex.type = WrongCh;
					break;
				}

				// keywords and logic ops check
				if (lex.type == Id)
				{
					if (strcmp(lex.str, "do") == 0)
						lex.type = KwDo;
					else if (strcmp(lex.str, "while") == 0)
						lex.type = KwWhile;
					else if (strcmp(lex.str, "loop") == 0)
						lex.type = KwLoop;
					else if (strcmp(lex.str, "not") == 0)
						lex.type = LogicNot;
					else if (strcmp(lex.str, "and") == 0)
						lex.type = LogicAnd;
					else if (strcmp(lex.str, "or") == 0)
						lex.type = LogicOr;

				}

				//if (lex.type == ArithOp && strcmp(lex.str, "-") == 0) lex.type = Minus;

				ans.push_back(lex);

				// if R, backtrack once
				if (curState == AState::R)
					position--;

				// back to S
				curState = AState::S;
			}

			position++;
			if (currentChar == '\n')
				line++;

		} while (str[position - 1] != '\0');
		
	}

};

class SyntaxAnal {
private:

	enum AState { S, A, B, C, D, G, H, N,	K, L, M, O, Q, NN,	E, E1, F }; // automata states

	// transition table
	const AState table[15][14] =
	{
	//	S	A	B	C	D	G	H	N	K	L	M	O	Q	NN

		A,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,
		E,	B,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,
		E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	F,	E,	E,	E,
		E,	E,	C,	K,	C,	C,	C,	E,	E,	M,	E,	M,	K,	E,
		E,	E,	C,	K,	C,	C,	C,	E,	E,	M,	E,	M,	K,	E,
		E,	E,	C,	E,	C,	C,	C,	E,	E,	M,	E,	M,	E,	E,

		E,	E,	D,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,
		E,	E,	E,	G,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,
		E,	E,	E,	G,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,
		E,	E,	E,	H,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,

		E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	Q,	E,	E,	E,
		E,	E,	E,	E,	E,	E,	E,	E,	L,	E,	E,	E,	E,	E,
		E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	O,	E,	E,	E,
		E,	E,	N,	N,	N,	N,	N,	E,	E,	NN,	O,	NN,	E,	E,
		E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E,	E
	};

	// error msg
	void error(int line, AState expectedAfter, LexType whatFound) {
		cout << "error line " << line << ": expected ";
		switch (expectedAfter)
		{
		case SyntaxAnal::S:
			cout << "do";
			break;
		case SyntaxAnal::A:
			cout << "while";
			break;
		case SyntaxAnal::B:
			cout << "logic expr";
			break;
		case SyntaxAnal::C:
			cout << "ops, 'and', 'or', comp op";
			break;
		case SyntaxAnal::D: case SyntaxAnal::G: case SyntaxAnal::H:
			cout << "operand";
			break;
		case SyntaxAnal::N:
			cout << "number";
			break;
		case SyntaxAnal::K:
			cout << "assignment";
			break;
		case SyntaxAnal::L: case SyntaxAnal::O:
			cout << "operand";
			break;
		case SyntaxAnal::M:
			cout << "arith op, semicolon, loop";
			break;
		case SyntaxAnal::Q:
			cout << "operand";
			break;
		default:
			break;
		}
		cout << " but found ";
		switch (whatFound)
		{
		case KwDo:			cout << "keyword_do";			break;
		case KwWhile:		cout << "keyword_while";		break;
		case KwLoop:		cout << "keyword_loop";			break;
		case Id:			cout << "identifier";			break;
		case Number:		cout << "number const";			break;
		case InvalidId:		cout << "invalid id";			break;
		case WrongCh:		cout << "wrong char";			break;
		case Assignment:	cout << "assignment";			break;
		case LogicNot:		cout << "logic_not";			break;
		case LogicAnd:		cout << "logic_and";			break;
		case LogicOr:		cout << "logic_or";				break;
		case CompareOp:		cout << "compare op";			break;
		case ArithOp:		cout << "arithmetic op";		break;
		case Semicolon:		cout << "semicolon";			break;
		default:
			break;
		}
	}


public:
	// main func
	void analyze(const vector<Lexeme>& vec)
	{
		
		int position = 0; // current position
		AState curState = S, prevState; // current and prev states
		LexType whatFound; // what found
		
		Lexeme lex; // current lexeme

		bool needIgnore = false; // needIgnore
		
		do{
			lex = vec[position];
			whatFound = lex.type;

			// if ok
			if (curState == AState::F) {
				cout << "\tprevious construction okay " << endl;
				curState = S;
				cout << lex.str << '\t';
			}
			else {
				cout << lex.str << '\t';
			}

			if (lex.type == InvalidId) {
				cout << "Error line " << lex.line << ": id invalid! ";
			}
			
			if (lex.type == KwDo) {
				if (curState != S && curState != F) {
					cout << "(previous cycle not ended correctly, anyway analyzing next one)";
					curState = S;
				}
			}

			// if all good
			if (curState != AState::E) {

				prevState = curState;
				curState = table[lex.type][curState];

				if (curState != AState::E)
				{
					cout << /*"ok" <<*/ endl;
				}
				else {

					error(lex.line, prevState, whatFound);
					cout << endl;
					
					if (prevState == A) {
						cout << "\twhile not found, i dunno that construction\n\tneed to skip all until 'do while'\n";
						needIgnore = true;
					}
					else if (prevState == S) {
						cout << "\tthing out of cycle, i dunno\n";
						needIgnore = true;
					}
					
					if (!needIgnore) {
						if (lex.type == Semicolon)
							curState = Q;
						else if (lex.type == KwLoop)
							curState = S; 
					}
				}
			}
			else
			{
				if (!needIgnore) {
					if (lex.type == Semicolon) {
						curState = Q;
						cout << /*"ok" <<*/ endl;
					}
					else if (lex.type == KwLoop) {
						curState = S;
						cout << /*"ok" <<*/ endl;
					}
					else
						cout << "skipping" << endl;
				}
				else
					cout << "skipping" << endl;
			}

			position++;

		} while (position < vec.size());
		
		
	}

};

int main() {
	
	cout << "Input filename: ";
	string fn;
	cin >> fn;

	ifstream fin(fn);

	// check: is file open?
	if (!fin.is_open())
	{
		cout << "Couldn't open the file" << endl;
		system("pause");
		return 1;
	}

	// read whole file in one string
	fin.seekg(0, ios::end);
	int length = fin.tellg();	// getting string length with caret manipulations
	char* str = new char[length + 1];
	fin.seekg(0, ios::beg);
	fin.read(&str[0], length);
	str[length] = '\0';			// don't forget terminatig zero

	fin.close();
	
	// create obj
	LexParserFSM la;
	
	vector<Lexeme> ans;		// resulting vector
	la.analyze(str, ans);	// brains
	
	
	// memory clear
	delete[]  str;
	
	// output
	/*
	for (Lexeme& l : ans) {

		cout << setw(20) << left << l.str << '\t';

		switch (l.type)
		{
		case KwDo:			cout << "keyword_do";			break;
		case KwWhile:		cout << "keyword_while";		break;
		case KwLoop:		cout << "keyword_loop";			break;
		case Id:			cout << "identifier";			break;
		case Number:		cout << "number const";			break;
		case InvalidId:		cout << "invalid id";			break;
		case WrongCh:		cout << "wrong char";			break;
		case Assignment:	cout << "assignment";			break;
		case LogicNot:		cout << "logic_not";			break;
		case LogicAnd:		cout << "logic_and";			break;
		case LogicOr:		cout << "logic_or";				break;
		case CompareOp:		cout << "compare op";			break;
		case ArithOp:		cout << "arithmetic op";		break;
		case Semicolon:		cout << "semicolon";			break;
		//case leftParen:		cout << "l parenthesis";		break;
		//case rightParen:	cout << "r parenthesis";		break;
		default:
			break;
		}
		cout << '\t' << l.pos << '\t' << l.line;
		cout << endl;
	}
	*/

	SyntaxAnal sa;
	sa.analyze(ans);

	
	// delete memory
	for (Lexeme& l : ans) {
		delete[] l.str;
	}

	system("pause");
	return 0;
}

