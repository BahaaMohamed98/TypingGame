#include "gameTime.h"

void gameTime::start()
{
    startTime = std::chrono::steady_clock::now();
}

void gameTime::end()
{
    endTime = std::chrono::steady_clock::now();

    // calculates gameTime of the game
    duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
}

int gameTime::getMinutes()
{
    return (int)duration.count() / 60;
}

double gameTime::getSeconds()
{
    return (int)duration.count() % 60;
}

double gameTime::getTotalTimeInSeconds()
{
    return (int)duration.count();
}