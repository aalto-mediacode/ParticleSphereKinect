#pragma once

#include "ofMain.h"

class Particle {
    
public:
    void setup();
    void addNoise(int i, int distC);
    void attractedTo(glm::vec3 attractor, float G);
    void draw(int i);
    
    float lambda, u, phi;
    
    glm::vec3 pos;
    glm::vec3 origin;
    glm::vec3 nOffset; // noise offset
    
    bool isClose;
};
