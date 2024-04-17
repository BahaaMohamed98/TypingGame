#include <iostream>
#include <conio.h> //for kbhit()
#include <vector>
#include <utility> // for pair
#include <iomanip> // for output manipulation
#include <chrono>  // for time calculation
#include <thread>  // for multithreading
#include <cmath>
#include <fstream> // for files
#include <sstream> // for string streams
#include <random>  // for random number generation

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

// defines necessary macros
// red and green colors for the ANSI escape sequences
#define red 31
#define green 32
// keyboard buttons for input/output
#define enter (char)13
#define backspace (char)8
#define escape (char)27
#define curOn cout << "\033[?25h"  // shows cursor
#define curOff cout << "\033[?25l" // ANSI escape code to hide cursor
// clearing the screen
#ifdef _WIN32
// For Windows
#define cls system("cls")
#else
// for linux and macOS
#define cls system("clear")
#endif

// a vector of pairs to store the characters with their corresponding condition (color)
vector<pair<char, int>> testText;
vector<string> allGameWords; // will be used to generate a random test
string current_text{"hello world"};
char gameMode = 'r'; // r = random test; i = user input test

// test settings
int wordsInTest = 10;
bool usePunctuation, useCapitalLetters;

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

void readFileText();

void generateTest();

int generateRandomNumber(const int &max);

void openFile();

void deleteFile();

void settings();

void import_settings();

void updateSettings();

// prints the current word progress
void print(vector<pair<char, int>> &gameTxt, int charsTyped, const int &wordLength)
{
    // calculates progress percentage
    int percentDone = ((charsTyped * 100) / wordLength);

    if (percentDone < 0)
        percentDone = 0;
    cls; // clears the screen
    for (auto pr : gameTxt)
    {
        cout << "\033[" << pr.second << "m" << flush;

        switch (pr.first)
        {
        case enter:
            cout << "->"
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
    // prints progress bar
    cout << endl
         << endl
         << "progress: [" << flush;

    for (int i = 0; i < percentDone; i += 10)
        cout << '#' << flush;

    cout << '>' << flush;

    for (int i = 100 - percentDone; i > 0; i -= 10)
        cout << '.' << flush;

    cout << "] " << percentDone << '%' << endl
         << flush;
}

// sets up the game text according to the parameter
int setup(const string &txt = current_text, int mode = 1) // mode = 1 prints the word, else = it doesn't
{
    testText.clear();

    for (auto &ch : txt)
        testText.emplace_back(ch, 0);

    testText.shrink_to_fit(); // reduces the vector's capacity to fit its size

    if (mode == 1)
        print(testText, 0, (int)current_text.length()); // prints the first time to know what to type

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
         << "WPM: " << calculate_wpm(time, correct_chars, correct_spaces) << "  |  "
         << "Accuracy: " << calculateAccuracy(correct_chars, incorrect_chars) << endl;
    time.printTime();
    mini_menu();
}

void wrapperMain()
{
    readFileText();
    generateTest();
}

int main()
{
    import_settings();
    thread testGen(wrapperMain);

    curOff; // ANSI escape code to hide cursor
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

[esc] Exit)"
         << endl;

    switch (getch())
    {
    case '1':
        if (gameMode == 'r')
            generateTest();
        start_game();
        break;
    case '2':
        displayInstructions();
        break;
    case '3':
        setup(import_text(), 2);
        mini_menu(4);
        break;
    case '4':
        settings();
        break;
    case escape: // exits on escape
        exit(0);
    default:
        cout << endl
             << endl
             << "Invalid choice" << endl
             << "Enter choice again" << endl;
        this_thread::sleep_for(1s);
        main_menu();
    }
}

//[1] -> mode 1 = "Next test"; else = "start game";
void mini_menu(int mode)
{
    cout << endl
         << endl
         << (mode == 1 ? (gameMode == 'r' ? "[1] Next test" : "[1] Random test") : "[1] Start game") << endl
         << (mode == 1 ? "[2] Restart test" : (mode == 4 ? "[esc] Main menu" : "[2] Start random test")) << endl
         << (mode == 1 || mode == 2 ? "\n[esc] Main menu" : "") << endl;

    switch (getch())
    {
    case '1':
    {
        if (mode == 1)
            gameMode = 'r';
        if (mode == 1 || mode == 3)
            generateTest();
        start_game();
        break;
    case escape:
        main_menu();
        break;
    }
    case '2':
    {
        if (mode == 1)
        {
            start_game();
            break;
        }

        else if (mode != 4)
        {
            mode = 'i';
            generateTest();
            start_game();
            break;
        }
        // mode = 4 it will goto escape
    }
    //  Intentional fall through
    default:
        cout << endl
             << "Invalid choice!" << endl
             << "Enter choice again" << endl;
        this_thread::sleep_for(1s);
        cls;
        mini_menu(mode); // goes back to take input again
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
    bool fileOpen{true};
    ifstream fin;
    char mode;
    cls;
    cout << "[1] Import from console" << endl
         << "[2] import from file" << endl
         << endl
         << "[esc] Main menu" << endl
         << endl;

    switch (getch())
    {
    case '1':
        mode = 'c';
        curOn; // shows cursor
        break;
    case '2':
    {
    fInput:
        mode = 'f';
        {
            ofstream tmp("input.txt");
            tmp.close();
        }

        fin.open("input.txt", ios::in);

        if (!fin.is_open())
        {
            cout << "failed to open file : input.txt";
            exit(-1);
        }
        cls;

        cout << "A file will appear, paste text into it, save, and then confirm" << endl
             << "[make sure to save it]";
        cout << endl
             << endl
             << "[1] Confirm" << endl
             << "[2] Cancel" << endl
             << endl;

        if (fileOpen)
            openFile();
        fileOpen = true;

        auto choice = getch();
        if (choice == '2')
        {
            cls;
            cout << "Operation cancelled!";
            fin.close();
            deleteFile();
            mini_menu(4);
        }
        else if (choice != '1')
        {
            cout << "Invalid choice" << endl
                 << "Enter choice again" << endl;
            this_thread::sleep_for(1s);
            cls;
            fileOpen = false;
            goto fInput; // goes back to take input again
        }
        break;
    }
    case escape:
        main_menu();
        break;
    default:
        cout << "Invalid choice" << endl
             << "Enter choice again" << endl;
        this_thread::sleep_for(1s);
        import_text(); // goes back to take input again
    }
cInput:
    if (mode == 'c')
    {
        cls;
        cout << "Enter text [Double enter to insert]:" << endl
             << endl;
    }

    string line, text;
    // takes input as long as user doesn't submit an empty line
    while (getline((mode == 'c' ? cin : fin), line) && !line.empty())
    {
        remove_duplicate_spaces(line);
        text.append(line + enter);
    }

    if (mode == 'f')
        fin.close();

    if (text.empty())
    {
        curOff;
        cout << endl
             << "Input cannot be empty!";
        this_thread::sleep_for(1s);
        cls;
        if (mode == 'c')
        {
            curOn;
            goto cInput;
        }
        else
        {
            fin.close();
            deleteFile();
            goto fInput;
        }
    }

    if (mode == 'f')
        deleteFile();

    text.pop_back();
    if (text.back() == ' ')
        text.pop_back();

    curOff; // hides cursor again
    cls;
    cout << "imported successfully!";
    gameMode = 'i';
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

            if ((typed_char == backspace) && (typedChars >= 0)) // backspace functionality
            {
                testText[--index].second = 0; // changes the character back to normal color
                typedChars--;
                print(testText, typedChars, word_length);
                continue; // skips the rest of the iteration
            }

            if (typed_char == testText[index].first)
            {
                // colors the current character green if it is correct
                testText[index].second = green;
                correct_chars++; // increments the count of the correct characters typed

                if (testText[index].first == ' ' || testText[index].first == enter)
                    correct_spaces++;
                typedChars++;
            }
            else
            {
                testText[index].second = red; // colors the character red on mistakes
                incorrect_chars++;            // increments the count of the incorrect characters typed
                typedChars++;
            }

            print(testText, typedChars, word_length); // updates the screen and the test's visual cues
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
7. Press Escape to stop the game.

Have fun while improving your typing skills!)";

    mini_menu(4);
}

string calculateAccuracy(int &correct_chars, int &incorrect_chars)
{
    if (correct_chars == 0)
        return "0%";
    return to_string((int)round((correct_chars * 100.0) / (correct_chars + incorrect_chars))) + '%';
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

// opens the words file to store the file's contents in a vector
void readFileText()
{
    allGameWords.clear(); // clears the global vector to remove unwanted old words

    ifstream fin(R"(testGeneration\words.txt)", ios::in); // specifies read only

    if (!fin.is_open()) // checks if the file had no problems opening
    {
        cls;
        cout << "Failed to open file";
        exit(-1);
    }
    string word;

    while (fin >> word) // takes input till reaching the end of the file
    {
        if (!usePunctuation)              // removes trailing punctuation
            while (!isalpha(word.back())) // removes the last character if not an alphabetic character
                word.pop_back();

        if (!useCapitalLetters) // converts leading capital letters into lowercase letters
            word.front() = (char)tolower(word.front());

        allGameWords.push_back(word);
    }

    fin.close();                  // closes the file
    allGameWords.shrink_to_fit(); // removes the vector's extra capacity
}

void generateTest()
{
    int testWordsCount = (int)allGameWords.size();

    if (testWordsCount < 1)
    {
        cout << "failed to generate test";
        exit(-1);
    }
    string testTxt;

    // makes a string stream to output to it like stdout
    ostringstream oss(testTxt); // will be used to store game test's text

    if (!allGameWords.empty())
    {
        if (wordsInTest < 1)
        {
            cout << "words in test less than 1";
            exit(-44);
        }

        for (int i = 0, wordCounter = 0; i < wordsInTest - 1; i++)
        {
            wordCounter++;
            oss << allGameWords[generateRandomNumber(testWordsCount)];
            if (wordCounter == 6) // inserts a newline after specified number of words
            {
                oss << enter;
                wordCounter = 0;
            }
            else
                oss << ' ';
        }
        oss << allGameWords[generateRandomNumber(testWordsCount)];
        current_text = oss.str();
    }
    else // vector of all words is empty; cannot generate any test
    {
        cout << "Failed: gametxt is empty";
        exit(0);
    }
}

// generates a random number in a given range
int generateRandomNumber(const int &max)
{
    std::random_device rd;                               // Obtain a random number from hardware
    std::mt19937 gen(rd());                              // Seed the generator
    uniform_int_distribution<> distribution(0, max - 1); // Define the range
    return distribution(gen);
}

void openFile()
{
    // opening the file for user on specific OS
#ifdef _WIN32
    // For Windows
    system("start input.txt");
#elif __linux__
    // For Linux
    system("xdg-open input.txt");
#elif __APPLE__
    // For macOS
    system("open input.txt");
#else
    // Unsupported platform
    std::cerr << "Unsupported platform." << std::endl;
#endif
}

void deleteFile()
{
    if (remove("input.txt") != 0)
        cout << endl
             << "failed to remove file : input.txt" << endl
             << endl;
}

void settings()
{
    bool again = true;
    curOff;
    cls;
    int inputNumber;
    string inputStr;
    char mode = 'c';

    cout << "Settings:" << endl
         << endl
         << "[1] Number of words in the test: " << wordsInTest << endl
         << "[2] punctuation: " << (usePunctuation ? "yes" : "no") << endl
         << "[3] capital letters: " << (useCapitalLetters ? "yes" : "no") << endl
         << endl
         << "[esc] Main menu";

    switch (getch())
    {
    case '1':
    input:
        cls;
        cout << "Enter words number per test [1-100]: ";
        cin >> inputNumber;

        if (inputNumber < 1 || inputNumber > 100)
        {
            curOff;
            cout << endl
                 << "Invalid value!";
            this_thread::sleep_for(1s);
            cls;
            goto input;
        }
        curOff;
        wordsInTest = inputNumber;
        break;
    case '2':
        mode = 'p'; // Intentional fall through
    case '3':
    {
        cls;
        curOn;
        cout << "Include " << (mode == 'c' ? "Capital letters" : "Punctuation marks") << "? [y/n]: ";
        cin >> inputStr;
        (mode == 'c' ? useCapitalLetters : usePunctuation) = (inputStr[0] == 'y' || inputStr[0] == 'Y');
        curOff;
        break;
    }
    case escape:
    {
        again = false;
        thread readFile(readFileText);
        readFile.join();
        main_menu();
        break;
    }
    default:
        cout << endl
             << endl
             << "Invalid choice" << endl
             << "Enter choice again" << endl;
        this_thread::sleep_for(1s);
        cls;
        settings();
    }
    updateSettings();
    cout << endl
         << endl
         << "Setting updated successfully!";
    this_thread::sleep_for(1s);
    cls;
    if (again)
        settings();
}

void import_settings()
{
    ifstream fin(R"(testGeneration\settings.txt)", ios::in);

    if (!fin.is_open())
    {
        cout << "failed to open file : settings.txt";
        exit(7);
    }

    fin >> wordsInTest >> usePunctuation >> useCapitalLetters;
    fin.close();
}

void updateSettings()
{
    ofstream fout(R"(testGeneration\settings.txt)", ios::trunc);

    if (!fout.is_open())
    {
        cout << "failed to open file : settings.txt";
        exit(7);
    }

    fout << wordsInTest << ' ' << useCapitalLetters << ' ' << usePunctuation;
    fout.close();
}
