#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility>   // for pair
//#include <windows.h> //for the Beep() sound

using namespace std;

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32
#define enter (char)13

// a vector of pairs to store the characters with their corresponding condition
vector<pair<char, int>> word;
string initial_txt{"hello world"};

void main_menu();

void start_game();

void mini_menu(int num = 1);

string get_input();

void remove_duplicate_spaces(string &str);

// prints the current word progress
void print(vector<pair<char, int>> &testTxt) {
    system("cls"); // clears the screen
    for (auto pr: testTxt) {
        cout << "\033[1;" << pr.second << "m";

        switch (pr.first) {
            case enter:
                cout << "\\n"
                     << endl;
                break;

            case ' ': // changes whitespace to underscore for convenience
                cout << '_';
                break;

            default:
                cout << pr.first;
                break;
        }
        cout << "\033[0m";
    }
}

// sets up the game text according to the parameter
int setup(const string &txt = initial_txt, int mode = 1) // mode = 1 prints the word else it doesn't
{
    word.clear();

    for (auto &ch: txt)
        word.emplace_back(ch, 0);

    if (mode == 1)
        print(word); // prints the first time to know what to type

    initial_txt = txt;
    return (int) txt.length(); // returns the length of the text
}

void logic() {
    int word_length = setup();

    char typed_char;
    int no_of_chars_typed{0}, correct_chars{0};

    // closes when the typed character equals the length of the text
    while (no_of_chars_typed < word_length) {
        if (kbhit()) // checks if the keyboard was hit
        {
            typed_char = (char) getch(); // takes the typed character as input

            if (typed_char == 27) // exits if the player presses escape
                exit(0);

            if (typed_char == word[no_of_chars_typed].first) {
                // colors the current character green if it is correct
                word[no_of_chars_typed].second = green;
                correct_chars++;
            } else {
                word[no_of_chars_typed].second = red; // colors the character red on mistakes
                // Beep(800, 200);                       // adds a beep sound on mistakes
            }

            print(word); // prints the text with visual cues
            no_of_chars_typed++;
        }
    }
    cout << endl;
    if (correct_chars == word_length)
        cout << endl
             << "\033[1;" << green << "mPERFECT\033[0m";
    if (correct_chars == 0)
        cout << endl
             << "\033[1;" << red << "mFAILED\033[0m";
    // calculates the success rate
    // by dividing the number of correct characters type by the total characters
    cout << endl
         << "Success rate: " << ((correct_chars * 100) / word_length) << '%';
    mini_menu();
}

int main() {
    main_menu();
    return 0;
}

void main_menu() {
    system("cls");
    cout << "\tTYPING GAME" << endl
         << endl
         << "[1] Start game" << endl
         << "[2] Import custom text" << endl
         << "[esc] Exit" << endl;
    input:
    switch (getch()) {
        case '1':
            start_game();
            break;
        case '2':
            setup(get_input(), 2);
            mini_menu(2);
            break;
        case 27: // exits on escape
            exit(0);
        default:
            cout << endl
                 << "Invalid choice" << endl
                 << "enter choice again" << endl;
            goto input;
    }
}

//[1] -> 1 = "play again"; else = "start game";
void mini_menu(int num) {
    cout << endl
         << endl
         << (num == 1 ? "[1] Play again" : "[1] Start game") << endl
         << "[2] Main menu" << endl
         << "[esc] Exit";
    input:
    switch (getch()) {
        case '1':
            start_game();
            break;
        case '2':
            main_menu();
            break;
        case 27:
            exit(0);
        default:
            cout << "Invalid choice" << endl
                 << "enter choice again" << endl;
            goto input;
    }
}

void start_game() {
    system("cls");
    cout << endl
         << "  press any button to start";
    while (true) {
        if (kbhit()) // checks for any buttons pressed to start the game
        {
            getch(); // clears the pressed button from the input buffer
            logic();
            break;
        }
    }
}

string get_input() {
    system("cls");
    cout << "Enter text:" << endl
         << endl;

    string line, text;
    while (getline(cin, line) && !line.empty()) {
        remove_duplicate_spaces(line);
        text.append(line + enter);
    }

    if (!text.empty())
        text.pop_back();
    if (text.back() == ' ')
        text.pop_back();

    cout << "Added successfully!" << endl;
    return text;
}

// removes duplicate whitespaces from text also removes any whitespaces before newlines
void remove_duplicate_spaces(string &str) {
    for (int i = (int) str.size() - 1; i > 0; i--) {
        if (str[i] == ' ' && str[i - 1] == ' ')
            str.erase(str.begin() + i);
    }

    if (str.back() == ' ')
        str.pop_back();
}