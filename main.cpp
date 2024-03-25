#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility>   // for pair
#include <windows.h> //for the Beep() sound
using namespace std;

// defines red and green colors for the ANSI escape sequences
#define red 31
#define green 32

// a vector of pairs to store the characters with their corresponding condition
vector<pair<char, int>> word;

void main_menu();
void start_game();
void mini_menu();

// prints the current word progress
void print(vector<pair<char, int>> &testTxt)
{
    system("cls"); // clears the screen
    for (auto pr : testTxt)
        cout << "\033[1;" << pr.second << "m"
             << (pr.first == ' ' ? '_' : pr.first) << "\033[0m";
}

// sets up the game text according to the parameter
int setup(string &txt)
{
    system("cls");
    word.clear();

    for (auto &ch : txt)
        word.emplace_back(ch, 0);

    print(word); // prints the first time to know what to type

    return (int)txt.length(); // returns the length of the text
}

void logic(string txt)
{
    int word_length = setup(txt);

    char typed_char;
    int no_of_chars_typed{0}, correct_chars{0};

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
            break;
        case 27: // exits on escape
            exit(0);
        default:
            cout <<endl<< "Invalid choice" << endl
                 << "enter choice again" << endl;
            goto input;
    }
}

void mini_menu()
{
    cout << endl
         << endl
         << "[1] Play again" << endl
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
        if (kbhit()) // checks for any buttons pressed to start the game
        {
            getch(); // clears the pressed button from the input buffer
            logic("hello world");
            break;
        }
}
