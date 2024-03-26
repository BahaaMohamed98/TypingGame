#ifndef GAMETIME_H
#define GAMETIME_H

#include <chrono>

class gameTime
{
private:
    std::chrono::steady_clock::time_point startTime, endTime;
    std::chrono::seconds duration;
public:
    void start();
    void end();
    int getMinutes();
    double getSeconds();
    double getTotalTimeInSeconds();
};

#endif //GAMETIME_H