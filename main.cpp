#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility> // for pair
#include <iomanip> // for output manipulation
#include <cmath>   // for simple math
#include <chrono>  // for time calculation
#include <thread>  // for multithreading
// #include <windows.h> //for the Beep() sound

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
        cout << "Elapsed time: " << setw(2) << setfill('0') << getMinutes() << ':'
             << setw(2) << setfill('0') << getSeconds() << 's';
    }
};

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32
#define enter (char)13
#define backspace (char)8
#define escape (char)27
#define cls cout << "\033[2J\033[1;1H"; // ANSI escape code to clear screen

// a vector of pairs to store the characters with their corresponding condition (color)
vector<pair<char, int>> word;
string current_text{"hello world"};

void main_menu();

void start_game();

void mini_menu(int mode = 1);

string import_text();

void remove_duplicate_spaces(string &str);

int calculate_wpm(Time &time, int &correct_chars, int &correct_spaces);

void get_test_input(int &word_length, int &correct_chars, int &correct_spaces, int &incorrect_chars);

void displayInstructions();

string calculateAccuracy(int &correct_chars, int &incorrect_chars);

void game_intro();

// prints the current word progress
void print(vector<pair<char, int>> &gameTxt, int charsTyped, const int &wordLength)
{
    // calculates progress percentage
    int percentDone = ((charsTyped * 100) / wordLength);
    cls; // clears the screen
    for (auto pr : gameTxt)
    {
        cout << "\033[" << pr.second << "m" << flush;

        switch (pr.first)
        {
        case enter:
            cout << " ->"
                 << endl
                 << flush;
            break;

        case ' ': // changes whitespace to underscore for convenience
            cout << '_' << flush;
            break;

        default:
            cout << pr.first << flush;
            break;
        }
        cout << "\033[0m" << flush;
    }
    // prints progress
    cout << endl
         << endl
         << "progress: " << percentDone << '%' << endl;

    for (int i = 0; i < percentDone; i += 10)
    {
        cout << '#';
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
        print(word, 0, current_text.length()); // prints the first time to know what to type

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
         << "  Accuracy: " << calculateAccuracy(correct_chars, incorrect_chars) << endl;
    time.printTime();
    mini_menu();
}

int main()
{
    cout << "\033[?25l"; // ANSI escape code to hide cursor
    main_menu();
    return 0;
}

void main_menu()
{
    cls;
    cout << '\t' << R"(TYPING GAME 

[1] Start game 
[2] Instructions 
[3] Import custom text 
[4] Settings 
[esc] Exit)";

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

//[1] -> mode 1 = "play again"; else = "start game";
void mini_menu(int mode)
{
    cout << endl
         << endl
         << (mode == 1 ? "[1] Play again" : "[1] Start game") << endl
         << "[2] Main menu";
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
        goto input; // goes back to take input again
    }
}

bool testIsActive = false;
void start_game()
{
    testIsActive = true;
    cls;
    thread intro(game_intro); // makes another thread for game intro while taking input from the main thread

    while (true)
    {
        if (kbhit()) // checks for any buttons pressed to start the game
        {
            testIsActive = false;
            getch(); // clears the pressed button from the input buffer
            logic();
            break;
        }
    }
}

string import_text()
{
    cout << "\033[?25h"; // enables cursor
    cls;
input:
    cout << "Enter text [Double enter to insert]:" << endl
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

    cout << "\033[?25l"; // disable cursor again
    cout << "Added successfully!";
    return text;
}

// removes duplicate whitespaces from text also removes any whitespaces before newlines
void remove_duplicate_spaces(string &str)
{
    // loops on the text to remove any redundant whitespaces
    for (int i = (int)str.size() - 1; i > 0; i--)
    {
        if (str[i] == ' ' && str[i - 1] == ' ')
            str.erase(str.begin() + i);
    }

    // removes the last character if it is a whitespace
    if (str.back() == ' ')
        str.pop_back();
}

int calculate_wpm(Time &time, int &correct_chars, int &correct_spaces)
{
    if (time.getTotalTimeInSeconds() < 1) // avoids dividing by zero
        return 0;
    // calculates WPM and rounds it to the nearest integer
    return static_cast<int>(round((correct_chars + correct_spaces) * (60 / time.getTotalTimeInSeconds()) / 5.0));
}

void get_test_input(int &word_length, int &correct_chars, int &correct_spaces, int &incorrect_chars)
{
    char typed_char;
    int index{0}, typedChars{0} /* for progress */;

    // closes when the typed character equals the length of the text
    while (index < word_length)
    {
        if (kbhit()) // checks if the keyboard was hit
        {
            typed_char = (char)getch(); // takes the typed character as input

            if (typed_char == escape) // stops if the player presses escape
                break;

            if (typed_char == backspace) // backspace functionality
            {
                word[--index].second = 0; // changes the character back to normal color
                typedChars--;
                print(word, typedChars, word_length);
                continue; // skips the rest of the iteration
            }

            if (typed_char == word[index].first)
            {
                // colors the current character green if it is correct
                word[index].second = green;
                correct_chars++; // increments the count of the correct characters typed

                if (word[index].first == ' ' || word[index].first == enter)
                    correct_spaces++;
                typedChars++;
            }
            else
            {
                word[index].second = red; // colors the character red on mistakes
                incorrect_chars++;        // increments the count of the incorrect characters typed
                typedChars++;
                // adds a beep sound on mistakes
                // Beep(800, 200); /* slows down input */
            }

            print(word, typedChars, word_length); // prints the text with visual cues
            index++;
        }
    }
}

void displayInstructions()
{
    cls;
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
    if (correct_chars == 0)
        return "0%";
    return to_string(static_cast<int>(round((correct_chars * 100.0) / (correct_chars + incorrect_chars)))) + '%';
}

void game_intro() // displays blinking game intro
{
    using namespace std::literals::chrono_literals;
    bool displayMessage = true;
    while (testIsActive)
    {
        if (displayMessage)
            cout << endl
                 << "\tPress any button to start" << flush;
        else
            cls;
        displayMessage = !displayMessage;
        this_thread::sleep_for(1s); // waits before continuing
    }
    testIsActive = true;
}