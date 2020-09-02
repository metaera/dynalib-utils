//
// Created by Ken Kopelson on 8/10/17.
//

#include <iostream>
#include <chrono>
using namespace std;
#include "Stopwatch.h"

Stopwatch::Stopwatch(bool useHighRes) : _useHighRes(useHighRes) {
}

long Stopwatch::_getNow() {
    if (_useHighRes) {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        return t1.time_since_epoch().count();
    }
    else {
        return clock();
    }
}

Stopwatch* Stopwatch::start() {
    _startTime   = _intervalTime = _getNow();
    _stopTime    = -1L;
    _restartTime = -1L;
    _totalPaused = 0L;
    _running     = true;
    return this;
}

Stopwatch* Stopwatch::stop() {
    if (_running) {
        _stopTime = _getNow();
        _running  = false;
    }
    return this;
}

Stopwatch* Stopwatch::restart() {
    if (_stopTime != -1L) {
        _restartTime = _intervalTime = _getNow();
        _totalPaused += _restartTime - _stopTime;
        _stopTime    = -1L;
        _running     = true;
    }
    return this;
}

long Stopwatch::getElapsedTime() {
    if (_startTime == -1L)
        return 0L;
    if (_running)
        return _getNow() - _startTime - _totalPaused;
    else
        return _stopTime - _startTime - _totalPaused;
}

long Stopwatch::getLastInterval() {
    if (_startTime == -1L)
        return 0L;
    if (_running) {
        long curTime  = _getNow();
        long interval = curTime - _intervalTime;
        _intervalTime = curTime;
        return interval;
    }
    else
        return _stopTime - _intervalTime;
}

Stopwatch* Stopwatch::reset() {
    _startTime    = -1L;
    _stopTime     = -1L;
    _intervalTime = -1L;
    _restartTime  = -1L;
    _totalPaused  = 0L;
    if (_running)
        start();
    return this;
}

string Stopwatch::toString(string msg) {
    long   interval = getLastInterval();
    long   paused   = getTotalPaused();
    string unitStr  = _useHighRes ? "ns" : (CLOCKS_PER_SEC == 1000000 ? "us" : "ms");
    string outStr = "";
    outStr += "Elapsed: " + to_string(getElapsedTime()) + unitStr;
    outStr += " (Interval: " + to_string(interval) + unitStr;
    if (paused > 0L)
        outStr += ", Paused: " + to_string(paused) + unitStr;
    outStr += ")";
    if (msg != "")
        outStr += ": " + msg;
    return outStr;
}

string Stopwatch::toString() {
    return toString("");
}

void Stopwatch::printMessage(string msg) {
    cout << toString(msg) << endl;
}

void Stopwatch::printMessage() {
    return printMessage("");
}
