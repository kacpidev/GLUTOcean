#include "Scene.h"
#include <random>
#include <time.h>


int main(int argc, char **argv)
{
    srand(time(NULL));

    Scene::init(WIDTH, HEIGHT, "OpenGL GLUT Ocean", argc, argv);
    Scene::setFPS(60);
    Scene::launch();

    return 0;
}