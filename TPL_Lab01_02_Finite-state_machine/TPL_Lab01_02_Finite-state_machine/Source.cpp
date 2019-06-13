#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// lexeme container 
struct Lexeme {
	bool valid;		// is valid or not
	char* str;		// lexeme's string
};

// lexeme analyzer based on finite-state machine
class LexemeAnalyzerFSM {
private:

	vector<vector<int>> table;	// automata transinion table

	struct States
	{
		int S, E, F;
	} states;				// important states: start, error, finish

	int(*getCol)(char);		// function (pointer) returning coloumn number by inputting character

public:
	// class constructor
	// Input params: transition table (2d vector int), start state number, 
	//				 error state number, finish state number,
	//				 function (pointer) returning coloumn number by inputting character
	LexemeAnalyzerFSM(vector<vector<int>>&& matrix, int startState, int errorState, int finishState, int(*colFunc)(char)) :
		table(matrix), getCol(colFunc)
	{
		states.S = startState;
		states.E = errorState;
		states.F = finishState;
	}

	// Params: string to analyze, vector to save answer
	void analyze(const char* str, vector<Lexeme>& ans)
	{
		int position = 0;		// current position
		int state = states.S;	// current state
		Lexeme lex;				// current lexeme
		int beginPos;			// lexeme's begin position

		while (str[position] != '\0') {
			char currentChar = str[position];

			// lexeme init when non-space
			if (state == states.S && !isspace(currentChar)) {
				beginPos = position;
				lex.valid = true;
			}

			// state transition
			state = table[state][getCol(currentChar)];

			// is valid?
			if (state == states.E)
				lex.valid = false;

			// if we're at string end, with non-space and not in S, we must end
			bool lastAndNotSpace = (str[position + 1] == '\0' && state != states.S);

			// lexeme saving
			if (state == states.F || lastAndNotSpace)
			{
				int len = position - beginPos;
				if (lastAndNotSpace) len++; // because len = position + 1 - beginPos

				lex.str = new char[len + 1];
				strncpy_s(&lex.str[0], len + 1, &str[0] + beginPos, len); // lexeme copy and save

				ans.push_back(lex);

				state = states.S;
			}

			position++;
		}

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

	// creating object (instance)
	LexemeAnalyzerFSM la(
		{							// transition table
			{ 1, 2, 0 },
			{ 1, 2, 3 },
			{ 2, 2, 3 }
		},
		0, 2, 3,					// state numbers: S, E, F
		[](char ch) -> int			// lambda-function returning coloumn number by inputting character
		{
			if (isalpha(ch)) return 0;
			if (isspace(ch)) return 2;
			return 1;
		}
	);
	
	vector<Lexeme> ans;		// resulting vector
	la.analyze(str, ans);

	// shows input file on screen
	cout << "INPUT:\n" << str << "\n---------";

	// memory clear
	delete[]  str;


	ofstream ofCorrect("correct.txt");
	ofstream ofIncorrect("incorrect.txt");

	// writing all lexemes to files, an printing correct on cmd
	cout << "\nCORRECT:\n";
	for (Lexeme l : ans) {
		if (l.valid) {
			ofCorrect << l.str << '\n';
			cout << l.str << ' ';
		}
		else
			ofIncorrect << l.str << '\n';
	}
	cout << "\n---------";

	// printing incorrect on cmd and freeing memory
	cout << "\nINCORRECT:\n";
	for (Lexeme l : ans) {
	
		if (!l.valid)
			cout << l.str << ' ';

		delete[] l.str;
	}
	cout << "\n---------";

	ofCorrect.close();
	ofIncorrect.close();

	cout << endl;
	system("pause");
	return 0;
}