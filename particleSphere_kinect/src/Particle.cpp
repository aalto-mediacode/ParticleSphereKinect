#include "Particle.hpp"

//--------------------------------------------------------------
void Particle::setup(){
    
    // Calculate random origin points on sphere
    int radius = 340;
    lambda = ofRandom(1.0f);
    u = ofRandom(-1.0f, 1.0f);
    phi = ofRandom( 2.0 * PI );
    
    glm::vec3 p;
    p.x = pow(lambda, 1/3) * sqrt(1.0 - u * u) * cos(phi) * radius;
    p.y = pow(lambda, 1/3) * sqrt(1.0 - u * u) * sin(phi) * radius;
    p.z = pow(lambda, 1/3) * u * radius;
    
    origin = p;
    pos = origin;
    
    isClose = false;
}

//--------------------------------------------------------------
void Particle::addNoise(int i, int distC){
    
    // Differing noise values for each axis
    float offsetX, offsetY, offsetZ;
    int timestepX = 10 * i;
    int timestepY = 200 * i;
    int timestepZ = 800 * i;
    
    float time = ofGetElapsedTimef() * 0.4; // edit this for changing noise speed
    
    if (distC > 90){
        // Noise intensifies the closer the center is
        int intensity = ofMap(distC, 90, 400, 5, 0.2);
        offsetX = ofMap((ofNoise(time * intensity, timestepX)), 0, 1, -1, 1);
        offsetY = ofMap((ofNoise(time * intensity, timestepY)), 0, 1, -1, 1);
        offsetZ = ofMap((ofNoise(time * intensity, timestepZ)), 0, 1, -1, 1);
    }
    else if (distC < 90){
        // When close to center, get shaky
        int intensity = ofMap(distC, 0, 90, 80, 0.3);
        offsetX = ofMap((ofNoise(time * intensity, timestepX)), 0, 1, -1, 1);
        offsetY = ofRandom(-0.5, 0.5);
        offsetZ = ofRandom(-0.5, 0.5);
    }
    
    nOffset = {offsetX, offsetY, offsetZ};
    pos += nOffset;
}

//--------------------------------------------------------------
void Particle::attractedTo(glm::vec3 attractor, float G){
    
    glm::vec3 dir = attractor - this->pos; // Get direction between particle and attractor
    glm::normalize(dir);
    
    float dist = glm::distance(this->pos, attractor);
    dist = ofClamp(dist, 200, 800);
    
    float magnitude = G / (dist * dist);
    dir *= magnitude;
    this->pos += dir;
}

//--------------------------------------------------------------
void Particle::draw(int i){
    
    // Red particles
    if (i % 40 == 1) {
        ofSetColor(245,100,24);
        ofDrawSphere(pos, 2);
        
        // Whites
    } else if (isClose){
        ofSetColor(255);
        ofDrawSphere(pos, 0.7);
        
        // Light blues
    } else {
        ofSetColor(178,230,255);
        ofDrawSphere(pos, 0.7);
    }
}
