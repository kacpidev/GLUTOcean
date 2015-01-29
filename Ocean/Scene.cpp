#include "Scene.h"
#include <array>

#define RANGE 10
#define STEP 0.5f

namespace Scene
{
    int fps, fpsGoal;
    int mainWindow;
    int t, deltaTime;
    int frames{ -1 };
    std::string fpsString;
    time_t sleepAvant(0);

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
    }

    void launch()
    {
        deltaTime = 0;
        t = glutGet(GLUT_ELAPSED_TIME);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glutReshapeFunc(reshape);
        glutDisplayFunc(draw);
        glutMainLoop();
    }

    void draw()
    {
        if (glutGet(GLUT_ELAPSED_TIME) - t >= 1000) actionFps();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        GLfloat pointLightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat pointLightPosition[] = { 1.0f, 0.5f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, pointLightColor);
        glLightfv(GL_LIGHT0, GL_POSITION, pointLightPosition);


        //camera.translation();
        gluLookAt(0.0f, 10.0f, 10.0f,
                  0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f);

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

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    float dotProduct(std::array<float, 2> v0, std::array<float, 2> v1)
    {
        return v0[0] * v1[0] + v0[1] * v1[1];
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

    void drawOcean()
    {
        glPushMatrix();

        glColor3ub(80, 140, 220);

        glBegin(GL_TRIANGLES);
        for (float x = -RANGE; x < RANGE; x += STEP)
        {
            for (float y = -RANGE; y < RANGE; y += STEP)
            {
                std::array<float, 3> p1 = { x, 0.0f, y };
                std::array<float, 3> p2 = { x, 0.0f, y + STEP };
                std::array<float, 3> p3 = { x + STEP, 0.0f, y };
                std::array<float, 3> p4 = { x + STEP, 0.0f, y + STEP };

                std::array<float, 3> n1 = calulateNormal(p1, p3, p2);
                std::array<float, 3> n2 = calulateNormal(p2, p3, p4);

                glNormal3f(n1[0], n1[1], n1[2]);
                glVertex3f(p1[0], p1[1], p1[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);

                glNormal3f(n2[0], n2[1], n2[2]);
                glVertex3f(p2[0], p2[1], p2[2]);
                glVertex3f(p3[0], p3[1], p3[2]);
                glVertex3f(p4[0], p4[1], p4[2]);
            }
        }
        glEnd();

        glColor3ub(255, 255, 255);

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