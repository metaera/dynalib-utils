//
// Created by Ken Kopelson on 8/10/17.
//

#include <cmath>
#include "Position.h"

/**
 * Returns a cubic (X,Y,Z) Position for a given set of spherical coordinates
 * @param elevationAngle Vertical angle of the radial extending directly out in front, positive up and
 * negative down.
 * @param azimuthAngle Horizontal angle of the radial extending directly out in front, positive to the right
 * and negative to the left.
 * @param radialDistance Distance of the radial extending directly out in front.
 * @return Pointer to a Position object
 */
Position* Position::fromPolar(Position* pos, double elevationAngle, double azimuthAngle, long radialDistance) {
    double    toRadians   = M_PI / 180.0;
    double    elevRadians = elevationAngle * toRadians;
    double    azRadians   = azimuthAngle * toRadians;
    double    temp        = cos(elevRadians);
    pos->xPos = (long)(round(radialDistance * sin(azRadians) * temp));
    pos->yPos = (long)(round(radialDistance * sin(elevRadians)));
    pos->zPos = (long)(round(radialDistance * cos(azRadians) * temp));
    return pos;
}

/**
 * Returns a spherical Polar object for a given X,Y,Z location in a 3D Cartesian coordinate system.
 * @param xPos
 * @param yPos
 * @param zPos
 * @return Pointer to a Polar object
 */
Polar* Position::toPolar(Polar* pol, long xPos, long yPos, long zPos) {
    double toDegrees = 180.0 / M_PI;
    double xzSqr     = ((double)xPos * (double)xPos) + ((double)zPos * (double)zPos);
    pol->azimuth     = round(atan2((double)xPos, (double)zPos) * toDegrees);
    pol->elevation   = round(atan2((double)yPos, sqrt(xzSqr)) * toDegrees);
    pol->distance    = (long)round(sqrt(xzSqr + ((double)yPos * (double)yPos)));
    return pol;
}

Polar::Polar() : elevation(0.0), azimuth(0.0), distance(0) {
}

Polar::Polar(double elevation, double azimuth, long distance) :
        elevation(elevation), azimuth(azimuth), distance(distance) {
//    if (distance < 1L)
//        this->distance = 1L;
}

Polar::Polar(long xPos, long yPos, long zPos) {
    Polar* polar = Position::toPolar(new Polar(), xPos, yPos, zPos);
    elevation = polar->elevation;
    azimuth   = polar->azimuth;
    distance  = polar->distance;
    delete polar;
}

Polar::Polar(Position* pos) : Polar(pos->xPos, pos->yPos, pos->zPos) {
}

Position::Position() : Position(0L, 0L, 0L) {
}

Position::Position(long xPos, long yPos, long zPos) :
        xPos(xPos), yPos(yPos), zPos(zPos) {
    if (zPos < 1L)
        this->zPos = 1L;
}

Position::Position(double elevation, double azimuth, long distance) {
    Position* pos = Position::fromPolar(new Position(), elevation, azimuth, distance);
    xPos = pos->xPos;
    yPos = pos->yPos;
    zPos = pos->zPos;
    delete pos;
}

Position::Position(Polar* polar) : Position(polar->elevation, polar->azimuth, polar->distance) {
}

