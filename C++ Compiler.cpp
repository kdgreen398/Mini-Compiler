#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
using namespace std;


class Token {
private:
	string TYPE;
	string VALUE;

public:
	Token(string type_, string value_) {
		TYPE = type_;
		VALUE = value_;
	}

	string GetType() {
		return TYPE;
	}

	string GetValue() {
		return VALUE;
	}
};

class Lexer {
private:
	vector<Token> TOKEN_LIST; // I also include a vector of the tokens just incase they are gone from the queue but you may still need to reuse them
	queue<Token> TOKEN_QUEUE;

public:
	void AddToken(string type_, string* value_) {
		if (*value_ != "") {
			Token t = Token(type_, *value_);
			TOKEN_LIST.push_back(t);
			TOKEN_QUEUE.push(t);

			*value_ = "";
		}
	}

	queue<Token> GetTokenQueue() {
		return TOKEN_QUEUE;
	}

	vector<Token> GetTokenVector() {
		return TOKEN_LIST;
	}

	void ShowTokens() {
		for (int i = 0; i < TOKEN_LIST.size(); i++) {
			cout << TOKEN_LIST.at(i).GetValue() << "\t" << TOKEN_LIST.at(i).GetType() << endl;
		}
	}

	void GetTokensFromCode(string code_string) {
		string token_value = "";
		string token_type = "";

		bool foundError = false;

		/*
		*   THIS LOOP CHECKS EACH CHARACTER IN THE GIVEN LINE.
		*   0-9: INTEGER LITERAL
		*   A-Z: IDENTIFIER , if 'input' then it is a input call, if 'display' then it is a display call, if (") then it is a string
		*   +,-,*,/ : EXPRESSION OPERATORS
		*	;: END OF STATEMENT
		*	if a string is started but no end quote for the string is found, then the process will be stopped and the queue will be cleared of ALL tokens
		*/

		for (int i = 0; i < code_string.length(); i++) {
			// loop through the code character by character
			char character = code_string.at(i);

			if (isdigit(character)) {
				// if current character is a number
				while (isdigit(character)) {
					// add it and every following number to the end of it to complete the full number
					token_value += character;
					i++;
					if (i < code_string.length())
						character = code_string.at(i);
					else
						break;
				}

				this->AddToken("INTEGER", &token_value);
			}
			if (isalpha(character)) {
				// if current character is an alphabet
				while (isalpha(character)) {
					// add it and every following alphabet to the end of it to complete the full word
					token_value += character;
					i++;
					if (i < code_string.length())
						character = code_string.at(i);
					else
						break;
				}
				if (token_value == "Begin") // if the resulting word is display, then it is our display keyword
					this->AddToken("PROGRAM_START", &token_value);
				else if (token_value == "End") // if the resulting word is display, then it is our display keyword
					this->AddToken("PROGRAM_END", &token_value);
				if (token_value == "input") // if the resulting word is input, then it is our input keyword
					this->AddToken("Input Call", &token_value);
				else if (token_value == "display") // if the resulting word is display, then it is our display keyword
					this->AddToken("Display Call", &token_value);
				else // else the resulting word is just an identifier
					this->AddToken("IDENTIFIER", &token_value);
			}
			if (character == ';') {
				token_value = character;
				this->AddToken("SEMICOLON", &token_value);
			}
			if (character == '+') {
				token_value = character;
				this->AddToken("ADDITION_OPERATOR", &token_value);
			}
			if (character == '-') {
				token_value = character;
				this->AddToken("SUBTRACTION_OPERATOR", &token_value);
			}
			if (character == '*') {
				token_value = character;
				this->AddToken("MULTIPLICATION_OPERATOR", &token_value);
			}
			if (character == '=') {
				token_value = character;
				this->AddToken("ASSIGNMENT_OPERATOR", &token_value);
			}
			if (character == '/') {
				token_value = character;
				this->AddToken("DIVISION_OPERATOR", &token_value);
			}
			if (character == '"') {
				do {
					// if current character is a quote ("), add every following character until an end quote is found
					if (character != '"') // don't add the quotes to the string
						token_value += character;
					i++;
					if (i < code_string.length())
						character = code_string.at(i);
					else {
						// executes if end quote (") for string is never found
						foundError = true;
						break;
					}
				} while (character != '"');
				if (foundError) {
					// if error was found, print the error and clear the queue to prevent code was running
					cout << "ERROR: mising closing (\")";
					while (!this->GetTokenQueue().empty())
						this->GetTokenQueue().pop();
					return;

				}
				this->AddToken("STRING", &token_value);
			}
		}
	}
};

bool ErrorChecking(queue<Token> token_queue) {
	if (token_queue.front().GetType() == "PROGRAM_START") {
		bool endFound = false;
		token_queue.pop();

		while (!token_queue.empty()) {
			if (token_queue.front().GetType() == "IDENTIFIER") { // In this case, a variable is being initialized
				token_queue.pop();
				if (token_queue.front().GetType() == "ASSIGNMENT_OPERATOR") { // equal sign must be next
					token_queue.pop();
					if (token_queue.front().GetType() == "IDENTIFIER" || token_queue.front().GetType() == "INTEGER") { // A variable or an Integer must be assigned
						token_queue.pop();
						if (token_queue.front().GetType() == "SEMICOLON") { // must end with semicolon
							token_queue.pop();
						}
						else if (token_queue.front().GetType() == "ADDITION_OPERATOR" || token_queue.front().GetType() == "SUBTRACTION_OPERATOR" || token_queue.front().GetType() == "MULTIPLICATION_OPERATOR" || token_queue.front().GetType() == "DIVISION_OPERATOR") { // or followed by an operator to perform an expression
							token_queue.pop();
							if (token_queue.front().GetType() == "IDENTIFIER" || token_queue.front().GetType() == "INTEGER") { // operator must be followed by another variable or another integer
								token_queue.pop();
								if (token_queue.front().GetType() == "SEMICOLON") { // statement must end with a semicolon
									token_queue.pop();
								}
								else {
									cout << "ERROR: Expected a semicolon." << endl;
									return true;
								}

							}
							else {
								cout << "ERROR: Expected an integer or variable for opperation" << endl;
								return true;
							}
						}
						else {
							cout << "ERROR: Expected a semicolon" << endl;
							return true;
						}
					}
					else {
						cout << "ERROR: Expected an integer or variable for assignment" << endl;
						return true;
					}
				}
				else
				{
					cout << "ERROR: Variable not initialized properly" << endl;
					return true;
				}

			}
			else if (token_queue.front().GetType() == "Input Call") { // if receiving a user input
				token_queue.pop();
				if (token_queue.front().GetType() == "IDENTIFIER") { // must be followed by a variable
					token_queue.pop();
					if (token_queue.front().GetType() == "SEMICOLON") { // must end with a semicolon
						token_queue.pop();
					}
					else {
						cout << "ERROR: Expected a semicolon" << endl;
						return true;
					}
				}
				else {
					cout << "ERROR: Variable required for input call" << endl;
					return true;
				}
			}
			else if (token_queue.front().GetType() == "Display Call") { // if displaying a variable or string
				token_queue.pop();
				if (token_queue.front().GetType() == "IDENTIFIER") { // value must be a variavle...
					token_queue.pop();
					if (token_queue.front().GetType() == "SEMICOLON") { // followed by a semicolon
						token_queue.pop();
					}
					else {
						cout << "ERROR: Expected a semicolon" << endl;
						return true;
					}
				}
				else if (token_queue.front().GetType() == "STRING") { // ...or value could be a string
					token_queue.pop();
					if (token_queue.front().GetType() == "SEMICOLON") { // followed by a semicolon
						token_queue.pop();
					}
					else {
						cout << "ERROR: Expected a semicolon" << endl;
						return true;
					}
				}
				else {
					cout << "ERROR: Variable or string required for display call" << endl;;
					return true;
				}
			}
			else if (token_queue.front().GetType() == "PROGRAM_END") { // checking for END keyword
				token_queue.pop();
				return false;
			}
			
		}

		if (endFound == false) { // this value is false by default. If the END keyword is found, then it will return the function and this line will never run; but if END keyword isn't found then this will execute
			cout << "ERROR: Program End not Found" << endl;
			return true;
		}
	}
	else {
		cout << "ERROR: Program Begin not found" << endl;
		return true;
	}
}

void ExecuteCode(queue<Token> token_queue) {
	int A = 0, B = 0, C = 0; // integers initialized with value of 0

	while (!token_queue.empty()) {
		if (token_queue.front().GetValue() == "A") { // ASSIGNING VALUE TO A
			token_queue.pop(); // pop the A
			token_queue.pop(); // pop the equal sign (=)

			string value = token_queue.front().GetValue();
			token_queue.pop();

			if (token_queue.front().GetValue() == ";") {
				// if value to be assigned is a variable or an integer
				if (value == "B") {
					A = B;
				}
				else if (value == "C") {
					A = C;
				}
				else {
					A = stoi(value);
				}
			}
			else {
				// if assigning an expression, evaluate the expression then assign it to the variable
				int result;
				int value_2;

				if (value == "A")
					value_2 = A;
				else if (value == "B")
					value_2 = B;
				else if (value == "C")
					value_2 = C;

				if (token_queue.front().GetValue() == "+") {
					token_queue.pop();

					if (token_queue.front().GetValue() == "A") {
						A = value_2 + A;
					}
					else if (token_queue.front().GetValue() == "B") {
						A = value_2 + B;
					}
					else if (token_queue.front().GetValue() == "C") {
						A = value_2 + C;
					}
					else {
						A = value_2 + stoi(token_queue.front().GetValue());
					}

				}
				else if (token_queue.front().GetValue() == "-") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						A = value_2 - A;
					}
					else if (token_queue.front().GetValue() == "B") {
						A = value_2 - B;
					}
					else if (token_queue.front().GetValue() == "C") {
						A = value_2 - C;
					}
					else {
						A = value_2 - stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "*") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						A = value_2 * A;
					}
					else if (token_queue.front().GetValue() == "B") {
						A = value_2 * B;
					}
					else if (token_queue.front().GetValue() == "C") {
						A = value_2 * C;
					}
					else {
						A = value_2 * stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "/") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						A = value_2 / A;
					}
					else if (token_queue.front().GetValue() == "B") {
						A = value_2 / B;
					}
					else if (token_queue.front().GetValue() == "C") {
						A = value_2 / C;
					}
					else {
						A = value_2 / stoi(token_queue.front().GetValue());
					}
				}
			}
		}
		else if (token_queue.front().GetValue() == "B") { // ASSIGNING VALUE TO B
			token_queue.pop(); // pop the B
			token_queue.pop(); // pop the equal sign (=)

			string value = token_queue.front().GetValue();
			token_queue.pop();

			if (token_queue.front().GetValue() == ";") {
				// if value to be assigned is a variable or an integer
				if (value == "A") {
					B = A;
				}
				else if (value == "C") {
					B = C;
				}
				else {
					B = stoi(value);
				}
			}
			else {
				// if assigning an expression, evaluate the expression then assign it to the variable
				int result;
				int value_2;

				if (value == "A")
					value_2 = A;
				else if (value == "B")
					value_2 = B;
				else if (value == "C")
					value_2 = C;
				
				if (token_queue.front().GetValue() == "+") {
					token_queue.pop();

					if (token_queue.front().GetValue() == "A") {
						B = value_2 + A;
					}
					else if (token_queue.front().GetValue() == "B") {
						B = value_2 + B;
					}
					else if (token_queue.front().GetValue() == "C") {
						B = value_2 + C;
					}
					else {
						B = value_2 + stoi(token_queue.front().GetValue());
					}

				}
				else if (token_queue.front().GetValue() == "-") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						B = value_2 - A;
					}
					else if (token_queue.front().GetValue() == "B") {
						B = value_2 - B;
					}
					else if (token_queue.front().GetValue() == "C") {
						B = value_2 - C;
					}
					else {
						B = value_2 - stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "*") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						B = value_2 * A;
					}
					else if (token_queue.front().GetValue() == "B") {
						B = value_2 * B;
					}
					else if (token_queue.front().GetValue() == "C") {
						B = value_2 * C;
					}
					else {
						B = value_2 * stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "/") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						B = value_2 / A;
					}
					else if (token_queue.front().GetValue() == "B") {
						B = value_2 / B;
					}
					else if (token_queue.front().GetValue() == "C") {
						B = value_2 / C;
					}
					else {
						B = value_2 / stoi(token_queue.front().GetValue());
					}
				}
			}
			
		}
		else if (token_queue.front().GetValue() == "C") { // ASSIGNING VALUE TO C
			token_queue.pop(); // pop the C
			token_queue.pop(); // pop the equal sign (=)
			string value = token_queue.front().GetValue();
			token_queue.pop();

			if (token_queue.front().GetValue() == ";") {
				// if value to be assigned is a variable or an integer
				if (value == "A") {
					C = A;
				}
				else if (value == "B") {
					C = B;
				}
				else {
					C = stoi(value);
				}
			}
			else {
				// if assigning an expression, evaluate the expression then assign it to the variable
				int result;
				int value_2;

				if (value == "A")
					value_2 = A;
				else if (value == "B")
					value_2 = B;
				else if (value == "C")
					value_2 = C;

				if (token_queue.front().GetValue() == "+") {
					token_queue.pop();

					if (token_queue.front().GetValue() == "A") {
						C = value_2 + A;
					}
					else if (token_queue.front().GetValue() == "B") {
						C = value_2 + B;
					}
					else if (token_queue.front().GetValue() == "C") {
						C = value_2 + C;
					}
					else {
						C = value_2 + stoi(token_queue.front().GetValue());
					}

				}
				else if (token_queue.front().GetValue() == "-") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						C = value_2 - A;
					}
					else if (token_queue.front().GetValue() == "B") {
						C = value_2 - B;
					}
					else if (token_queue.front().GetValue() == "C") {
						C = value_2 - C;
					}
					else {
						C = value_2 - stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "*") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						C = value_2 * A;
					}
					else if (token_queue.front().GetValue() == "B") {
						C = value_2 * B;
					}
					else if (token_queue.front().GetValue() == "C") {
						C = value_2 * C;
					}
					else {
						C = value_2 * stoi(token_queue.front().GetValue());
					}
				}
				else if (token_queue.front().GetValue() == "/") {
					token_queue.pop();
					if (token_queue.front().GetValue() == "A") {
						C = value_2 / A;
					}
					else if (token_queue.front().GetValue() == "B") {
						C = value_2 / B;
					}
					else if (token_queue.front().GetValue() == "C") {
						C = value_2 / C;
					}
					else {
						C = value_2 / stoi(token_queue.front().GetValue());
					}
				}
			}
		}
		else if (token_queue.front().GetValue() == "input") { // ASSIGNING A VALUE USING INPUT
			token_queue.pop();
			if (token_queue.front().GetValue() == "A") {
				cout << "Enter a value for A: ";
				cin >> A;
			}
			else if (token_queue.front().GetValue() == "B") {
				cout << "Enter a value for B: ";
				cin >> B;
			}
			else if (token_queue.front().GetValue() == "C") {
				cout << "Enter a value for C: ";
				cin >> C;
			}
		}
		else if (token_queue.front().GetValue() == "display") { // DISPLAYING A VALUE
			token_queue.pop();
			if (token_queue.front().GetValue() == "A") {
				cout << A << endl;
			}
			else if (token_queue.front().GetValue() == "B") {
				cout << B << endl;
			}
			else if (token_queue.front().GetValue() == "C")
				cout << C << endl;
			else if (token_queue.front().GetType() == "STRING")
				cout << token_queue.front().GetValue() << endl;
		}

		token_queue.pop(); // advance to the next token
	}

}

string GetCodeFromFile(string path) {
	ifstream program_file(path.c_str());
	if (program_file.is_open())
	{
		string code_string = "";
		string line;
		while (getline(program_file, line))
		{
			code_string += line + "\n";
		}
		program_file.close();

		cout << "CODE TO BE EXECUTED:\n\n" << code_string << "\n\n" << "OUTPUT:\n";
		return code_string;
	}
	else {
		return "Unable to open file";
	}
}

int main()
{
	string filename;
	cout << "Enter the path for the file: " << endl;
	cin >> filename;

	string code_string = GetCodeFromFile(filename);

	if (code_string != "Unable to open file") {
		Lexer lex;
		lex.GetTokensFromCode(code_string); // Breaks the input code down into tokens

		// ALL TOKENS GATHER FROM THE LEXER CAN BE PRINTED TO THE CONSOLE USING THE lex.ShowTokens() FUNCTION BELOW
		// lex.ShowTokens();

		queue<Token> t_queue = lex.GetTokenQueue();

		if (!ErrorChecking(t_queue)) // checks for errors in the code, if none are found then the code is executed
			ExecuteCode(t_queue);
	}
	else {
		cout << code_string << endl; // runs if file wasn't opened correctly
	}

	system("Pause");

	/*
	*
	* Fell free to use this test code string to run the program as well:
	* string test_code = "Begin B = 10; display \"Hi\"; A = 15; B = A / B; display B; input C; display C; End";
	* And pass the test code into the lex.GetTokenFromCode() function
	* 
	*/

}
