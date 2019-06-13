#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

enum LexType { KwDo, KwWhile, KwLoop, 
	Id, Number, InvalidId, WrongCh, Assignment, 
	LogicNot, LogicAnd, LogicOr, CompareOp, ArithOp, 
	Semicolon, leftParen, rightParen }; //lexeme types

// lexeme container 
struct Lexeme {
	LexType type;	// lexeme type
	char* str;		// lexeme's string
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
		if (isalpha(c) || c == '_') return 1;
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
				case L:
					lex.type = leftParen;
					break;
				case M:
					lex.type = rightParen;
					break;
				case N:
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

				ans.push_back(lex);

				// if R, backtrack once
				if (curState == AState::R)
					position--;

				// back to S
				curState = AState::S;
			}

			position++;

		} while (str[position - 1] != '\0');
		
	}

};

int main() {
	
	ifstream fin("input.txt");

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
	for (Lexeme& l : ans) {

		cout << setw(20) << left << l.str << '\t';

		switch (l.type)
		{
		case KwDo:			cout << "keyword_do";			break;
		case KwWhile:		cout << "keyword_while";		break;
		case KwLoop:		cout << "keyword_loop";			break;
		case Id:			cout << "identifier";			break;
		case Number:		cout << "number constant";		break;
		case InvalidId:		cout << "invalid identifier";	break;
		case WrongCh:		cout << "wrong char";			break;
		case Assignment:	cout << "assignment";			break;
		case LogicNot:		cout << "logic_not";			break;
		case LogicAnd:		cout << "logic_and";			break;
		case LogicOr:		cout << "logic_or";				break;
		case CompareOp:		cout << "compare operation";	break;
		case ArithOp:		cout << "arithmetic operation";	break;
		case Semicolon:		cout << "semicolon";			break;
		case leftParen:		cout << "l parenthesis";		break;
		case rightParen:	cout << "r parenthesis";		break;
		default:
			break;
		}

		cout << endl;
	}
	
	// delete memory
	for (Lexeme& l : ans) {
		delete[] l.str;
	}
	
	system("pause");
	return 0;
}

