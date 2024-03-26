#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility> // for pair
// #include <windows.h> //for the Beep() sound
#include <chrono> // for time calculation
#include <iomanip>
#include <cmath>

using namespace std;

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32
#define enter (char)13

// a vector of pairs to store the characters with their corresponding condition
vector<pair<char, int>> word;
string current_text{"hello world"};
int no_of_words{2};
// string random_words[]{"who an but then oil idea below your made mile three time another watch after those let then add made being line soon of which big mother her first miss together then time mean line part eye is its under long did up never list took group book is its book could page away few know point country its plant once set near cut to fall car my hard be he the still few us animal they will call was black list left add only if who much before later"};

void main_menu();

void start_game();

void mini_menu(int num = 1);

string get_input();

void remove_duplicate_spaces(string &str);

int count_words(const string &words);

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

    word.shrink_to_fit(); // reduces the vectors's capacity to fit its size

    if (mode == 1)
        print(word); // prints the first time to know what to type

    current_text = txt;
    no_of_words = count_words(txt);
    return (int)txt.length(); // returns the length of the text
}

void logic()
{
    int word_length = setup();

    char typed_char;
    int no_of_chars_typed{0}, correct_chars{0}, correct_spaces{0};

    // time of the game begin
    auto game_start_time = chrono::steady_clock::now();

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

    // time of the game finish
    auto game_end_time = chrono::steady_clock::now();
    // calculates time of the game
    auto duration = chrono::duration_cast<chrono::seconds>(game_end_time - game_start_time);

    int minutes = duration.count() / 60;
    double seconds = duration.count() - minutes * 60;
    double totalSeconds = chrono::duration_cast<chrono::seconds>(game_end_time-game_start_time).count();

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
         << round((correct_chars + correct_spaces) * (60 / totalSeconds) / 5.0)
         << "  Accuracy: " << ((correct_chars * 100) / word_length) << '%' << endl
         << "Elapsed time: " << setw(2) << setfill('0') << minutes << ':' << setw(2) << setfill('0') << seconds << 's';
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

string get_input()
{
    system("cls");
    cout << "Enter text:" << endl
         << endl;

    string line, text;
    // takes input as long as user doesnt submit an empty line
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
int count_words(const string &words)
{
    int counter{1};
    for (auto ch : words)
    {
        if (ch == ' ' || ch == enter)
            counter++;
    }
    return counter;
}
