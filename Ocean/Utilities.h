namespace Camera
{
    /// Position.     

    static float POSITION_X = 13.0f;      // Camera x position.
    static float POSITION_Y = 30.0f;     // Camera y position.
    static float POSITION_Z = -35.0f;      // Camera z position.
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
    static float ANGLE_Y = 40.f;    // Camera y rotation angle.
    static float DELTA_ANGLE_X = 0.0f;      // Camera delta x rotation.
    static float DELTA_ANGLE_Y = 0.0f;      // Camera delta y rotation.
    static float ROTATION_MODIFIER = 0.2f;  // Camera rotation speed modifier.

    // Distance to point (0, 0 ,0)
    static float DISTANCE = 0.0f;
}

namespace Light
{

    // Border values for linear attenuation.

#define MAX_LINEAR_ATT 0.1f
#define MIN_LINEAR_ATT 0.008f

    /// Light source coordinates.

    static float POS_X = 0.0f;      // Light x position.
    static float POS_Y = 20.0f;     // Light y position.
    static float POS_Z = -50.0f;    // Light z position.

    /// Spot directions.

    static float SPOT_X = 0.0f;     // Spot x direction.
    static float SPOT_Y = -1.0f;    // Spot y direction.
    static float SPOT_Z = 1.0f;     // Spot z direction.

    // Light linear attenuation.
    static float LINEAR_ATT = 0.05f;
}