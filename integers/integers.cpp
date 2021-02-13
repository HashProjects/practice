//Integers FSM
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

using namespace std;


int char_to_col(char ch) {
    if (isdigit(ch))
        return 0;
    if (isspace(ch)) // is . being detected here
        return 1;
    return -1; //this will crash the program
}

char state_to_label(int state) {
    return '0' + state;
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
        else result.append(" ----> ").append(accepted ? "accepted" : "not accepted");

        return result;
    }
};

// Define the FSM

// N      | digit | space
// ----------------------
// start 0|   1   |   2
// digit 1|   1   |   2
// space 2|   1   |   2

int ntable[3][2] = {
    {1, 2}, 
    {1, 2},
    {1, 2},
};

// F = space
int F[] = {2};

bool inF(int state) {
    for (int i = 0; i < sizeof(F)/sizeof(F[0]); ++i)
        if (state == F[i])
            return true;
    return false;
}

static list<long> integers;
static list<State> stateTransitions;

bool integerFSM(string w) {
    int state = 0; // q0 = 0
    string currentNumber;
    
    for (size_t i = 0; i < w.length(); ++i) {
        stateTransitions.push_back({state, w[i]});

        int col = char_to_col(w[i]);
        state = ntable[state][col];

        switch (state) {
            case 1: // inside a number
                currentNumber += w[i];
                break;
            case 2: // at the end of a number
                try {
                    if (currentNumber.length() > 0)
                        integers.push_back(stol(currentNumber));
                    currentNumber = ""; // reset the current number
                } catch (invalid_argument & e) {
                    cout << e.what() << ":invalid argument" << ":'" << currentNumber << "'" << endl;
                } catch (out_of_range & e) {
                    cout << e.what() << ":out of range" << ":'" << currentNumber << "'" << endl;
                }
                break;
            case 0:
                break;
        }
    }


    bool accepted = inF(state);
    stateTransitions.push_back({state, 0, accepted});
    cout << (accepted ? "accepted: " : "not accepted: ");
    return accepted;
}


int main(int, char * []) {
    cout << "Integer FSM" << endl;

    const char * strings[] = {
        "21",
        "42 34",
        "11001 123456 12345678 2938498239",
        "11100 93838 6732 8",
        " 2 "
    };

    for (size_t i = 0; i < sizeof(strings)/sizeof(strings[0]); ++i) {
        cout << "------------------------------------------------------------------" << endl;
        cout << "Processing string: " << strings[i] << endl;
        cout << " " << "integerFSM: " << integerFSM(string(strings[i]) + "\n") << endl;

        cout << " State Transitions: ";
        for (list<State>::iterator s = stateTransitions.begin(); s != stateTransitions.end(); ++s) {
            cout  << s->toString();
        }
        stateTransitions.clear();
        cout << endl;
    }

    cout << "Integer FSM tests complete" << endl << endl;

    cout << "Print out list of integers found:" << endl;
    for(list<long>::iterator i = integers.begin(); i != integers.end(); ++i) {
        cout << *i << endl;
    }
    cout << endl;
    

    return 0;
}