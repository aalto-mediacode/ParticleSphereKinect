#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "Particle.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxKinect kinect;
    vector<ofPolyline> kinectPolylines;
    vector<ofPolyline> polylines, smoothed, resampled;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // near thresholded image
    ofxCvGrayscaleImage grayThreshFar; //  far thresholded image
    bool bThreshWithOpenCV;
    int nearThreshold;
    int farThreshold;
    ofxCvContourFinder contourFinder;

    ofEasyCam easyCam;
    ofFbo fbo;
    ofColor bgColor;

    vector<Particle> particles;
    Particle particle;
    
    ofSpherePrimitive sphere;
    vector<glm::vec3> spherePoints;
    int numPoints;
    
    glm::vec3 center;
    float G, GOut; // Gravitational pull towards center / towards sphere
    float distC; // Distance from center
    
    int MODE;
};
