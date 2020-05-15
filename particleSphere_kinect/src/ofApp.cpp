#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    bgColor = ofColor(40,20,20,100);
    ofBackground(bgColor);
    ofSetBackgroundAuto(false);
    
    //--- SETUP KINECT
    ofSetLogLevel(OF_LOG_VERBOSE);
    kinect.setRegistration(true); // enable depth->video image calibration
    kinect.init(false, false); // disable video image (faster fps)
    kinect.open();  // opens first available kinect
    kinect.setDepthClipping(500, 1200); // sensing only between 50 and 120 cm from the sensor
    
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    nearThreshold = 230;
    farThreshold = 180;
    bThreshWithOpenCV = true;
    
    
    //--- SETUP FBO
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    center = {0.0, 0.0, 0.0};
    sphere.setResolution(20);
    sphere.setPosition(center);
    sphere.setRadius(300);

    G = 1;
    GOut = 1;
    
    MODE = 1;

    // Create particles
    numPoints = 800;
    for (int i=0; i<numPoints; i++){
        particles.push_back(particle);
        particles[i].setup();
    }

}

//--------------------------------------------------------------
void ofApp::update(){

    //---GET DATA FROM KINECT
    kinect.update();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        grayImage.setFromPixels(kinect.getDepthPixels());  // load grayscale depth image from the kinect
        
        // THRESHOLDS
        // two thresholds - one for the far plane and one for the near plane
        // cvAnd gets the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {
            // or we do it ourselves - work manually with the pixels
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }
        // some effects to fight depth camera noise
        grayImage.dilate();
        grayImage.blurGaussian(3);
        grayImage.erode();
        
        // update the cv image
        grayImage.flagImageChanged();
        
        // detect contour areas between 40 pixels and 1/2 screen size. Find max 2 blobs(hands), not detecting holes
        contourFinder.findContours(grayImage, 40, kinect.width * kinect.height /2, 2, false);
    }
    
    //---BLOB DETECTION
    vector<ofxCvBlob>& blobs = contourFinder.blobs;
    int numBlobs = contourFinder.nBlobs;
    ofLog() << "Number of blobs detected " << numBlobs;
    
    if (numBlobs > 1) { // If we see two blobs(hands), calculate distance between their centerpoints
        glm::vec2 blobCentroid1 = blobs[0].centroid;
        glm::vec2 blobCentroid2 = blobs[1].centroid;
        float dist = distance(blobCentroid1, blobCentroid2);
        ofLog() << "Distance between blobs " << dist;
        
        //G = ofMap(dist, 100, 400, 1000, 0);
        //GOut = ofMap(dist, 400, 100, 1000, 0);
    }
    
    G = ofMap(mouseX, 0, ofGetWidth(), 800, 0);
    GOut = ofMap(mouseY, 0, ofGetHeight(), 800, 0);
    
    //---DRAW PARTICLES IN FBO
    fbo.begin();
    ofClear(0);
    ofBackground(bgColor);
    easyCam.begin();

    // Calculate one particle's distance to center (we don't need the distances from all of them)
    distC = distance(particles[0].pos, center);
    ofLog() << "Particle dist from center " << distC;
    
    // Calculate particles' distances to each other
    for(int i=0; i<particles.size(); i++){
        float distXY, distYZ;
        for(int j=i+1; j<particles.size(); j++){
            distXY = ofDist(particles[i].pos.x, particles[i].pos.y, particles[j].pos.x, particles[j].pos.y);
            distYZ = ofDist(particles[i].pos.z, particles[i].pos.y, particles[j].pos.z, particles[j].pos.y);
            
            if(distXY <18 && distYZ < 18 && distC > 240){
                particles[i].isClose = true;
                ofSetColor(255, 120);
                ofSetLineWidth(1);
                ofDrawLine(particles[i].pos, particles[j].pos);
                break; // if a close particle found, terminate loop
            }else{
                 particles[i].isClose = false;
            }
        }
        
        //particles[i].update(i);
        //particles[i].revolve(i, distC);
        
        particles[i].addNoise(i, distC);
        if(distC > 70){
            particles[i].attractedTo(center, G);
        }
        particles[i].attractedTo(particles[i].origin, GOut);
        particles[i].draw(i, MODE);
    }
    easyCam.end();
    fbo.end();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.draw(0, 0);
    
    grayImage.draw(0, 0, 640/3, 480/3);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
