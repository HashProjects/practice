//Numbers FSM
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

using namespace std;


int char_to_col(char ch) {
    if (isdigit(ch))
        return 0;
    if (ch == '.')
        return 1;
    if (isspace(ch))
        return 2;
    return 3; //any invalid character
}

string toString(char ch) {
    switch(ch) {
        case '\n': return "\\n";
        case ' ': return "[space]";
        default: 
        {
            char S[2];
            sprintf(S, "%c", ch);
            return string(S);
        }
    }
}

struct State {
    int state;
    char next_input;
    bool accepted;

    string toString() {
        string result = string("(") + to_string(state) + ")";
        if (next_input != 0) 
            result += " --" + ::toString(next_input) + "--> ";
        else result.append(" ----> ").append(accepted ? "accepted" : "not accepted").append("; ");

        return result;
    }
};

// Define the FSM

// N       | digit |   .   |space  | invalid
// -----------------------------------------
// start  0|   1   |   3   |   2   |   5
// digit  1|   1   |   3   |   2   |   5
// space  2|   4   |   5   |   2   |   5
// float  3|   4   |   5   |   2   |   5
// float  4|   4   |   3   |   2   |   5
//!valid  5|   5   |   5   |   2   |   5

int ntable[6][4] = {
    {1, 3, 2, 5}, 
    {1, 3, 2, 5},
    {1, 5, 2, 5},
    {4, 5, 2, 5},
    {4, 5, 2, 5},
    {5, 5, 5, 5}
};


struct Record {
    string token;
    string lexeme;
    bool accepted;
    string errorMessage;
};

ostream & operator << (ostream & stream, Record record) {
    stream << "token: " << record.token << "; lexeme: " << record.lexeme;
    if (!record.accepted)
        stream << record.errorMessage;
    return stream;
}

class LexicalScanner {
    string w;
    int pos;
public: 

    list<State> stateTransitions;
    int errorCharacter = -1;

    LexicalScanner(string input) : w(input + "\n"), pos(0) {

    }

    bool isFinished() {
        return pos == w.length();
    }

    const int F[1] = {2};

    bool inF(int state) {
        for (int i = 0; i < sizeof(F)/sizeof(F[0]); ++i)
            if (state == F[i])
                return true;
        return false;
    }


    Record lexer() {
        int state = 0; // q0 = 0
        string currentNumber;
        bool isInteger = true;
        errorCharacter = -1;
        Record record;
        bool reachedEnd = false;
        
        size_t i;
        if (isspace(w[pos]))
            pos++;

        for (i = pos; i < w.length() && !reachedEnd; ++i) {
            stateTransitions.push_back({state, w[i]});

            int col = char_to_col(w[i]);
            state = ntable[state][col];

            switch (state) {
                case 1: // inside a number
                    currentNumber += w[i];
                    record.token = "integer";
                    break;
                case 2: // at the end of a number
                    record.lexeme = currentNumber;
                    currentNumber = ""; // reset the current number
                    reachedEnd = true;
                    break;
                case 3:
                    record.token = "float";
                    currentNumber += w[i];
                    break;
                case 4:
                    currentNumber += w[i];
                    break;
                case 5:
                    // return not accepted
                    pos = i;
                    record.lexeme = currentNumber + w[i];
                    record.token  = "unknown";
                    record.accepted = false;
                    record.errorMessage = string("[an invalid character(") + w[i] +
                            + ") was found at position " 
                            + to_string(i) + "]";
                    stateTransitions.push_back({state, 0, record.accepted});
                    
                    return record;
                case 0:
                    break;
            }
        }
        pos = i;

        record.accepted = inF(state);
        stateTransitions.push_back({state, 0, record.accepted});

        if (!record.accepted) {
            record.errorMessage = string("[an invalid character(") + to_string(w[errorCharacter]) +
                + ") was found at position " 
                + to_string(errorCharacter) + "]";
        }

        return record;
    }
};


int main(int, char * []) {
    cout << "Numbers FSM" << endl;

    const char * strings[] = {
        "21 3.4",
        "42 3.4a",
        "3.4e+10",
        "11001 12345.6 12345678 2938498239",
        "11100 93838 6732 8",
        "3.1415926535",
        "3.14 6.28 9.99 2.78",
        " 2 ",
        " 2.1 2a9",
        "2.3.3 9.4"
    };

    for (size_t i = 0; i < sizeof(strings)/sizeof(strings[0]); ++i) {
        cout << "------------------------------------------------------------------" << endl;
        cout << "Processing string: \"" << strings[i] << "\"" << endl;
        LexicalScanner scanner(strings[i]);
        Record record = {"", "", true, ""};
        while (!scanner.isFinished() && record.accepted) {
            record = scanner.lexer();
            cout << record << endl;  
        }
        cout << " State Transitions: ";
            for (list<State>::iterator s = scanner.stateTransitions.begin(); s != scanner.stateTransitions.end(); ++s) {
                cout  << s->toString();
            }
        cout << endl;
    }

    cout << "Number FSM tests complete" << endl << endl;    

    return 0;
}