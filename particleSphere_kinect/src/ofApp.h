#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "Particle.hpp"
#define NUMPOINTS 800

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    ofxKinect kinect;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // far thresholded image
    bool bThreshWithOpenCV;
    int nearThreshold;
    int farThreshold;
    ofxCvContourFinder contourFinder;
    vector<ofPolyline> kinectPolylines;
    
    ofEasyCam easyCam;
    float angle, inc;
    ofFbo fbo;
    ofColor bgColor;
    
    vector<Particle> particles;
    Particle particle;
    
    glm::vec3 center;
    float G, GOut; // Gravitational pull towards center / towards outer sphere
    float maxG; // Max limit for the G values
    
    float distC; // Example distance of one particle from center
    
    int MODE;
};
