#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility> // for pair
// #include <windows.h> //for the Beep() sound
#include <iomanip>
#include <cmath>

#include <chrono> // for time calculation

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
};

using namespace std;

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32
#define enter (char)13

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

void test_input(int &word_length, int &no_of_chars_typed, int &correct_chars, int &correct_spaces);

// prints the current word progress
void print(vector<pair<char, int>> &gameTxt)
{
    system("cls"); // clears the screen
    for (auto pr : gameTxt)
    {
        cout << "\033[1;" << pr.second << "m";

        switch (pr.first)
        {
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

    int no_of_chars_typed{0}, correct_chars{0}, correct_spaces{0};

    // time of the game begin
    time.start();

    test_input(word_length, no_of_chars_typed, correct_chars, correct_spaces);

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
         << "WPM: "
         << calculate_wpm(time, correct_chars, correct_spaces)
         << "  Accuracy: " << ((correct_chars * 100) / word_length) << '%' << endl
         << "Elapsed time: " << setw(2) << setfill('0') << time.getMinutes() << ':'
         << setw(2) << setfill('0') << time.getSeconds() << 's';
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
    cout << "\tTYPING GAME" << endl
         << endl
         << "[1] Start game" << endl
         << "[2] Import custom text" << endl
         << "[esc] Exit" << endl;
input:
    switch (getch())
    {
    case '1':
        start_game();
        break;
    case '2':
        setup(import_text(), 2);
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
void mini_menu(int num)
{
    cout << endl
         << endl
         << (num == 1 ? "[1] Play again" : "[1] Start game") << endl
         << "[2] Main menu" << endl
         << "[esc] Exit";
input:
    switch (getch())
    {
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

void start_game()
{
    system("cls");
    cout << endl
         << "  press any button to start";
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
    cout << "Enter text:" << endl
         << endl;

    string line, text;
    // takes input as long as user doesn't submit an empty line
    while (getline(cin, line) && !line.empty())
    {
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

void test_input(int &word_length, int &no_of_chars_typed, int &correct_chars, int &correct_spaces)
{
    char typed_char;

    // closes when the typed character equals the length of the text
    while (no_of_chars_typed < word_length)
    {
        if (kbhit()) // checks if the keyboard was hit
        {
            typed_char = (char)getch(); // takes the typed character as input

            if (typed_char == 27) // exits if the player presses escape
                exit(0);

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
                // Beep(800, 200);                       // adds a beep sound on mistakes
            }

            print(word); // prints the text with visual cues
            no_of_chars_typed++;
        }
    }
}