#include "Scene.h"
#include "Utilities.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <array>

#define RANGE 5
#define STEP 0.25f
#define __GERSTNER 681
#define GERSTNER_ITERATIONS 2
#define G 9.81f

namespace Scene
{
    int fps, fpsGoal;
    int mainWindow;
    int t, deltaTime;
    int frames{ -1 };
    std::string fpsString;
    time_t sleepAvant(0);

    float omegaZero = 2 * PI / 1000;
    float aZero = 0.4f;
    float phiZero = 0.3f;
    std::array<float, 2> kZero = { 3.0f, 4.0f };

    GLuint texture = 0;

    //void orthogonalStart()
    //{
    //    glMatrixMode(GL_PROJECTION);
    //    glPushMatrix();
    //    glLoadIdentity();
    //    gluOrtho2D(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2);
    //    glMatrixMode(GL_MODELVIEW);
    //}

    //void orthogonalEnd()
    //{
    //    glMatrixMode(GL_PROJECTION);
    //    glPopMatrix();
    //    glMatrixMode(GL_MODELVIEW);
    //}


    //void background()
    //{
    //    glBindTexture(GL_TEXTURE_2D, texture);

    //    orthogonalStart();

    //    // texture width/height
    //    const int iw = 500;
    //    const int ih = 500;

    //    glPushMatrix();
    //    glTranslatef(-iw / 2, -ih / 2, 0);
    //    glBegin(GL_QUADS);
    //    glTexCoord2i(0, 0); glVertex2i(0, 0);
    //    glTexCoord2i(1, 0); glVertex2i(iw, 0);
    //    glTexCoord2i(1, 1); glVertex2i(iw, ih);
    //    glTexCoord2i(0, 1); glVertex2i(0, ih);
    //    glEnd();
    //    glPopMatrix();

    //    orthogonalEnd();
    //}

    //GLuint LoadTexture()
    //{
    //    unsigned char data[] = { 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 255 };

    //    glGenTextures(1, &texture);
    //    glBindTexture(GL_TEXTURE_2D, texture);
    //    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //    //even better quality, but this will do for now.
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //    //to the edge of our shape. 
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //    //Generate the texture
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //    return texture; //return whether it was successful
    //}


    void init(int width, int height, std::string name, int argc, char **argv)
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
        glutInitWindowSize(width, height);
        mainWindow = glutCreateWindow(name.c_str());
        glEnable(GLUT_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_SMOOTH);
        glEnable(GL_BLEND);
        //texture = LoadTexture();
    }

    void launch()
    {
        deltaTime = 0;
        t = glutGet(GLUT_ELAPSED_TIME);

        // Keyboard callbacks.
        glutKeyboardFunc(Keyboard::handlePressedKey);	    // Pressed key handler.
        glutKeyboardUpFunc(Keyboard::handleReleasedKey);   // Released key handler.
        glutSpecialFunc(Keyboard::pressedKeySpecial);		// Pressed special key handler.
        glutSpecialUpFunc(Keyboard::releasedKeySpecial);	// Released special key handler.

        glutMouseFunc(Mouse::mouseButton);
        glutMotionFunc(Mouse::mouseMove);

        glutIgnoreKeyRepeat(GLUT_DEVICE_IGNORE_KEY_REPEAT);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glutReshapeFunc(reshape);
        glutDisplayFunc(draw);
        glutMainLoop();
    }

    float dotProduct(std::array<float, 2> v0, std::array<float, 2> v1)
    {
        return v0[0] * v1[0] + v0[1] * v1[1];
    }

    float magnitude(std::array<float, 2> v)
    {
        return sqrt(v[0] * v[0] + v[1] * v[1]);
    }

    std::array<float, 3> gerstner(float x, float y)
    {

        float t = glutGet(GLUT_ELAPSED_TIME) / 5000.0f;

        std::array<float, 2> v = { x, y };
        std::array<float, 3> xz = { 0.0f, 0.0f, 0.0f };

        xz[0] = v[0];
        xz[1] = 0.0f;
        xz[2] = v[1];

        for (int i = 1; i < 4; ++i)
        {
            float omega = (int)(sqrt(G*(i)) / omegaZero)*omegaZero;
            float a = aZero *pow(0.3f, i);
            std::array<float, 2> k = { kZero[0] * (-i)*0.9, kZero[1] * (i)*0.7 };

            xz[0] -= (k[0] / magnitude(k)) * a * sin(dotProduct(v, k) - omega*t + phiZero*i*i);
            xz[2] -= (k[1] / magnitude(k)) * a * sin(dotProduct(v, k) - omega*t + phiZero*i*i);
            xz[1] += a* cos(dotProduct(v, k) - omega*t + phiZero*i*i);
        }

        return xz;
    }

    void updateCamera()
    {
        if (Camera::DELTA_X || Camera::DELTA_Z || Camera::DELTA_Y)
        {
            Camera::POSITION_X += (Camera::DELTA_X * Camera::DIR_X - Camera::DELTA_Z * Camera::DIR_Z) * 0.1f;
            Camera::POSITION_Y += (Camera::DELTA_X * Camera::DIR_Y - Camera::DELTA_Y) * 0.1f;
            Camera::POSITION_Z += (Camera::DELTA_X * Camera::DIR_Z - Camera::DELTA_Z * Camera::DIR_X) * 0.1f;
        }

        Camera::DISTANCE = std::sqrt(
            std::sqrt(Camera::POSITION_X * Camera::POSITION_X + Camera::POSITION_Y * Camera::POSITION_Y) +
            Camera::POSITION_Z * Camera::POSITION_Z);
    }

    void updateLights()
    {
        GLfloat pointLightPosition[] = { Light::POS_X, Light::POS_Y, Light::POS_Z, 1.0f };
        GLfloat pointLightColor[] = { Light::LIGHT_COL, Light::LIGHT_COL, Light::LIGHT_COL, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, pointLightColor);
        glLightfv(GL_LIGHT0, GL_POSITION, pointLightPosition);
    }

    void draw()
    {
        if (glutGet(GLUT_ELAPSED_TIME) - t >= 1000) actionFps();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glEnable(GL_TEXTURE_2D);

        //background();

        updateCamera();

        gluLookAt(Camera::POSITION_X, Camera::POSITION_Y, Camera::POSITION_Z,
                  Camera::POSITION_X + Camera::DIR_X, Camera::POSITION_Y + Camera::DIR_Y, Camera::POSITION_Z + Camera::DIR_Z,
                  0.0f, 1.0f, 0.0f);
        updateLights();

        drawOcean();
        drawFps();

        glutSwapBuffers();
        glutPostRedisplay();

        std::this_thread::sleep_for(std::chrono::milliseconds(deltaTime));
        frames++;
    }

    void drawFps()
    {
        glMatrixMode(GL_PROJECTION);

        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 640, 0, 480);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glRasterPos2f(550, 460);

        char       buf[15] = "FPS : ";
        const char *p(buf);

        strcat_s(buf, std::to_string(fps).c_str());
        do glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p); while (*(++p));
        //glColor3ub(0, 0, 0);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }


    std::array<float, 3> calulateNormal(std::array<float, 3>  p1, std::array<float, 3>  p2, std::array<float, 3>  p3)
    {
        std::array<float, 3>  u =
        {
            0.0f, 0.0f, 0.0f
        };

        std::array<float, 3> v =
        {
            0.0f, 0.0f, 0.0f
        };

        std::array<float, 3> n =
        {
            0.0f, 0.0f, 0.0f
        };

        for (int i = 0; i < 3; ++i)
        {
            u[i] = p3[i] - p1[i];
            v[i] = p2[i] - p1[i];
        }

        n[0] = u[1] * v[2] - u[2] * v[1];
        n[1] = u[2] * v[0] - u[0] * v[2];
        n[2] = u[0] * v[1] - u[1] * v[0];


        return n;
    }

    std::array<float, 3> calculateVertexNormal(std::array<float, 3> n1, std::array<float, 3> n2, std::array<float, 3> n3, std::array<float, 3> n4)
    {
        std::array<float, 3> n = { 0.0f, 0.0f, 0.0f };

        for (int i = 0; i < 3; ++i)
        {
            n[i] = (n1[i] + n2[i] + n3[i] + n4[i]) / 4;
        }
        return n;
    }

    void drawOcean()
    {
        glPushMatrix();

        glColor3f(0.2f, 0.8f, 0.9f);

        glBegin(GL_TRIANGLES);
        for (float x = -RANGE; x < RANGE; x += STEP)
        {
            for (float y = -RANGE; y < RANGE; y += STEP)
            {
#ifdef __GERSTNER
                std::array<float, 3> p1 = gerstner(x, y);
                std::array<float, 3> p2 = gerstner(x, y + STEP);
                std::array<float, 3> p3 = gerstner(x + STEP, y);
                std::array<float, 3> p4 = gerstner(x + STEP, y + STEP);

                std::array<float, 3> p1A = gerstner(x, y - STEP);
                std::array<float, 3> p1B = gerstner(x - STEP, y);

                std::array<float, 3> p2A = gerstner(x, y + 2 * STEP);
                std::array<float, 3> p2B = gerstner(x - STEP, y + STEP);

                std::array<float, 3> p3A = gerstner(x + 2 * STEP, y);
                std::array<float, 3> p3B = gerstner(x + STEP, y - STEP);

                std::array<float, 3> p4A = gerstner(x + 2 * STEP, y + STEP);
                std::array<float, 3> p4B = gerstner(x + STEP, y + 2 * STEP);

#else
                std::array<float, 3> p1 = { x, 0.0f, y };
                std::array<float, 3> p2 = { x, 0.0f, y + STEP };
                std::array<float, 3> p3 = { x + STEP, 0.0f, y };
                std::array<float, 3> p4 = { x + STEP, 0.0f, y + STEP };
#endif // GRESTNER

                //std::array<float, 3> n1 = calulateNormal(p1, p3, p2);
                //std::array<float, 3> n2 = calulateNormal(p2, p1, p4);


                std::array<float, 3> n1 = calculateVertexNormal(calulateNormal(p1, p3, p2), calulateNormal(p1, p1A, p3), calulateNormal(p1, p1B, p1A), calulateNormal(p1, p2, p1B));
                std::array<float, 3> n2 = calculateVertexNormal(calulateNormal(p2, p1, p4), calulateNormal(p2, p2B, p1), calulateNormal(p2, p2A, p2B), calulateNormal(p2, p4, p2A));
                std::array<float, 3> n3 = calculateVertexNormal(calulateNormal(p3, p4, p1), calulateNormal(p3, p3A, p4), calulateNormal(p3, p3B, p3A), calulateNormal(p3, p1, p3B));
                std::array<float, 3> n4 = calculateVertexNormal(calulateNormal(p4, p2, p3), calulateNormal(p4, p4B, p2), calulateNormal(p4, p4A, p4B), calulateNormal(p4, p3, p4A));


                glNormal3f(n1[0], n1[1], n1[2]);
                glVertex3f(p1[0], p1[1], p1[2]);

                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(p2[0], p2[1], p2[2]);

                glNormal3f(n3[0], n3[1], n3[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(p2[0], p2[1], p2[2]);

                glNormal3f(n3[0], n3[1], n3[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

                glNormal3f(n4[0], n4[1], n4[2]);
                glVertex3f(p4[0], p4[1], p4[2]);
            }
        }
        glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);

        glPopMatrix();
    }

    void actionFps()
    {
        t = glutGet(GLUT_ELAPSED_TIME);
        fps = frames;
        frames = 0;

        sleepAvant = deltaTime;
        deltaTime = (int)(((double)(1.0 / fpsGoal) - (double)(1.0 / fps))*pow(10, 3) + sleepAvant) % 1000;
        printf("%d", deltaTime);
    }

    void reshape(int width, int height)
    {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, float(width) / float(height), 1, 1000);
    }

    void setFPS(int fps)
    {
        Scene::fpsGoal = fps;
    }
}