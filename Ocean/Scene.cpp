#include "Scene.h"
#include "Utilities.h"
#include "Mouse.h"
#include "Keyboard.h"
#include <stdio.h>
#include <array>

#define RANGE 4
#define STEP 0.5f
#define DRAW 2
#define __GERSTNER 681
#define LINES 0
#define SMOOTH 1
#define GERSTNER_ITERATIONS 5
#define G 9.81f

namespace Scene
{
    int fps, fpsGoal;
    int mainWindow;
    int t, deltaTime;
    int frames{ -1 };
    std::string fpsString;
    time_t sleepAvant(0);

    float currentTime;

    float omegaZero = 2 * PI / 1000;
    float aZero = 0.4f;
    float phiZero = 0.3f;


    GLuint LoadTexture(const char * filename)
    {

        GLuint texture;

        int width, height;

        unsigned char * data;

        FILE * file;
        const char * streamPointer;
        fopen_s(&file, filename, "rb");

        if (file == NULL) return 0;
        width = 1024;
        height = 1024;
        data = (unsigned char *)malloc(width * height * 3);
        //int size = fseek(file,);
        fread(data, width * height * 3, 1, file);
        fclose(file);

        for (int i = 0; i < width * height; ++i)
        {
            int index = i * 3;
            unsigned char B, R;
            B = data[index];
            R = data[index + 2];

            data[index] = R;
            data[index + 2] = B;

        }


        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
        free(data);

        return texture;
    }
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

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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
        float t = currentTime;
        std::array<float, 2> v = { x, y };
        std::array<float, 3> xz = { 0.0f, 0.0f, 0.0f };

        xz[0] = v[0];
        xz[1] = 0.0f;
        xz[2] = v[1];

        for (int i = 1; i < GERSTNER_ITERATIONS + 1; ++i)
        {
            std::array<float, 2> k = { pow(-0.4, i)*Wind::k[0], pow(-0.4, 1 + i)*Wind::k[1] };
            float omega = (int)(sqrt(G*(i)) / omegaZero)*omegaZero;
            float a = aZero *pow(0.6f, i);


            xz[0] -= (k[0] / magnitude(k)) * a * sin(dotProduct(v, k) - omega*t + phiZero*i*i);
            xz[2] -= (k[1] / magnitude(k)) * a * sin(dotProduct(v, k) - omega*t + phiZero*i*i);
            xz[1] += a* cos(dotProduct(v, k) - omega*t + phiZero*i*i);
        }

        return xz;
    }

    void updateWind()
    {
        Wind::rot += Wind::deltaRot;
        Wind::k[0] = LENGTH * sin(Wind::rot);
        Wind::k[1] = LENGTH * cos(Wind::rot);
    }

    void updateCamera()
    {
        if (Camera::DELTA_X || Camera::DELTA_Z || Camera::DELTA_Y)
        {
            Camera::POSITION_X += (Camera::DELTA_X * Camera::DIR_X - Camera::DELTA_Z * Camera::DIR_Z) * 0.1f;
            Camera::POSITION_Y += (Camera::DELTA_X * Camera::DIR_Y - Camera::DELTA_Y) * 0.1f;
            Camera::POSITION_Z += (Camera::DELTA_X * Camera::DIR_Z + Camera::DELTA_Z * Camera::DIR_X) * 0.1f;
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

    void drawSkybox()
    {
        GLint texture = LoadTexture("texture/skybox_texture.bmp");
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, texture);

        //front
        glBegin(GL_QUADS);
        glTexCoord2f(0.5f, 0.25f);  glVertex3f(5.f, -5.f, -5.f);
        glTexCoord2f(0.25f, 0.25f); glVertex3f(-5.f, -5.f, -5.f);
        glTexCoord2f(0.25f, 0.5f);  glVertex3f(-5.f, 5.f, -5.f);
        glTexCoord2f(0.5f, 0.5f);   glVertex3f(5.f, 5.f, -5.f);

        //right

        glTexCoord2f(0.75f, 0.25f); glVertex3f(5.f, -5.f, 5.f);
        glTexCoord2f(0.5f, 0.25f);  glVertex3f(5.f, -5.f, -5.f);
        glTexCoord2f(0.5f, 0.5f);   glVertex3f(5.f, 5.f, -5.f);
        glTexCoord2f(0.75f, 0.5f);  glVertex3f(5.f, 5.f, 5.f);

        //back
        glTexCoord2f(0.2904f, 1.0f); glVertex3f(-5.f, -5.f, 5.f);
        glTexCoord2f(0.539f, 1.0f);  glVertex3f(5.f, -5.f, 5.f);
        glTexCoord2f(0.539f, 0.75f);   glVertex3f(5.f, 5.f, 5.f);
        glTexCoord2f(0.2904f, 0.75f);   glVertex3f(-5.f, 5.f, 5.f);

        //left
        glTexCoord2f(0.05f, 0.25f);   glVertex3f(-5.f, -5.f, 5.f);
        glTexCoord2f(0.25f, 0.25f); glVertex3f(-5.f, -5.f, -5.f);
        glTexCoord2f(0.25f, 0.5f);  glVertex3f(-5.f, 5.f, -5.f);
        glTexCoord2f(0.05f, 0.5f);    glVertex3f(-5.f, 5.f, 5.f);
        glEnd();

        glPopAttrib();
        glPopMatrix();
    }

    void draw()
    {
        if (glutGet(GLUT_ELAPSED_TIME) - t >= 1000) actionFps();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        updateCamera();
        updateWind();
        gluLookAt(0, 0, 0,
                  Camera::DIR_X, Camera::DIR_Y, Camera::DIR_Z,
                  0.0f, 1.0f, 0.0f);

        drawSkybox();

        glTranslatef(-Camera::POSITION_X, -Camera::POSITION_Y, -Camera::POSITION_Z);

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

        char buf[15] = "FPS : ";
        const char *p(buf);

        strcat_s(buf, std::to_string(fps).c_str());
        do glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p); while (*(++p));

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

        currentTime = glutGet(GLUT_ELAPSED_TIME) / 5000.0f;

        glColor3f(0.2f, 0.8f, 0.9f);
#if (DRAW != LINE)
        glBegin(GL_TRIANGLES);
#else
        glBegin(GL_LINE);
#endif
        float step = STEP;

        for (float x = -RANGE; x < RANGE; x += step)
        {
            for (float y = -RANGE; y < RANGE; y += step)
            {
#ifdef __GERSTNER
                std::array<float, 3> p1 = gerstner(x, y);
                std::array<float, 3> p2 = gerstner(x, y + step);
                std::array<float, 3> p3 = gerstner(x + step, y);
                std::array<float, 3> p4 = gerstner(x + step, y + step);

#else
                std::array<float, 3> p1 = { x, 0.0f, y };
                std::array<float, 3> p2 = { x, 0.0f, y + step };
                std::array<float, 3> p3 = { x + step, 0.0f, y };
                std::array<float, 3> p4 = { x + step, 0.0f, y + step };
#endif // GRESTNER

#if (DRAW == SMOOTH)

                std::array<float, 3> n1 = calulateNormal(p1, p3, p2);
                std::array<float, 3> n2 = calulateNormal(p2, p1, p4);

                glNormal3f(n1[0], n1[1], n1[2]);
                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);
                glVertex3f(p4[0], p4[1], p4[2]);

#else
#if (DRAW == LINES)
                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p2[0], p2[1], p2[2]);

                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

                glVertex3f(p4[0], p4[1], p4[2]);
                glVertex3f(p2[0], p2[1], p2[2]);

                glVertex3f(p4[0], p4[1], p4[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

#else
                std::array<float, 3> p1A = gerstner(x, y - step);
                std::array<float, 3> p1B = gerstner(x - step, y);

                std::array<float, 3> p2A = gerstner(x, y + 2 * step);
                std::array<float, 3> p2B = gerstner(x - step, y + step);

                std::array<float, 3> p3A = gerstner(x + 2 * step, y);
                std::array<float, 3> p3B = gerstner(x + step, y - step);

                std::array<float, 3> p4A = gerstner(x + 2 * step, y + step);
                std::array<float, 3> p4B = gerstner(x + step, y + 2 * step);

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
#endif

#endif
            }
        }
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);

        glPopMatrix();
    }

    void actionFps()
    {
        t = glutGet(GLUT_ELAPSED_TIME);
        fps = frames;
        frames = 0;

        sleepAvant = deltaTime;
        deltaTime = (int)(((double)(1.0 / fpsGoal) - (double)(1.0 / fps))*pow(10, 3) + sleepAvant) % 1000;
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