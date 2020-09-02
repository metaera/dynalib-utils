/*
 * Position.h
 *
 *  Created on: 2/10/2017
 *      Author: ken
 */

#ifndef POSITION_H_
#define POSITION_H_

enum Orientation {ORIENT_LEFT=0, ORIENT_RIGHT};

struct Position;

struct Polar {
    double elevation;
    double azimuth;
    long   distance;

    Polar();
    Polar(double elevation, double azimuth, long distance);
    Polar(long xPos, long yPos, long zPos);
    explicit Polar(Position* pos);
};

struct Position {
    long xPos;
    long yPos;
    long zPos;

    static Position* fromPolar(Position* pos, double elevationAngle, double azimuthAngle, long radialDistance);
    static Polar* toPolar(Polar* polar, long xPos, long yPos, long zPos);

    Position();
    Position(long xPos, long yPos, long zPos);
    Position(double elevation, double azimuth, long distance);
    explicit Position(Polar* polar);
};

struct Moving {
    long xDelta;
    long yDelta;
    long zDelta;

    Moving() : xDelta(0), yDelta(0), zDelta(0) {
    }

    Moving(long xDelta, long yDelta, long zDelta) :
        xDelta(xDelta), yDelta(yDelta), zDelta(zDelta) {
    }
};

#endif /* POSITION_H_ */
