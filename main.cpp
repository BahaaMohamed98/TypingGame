#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility> // for pair
// #include <windows.h> //for the Beep() sound
#include <iomanip>
#include <cmath>
#include <chrono> // for time calculation

using namespace std;

class Time
{
private:
    std::chrono::steady_clock::time_point startTime, endTime;
    std::chrono::seconds duration;

public:
    void start()
    {
        startTime = std::chrono::steady_clock::now();
    }

    void end()
    {
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    }

    int getMinutes()
    {
        return static_cast<int>(duration.count() / 60);
    }

    double getSeconds()
    {
        return static_cast<double>(duration.count() % 60);
    }

    double getTotalTimeInSeconds()
    {
        return static_cast<double>(duration.count());
    }
    void printTime()
    {
        cout << setw(2) << setfill('0') << getMinutes() << ':'
             << setw(2) << setfill('0') << getSeconds() << 's';
    }
};

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32
#define enter (char)13
#define backspace (char)8
#define escape (char)27

// a vector of pairs to store the characters with their corresponding condition
vector<pair<char, int>> word;
string current_text{"hello world"};

// string random_words[]{"who an but then oil idea below your made mile three time another watch after those let then add made being line soon of which big mother her first miss together then time mean line part eye is its under long did up never list took group book is its book could page away few know point country its plant once set near cut to fall car my hard be he the still few us animal they will call was black list left add only if who much before later"};

void main_menu();

void start_game();

void mini_menu(int num = 1);

string import_text();

void remove_duplicate_spaces(string &str);

int calculate_wpm(Time &time, int &correct_chars, int &correct_spaces);

void get_test_input(int &word_length, int &correct_chars, int &correct_spaces, int &incorrect_chars);

void displayInstructions();

string calculateAccuracy(int &correct_chars, int &incorrect_chars);

// prints the current word progress
void print(vector<pair<char, int>> &gameTxt)
{
    system("cls"); // clears the screen
    for (auto pr : gameTxt)
    {
        cout << "\033[" << pr.second << "m";

        switch (pr.first)
        {
        case enter:
            cout << " ->"
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
int setup(const string &txt = current_text, int mode = 1) // mode = 1 prints the word else it doesn't
{
    word.clear();

    for (auto &ch : txt)
        word.emplace_back(ch, 0);

    word.shrink_to_fit(); // reduces the vector's capacity to fit its size

    if (mode == 1)
        print(word); // prints the first time to know what to type

    current_text = txt;
    return (int)txt.length(); // returns the length of the text
}

void logic()
{
    int word_length = setup();
    Time time;

    int correct_chars{0}, incorrect_chars{0}, correct_spaces{0};

    // time of the game begin
    time.start();

    get_test_input(word_length, correct_chars, correct_spaces, incorrect_chars);

    // time of the game finish
    time.end();

    cout << endl;
    if (correct_chars == word_length)
        cout << endl
             << "\033[1;" << green << "mPERFECT\033[0m" << endl;
    if (correct_chars == 0)
        cout << endl
             << "\033[1;" << red << "mFAILED\033[0m" << endl;

    // calculates the success rate
    // by dividing the number of correct characters type by the total characters
    cout << endl
         << "WPM: " << calculate_wpm(time, correct_chars, correct_spaces)
         << "  Accuracy: " << calculateAccuracy(correct_chars, incorrect_chars) << endl
         << "Elapsed time: ";
    time.printTime();
    mini_menu();
}

int main()
{
    main_menu();
    return 0;
}

void main_menu()
{
    system("cls");
    cout << '\t' << R"(TYPING GAME 

[1] Start game 
[2] Instructions 
[3] Import custom text 
[4] Settings 
[esc] Exit)"
         << endl;

input:
    switch (getch())
    {
    case '1':
        start_game();
        break;
    case '2':
        displayInstructions();
        break;
    case '3':
        setup(import_text(), 2);
        mini_menu(2);
        break;
    case '4':
        break;
    case escape: // exits on escape
        exit(0);
    default:
        cout << endl
             << "Invalid choice" << endl
             << "Enter choice again" << endl;
        goto input;
    }
}

//[1] -> 1 = "play again"; else = "start game";
void mini_menu(int num)
{
    cout << endl
         << endl
         << (num == 1 ? "[1] Play again" : "[1] Start game") << endl
         << "[2] Main menu" << endl;
input:
    switch (getch())
    {
    case '1':
        start_game();
        break;
    case '2':
        main_menu();
        break;
    default:
        cout << "Invalid choice" << endl
             << "Enter choice again" << endl;
        goto input;
    }
}

void start_game()
{
    system("cls");
    cout << endl
         << "\tpress any button to start";
    while (true)
    {
        if (kbhit()) // checks for any buttons pressed to start the game
        {
            getch(); // clears the pressed button from the input buffer
            logic();
            break;
        }
    }
}

string import_text()
{
    system("cls");
input:
    cout << "Enter text:" << endl
         << endl;

    string line, text;
    // takes input as long as user doesn't submit an empty line
    while (getline(cin, line) && !line.empty())
    {
        remove_duplicate_spaces(line);
        text.append(line + enter);
    }
    if (text.empty())
    {
        cout << "Invalid!" << endl;
        goto input;
    }
    text.pop_back();
    if (text.back() == ' ')
        text.pop_back();

    cout << "Added successfully!";
    return text;
}

// removes duplicate whitespaces from text also removes any whitespaces before newlines
void remove_duplicate_spaces(string &str)
{
    for (int i = (int)str.size() - 1; i > 0; i--)
    {
        if (str[i] == ' ' && str[i - 1] == ' ')
            str.erase(str.begin() + i);
    }

    if (str.back() == ' ')
        str.pop_back();
}

int calculate_wpm(Time &time, int &correct_chars, int &correct_spaces)
{
    if (time.getTotalTimeInSeconds() < 1)
        return 0;

    return round((correct_chars + correct_spaces) * (60 / time.getTotalTimeInSeconds()) / 5.0);
}

void get_test_input(int &word_length, int &correct_chars, int &correct_spaces, int &incorrect_chars)
{
    char typed_char;
    int no_of_chars_typed{0};

    // closes when the typed character equals the length of the text
    while (no_of_chars_typed < word_length)
    {
        if (kbhit()) // checks if the keyboard was hit
        {
            typed_char = (char)getch(); // takes the typed character as input

            if (typed_char == escape) // stops if the player presses escape
                break;

            if (typed_char == backspace)
            {
                no_of_chars_typed--;

                word[no_of_chars_typed].second = 0;
                print(word);
                continue;
            }

            if (typed_char == word[no_of_chars_typed].first)
            {
                // colors the current character green if it is correct
                word[no_of_chars_typed].second = green;
                correct_chars++;
                if (word[no_of_chars_typed].first == ' ' || word[no_of_chars_typed].first == enter)
                    correct_spaces++;
            }
            else
            {
                word[no_of_chars_typed].second = red; // colors the character red on mistakes
                incorrect_chars++;
                // adds a beep sound on mistakes
                // Beep(800, 200);
            }

            print(word); // prints the text with visual cues
            no_of_chars_typed++;
        }
    }
}

void displayInstructions()
{
    system("cls");
    cout << R"(Game instructions:

1. Type the displayed text as accurately and quickly as possible.
2. Use the backspace key to correct mistakes.
3. Words are separated by an underscore "_" to indicate a whitespace.
4. New lines are displayed as "->" to indicate an enter.
5. Pay attention to punctuation and capitalization.
6. The game ends when you've typed the entire text correctly.
7. Press Escape to stop the game and head back to the menu.

Have fun while improving your typing skills!)";

    mini_menu(2);
}

string calculateAccuracy(int &correct_chars, int &incorrect_chars)
{
    return to_string((int)round((correct_chars * 100.0) / (correct_chars + incorrect_chars))) + '%';
}
