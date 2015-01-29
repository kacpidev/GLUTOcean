#include "Scene.h"

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

    void drawOcean()
    {
        glPushMatrix();
    }

    void actionFps()
    {
        t = glutGet(GLUT_ELAPSED_TIME);
        fps = frames;
        frames = 0;

        sleepAvant = deltaTime;
        deltaTime = (int)(((double)(1.0/fpsGoal) - (double)(1.0/fps))*pow(10,3) + sleepAvant) % 1000;
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