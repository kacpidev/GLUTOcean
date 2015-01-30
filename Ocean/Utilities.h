#pragma once
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>

#define PI 3.14
#define MAX(a,b) (((a) < (b)) ? (a) : (b))
#define MIN(a,b) (((a) > (b)) ? (a) : (b))

namespace Camera
{
    /// Position.     

    static float POSITION_X = 7.0f;      // Camera x position.
    static float POSITION_Y = 10.0f;     // Camera y position.
    static float POSITION_Z = -15.0f;      // Camera z position.
    static float DELTA_X = 0.0f;
    static float DELTA_Y = 0.0f;
    static float DELTA_Z = 0.0f;
    static float SPEED = 8.f;

    /// Direction.

    static float DIR_X = -0.5f;      // Camera x direction.
    static float DIR_Y = -0.5f;      // Camera y direction.
    static float DIR_Z = 1.0f;     // Camera z direction.

    /// Rotation

    static float ANGLE_X = -150.0f;    // Camera x rotation angle.
    static float ANGLE_Y = 80.f;    // Camera y rotation angle.
    static float DELTA_ANGLE_X = 0.0f;      // Camera delta x rotation.
    static float DELTA_ANGLE_Y = 0.0f;      // Camera delta y rotation.
    static float ROTATION_MODIFIER = 0.2f;  // Camera rotation speed modifier.

    // Distance to point (0, 0 ,0)
    static float DISTANCE = 0.0f;
}

namespace Light
{

    // Border values for light color

    #define MAX_LIGHT_COL 1.0f
    #define MIN_LIGHT_COL 0.1f

    /// Light source coordinates.

    static float POS_X = 0.0f;      // Light x position.
    static float POS_Y = 5.0f;     // Light y position.
    static float POS_Z = -5.0f;    // Light z position.

    // Light color.
    static float LIGHT_COL = 0.5f;
}