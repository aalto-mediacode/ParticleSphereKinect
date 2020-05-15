#pragma once

#include "ofMain.h"

class Particle {
    
public:
    void setup();
    void update(int i);
    void draw(int i, int MODE);
    
    void addNoise(int i, int distC);
    void revolve(int i, int distC);
    void attractedTo(glm::vec3 attractor, float G);
    void repel(Particle p);
    
    float lambda, u, phi;
    
    glm::vec3 pos;
    glm::vec3 origin;
    glm::vec3 nOffset;
    
    int angleX, angleY, angleZ;
    bool isClose;
    float motionScalar;
    float inc;
};
