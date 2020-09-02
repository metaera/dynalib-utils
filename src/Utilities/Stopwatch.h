//
// Created by Ken Kopelson on 8/10/17.
//

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Stopwatch {
    bool _useHighRes   = false;
    long _startTime    = -1;
    long _stopTime     = -1;
    long _restartTime  = -1;
    long _totalPaused  = 0;
    long _intervalTime = -1;
    bool _running      = false;

    long _getNow();

public:
    explicit Stopwatch(bool useHighRes=false);

    Stopwatch* start();
    Stopwatch* stop();
    Stopwatch* restart();
    long getElapsedTime();
    long getLastInterval();
    Stopwatch* reset();

    long getStartTime()     { return _startTime; }
    long getStopTime()      { return _stopTime; }
    long getTotalPaused()   { return _totalPaused; }
    bool isRunning()        { return _running; }

    string toString(string msg);
    string toString();
    void   printMessage(string msg);
    void   printMessage();
};


#endif //STOPWATCH_H
