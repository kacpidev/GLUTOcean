#pragma once
#include <stdlib.h>
#include <GL\glut.h>
#include <chrono>
#include <thread>
#include <string>
#include <iostream>

namespace Scene
{
    void init(int width, int height, std::string name, int argc, char **argv);
    void launch();
    void draw();
    void drawFps();
    void drawOcean();
    void actionFps();
    void reshape(int width, int height);
    void setFPS(int fps);
};