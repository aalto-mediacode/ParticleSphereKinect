#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(30);
    bgColor = ofColor(35,15,15,120);
    ofBackground(bgColor);
    ofSetBackgroundAuto(false);
    
    MODE = 2;
    
    //--- SETUP KINECT
    ofSetLogLevel(OF_LOG_VERBOSE);
    kinect.setRegistration(true); // enable depth->video image calibration
    kinect.init(false, false); // disable video image (faster fps)
    kinect.open();  // opens first available kinect
    kinect.setCameraTiltAngle(4);
    kinect.setDepthClipping(500, 1000); // NOTE: sensing only between 50 and 100 cm from the sensor
    
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    nearThreshold = 255;
    farThreshold = 100;
    bThreshWithOpenCV = true;
    
    
    //--- SETUP FBO
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    center = {0.0, 0.0, 0.0};
    
    G = 1;
    GOut = 1;
    maxG = 1100; // edit this for changing the magnitude of gravitation
    
    angle = 1;
    inc = 0.1;
    
    // Create particles
    for (int i=0; i<NUMPOINTS; i++){
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
        
        // two thresholds: one for the far plane and one for the near plane
        // cvAnd gets the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        }
        
        // some effects to fight depth camera noise
        grayImage.dilate();
        grayImage.blurGaussian(3);
        grayImage.erode();
        
        // update the cv image
        grayImage.flagImageChanged();
        
        // detect contour areas between 100 pixels and 1/2 screen size. Find max 2 blobs(hands), not detecting holes
        contourFinder.findContours(grayImage, 100, kinect.width * kinect.height /2, 2, false);
    }
    
    
    //---BLOB DETECTION & MAPPING
    vector<ofxCvBlob>& blobs = contourFinder.blobs;
    int numBlobs = contourFinder.nBlobs;
    ofLog() << "Number of blobs detected " << numBlobs;
    
    if (numBlobs > 1) { // If we see two blobs(hands), calculate distance between their centerpoints
        glm::vec2 blobCentroid1 = blobs[0].centroid;
        glm::vec2 blobCentroid2 = blobs[1].centroid;
        float dist = distance(blobCentroid1, blobCentroid2);
        ofLog() << "Distance between blobs " << dist;
        
        // Map distance between hands to gravitational pull
        G = ofMap(dist, 80, 400, maxG, 0);
        GOut = ofMap(dist, 400, 80, maxG, 0);
    }
    
    // Uncomment this for testing with mouse instead of kinect
    //G = ofMap(mouseX, 0, ofGetWidth(), maxG, 0);
    //GOut = ofMap(mouseY, 0, ofGetHeight(), maxG, 0);
    
    
    //---DRAW PARTICLES IN FBO
    fbo.begin();
    ofClear(0);
    ofBackground(bgColor);
    
    // Settings for camera rotation
    easyCam.setGlobalPosition(0, 0, -600);
    glm::vec3 axis = {-0.5, 1, 0};
    angle += inc;
    easyCam.rotateAroundDeg(angle, axis, center);
    easyCam.setTarget(center);
    easyCam.begin();
    
    // Calculate one particle's distance to center (saving memory here, don't need the distances from all of them)
    distC = distance(particles[0].pos, center);
    ofLog() << "Particle dist from center " << distC;
    
    // Slow camera rotation when particles are farther away from center
    if (distC > 240){
        inc = 0.08;
    }else{
        inc = 0.15;
    }
    
    for(int i=0; i<particles.size(); i++){
        particles[i].addNoise(i, distC);
        particles[i].attractedTo(particles[i].origin, GOut);
        if(distC > 70){ // Stop attracting to center when 70px away from it
            particles[i].attractedTo(center, G);
        }
        
        // Calculate particles' distances to each other & draw a line between close ones
        float distXY, distYZ;
        for(int j=i+1; j<particles.size(); j++){
            distXY = ofDist(particles[i].pos.x, particles[i].pos.y, particles[j].pos.x, particles[j].pos.y);
            distYZ = ofDist(particles[i].pos.z, particles[i].pos.y, particles[j].pos.z, particles[j].pos.y);
            if(distXY < 18 && distYZ < 18 && distC > 240){
                particles[i].isClose = true;
                int lineOpacity = ofMap(distC, 240, 280, 0, 127, true);
                ofSetColor(255, lineOpacity);
                ofSetLineWidth(1);
                ofDrawLine(particles[i].pos, particles[j].pos);
                break; // if a close particle found, terminate loop
            }else{
                particles[i].isClose = false;
            }
        }
        particles[i].draw(i);
    }
    easyCam.end();
    
    
    //---DRAW FROM KINECT
    //MODE 1: Draw small kinect image in upper left corner
    if(MODE == 1){
        ofSetColor(255);
        grayImage.draw(640/3, 0, -640/3, 480/3);
        
        //MODE 2: Draw blobs in fullscreen size as a polyline overlay
    }else if(MODE == 2){
        kinectPolylines.clear();
        for(int i=0; i<numBlobs; i++) {
            vector<glm::vec3> pts = blobs[i].pts;
            ofPolyline polyline;
            polyline.addVertices(pts);
            polyline.close();
            polyline = polyline.getResampledByCount(polyline.size()/2).getSmoothed(4);
            polyline.scale(-2.625, 2.625); // Scale blobs according to screensize
            kinectPolylines.push_back(polyline);
        }
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        ofSetColor(255, 10);
        ofPushView();
        ofTranslate(ofGetWidth(), 0);
        for(int i=0; i<kinectPolylines.size(); i++){
            kinectPolylines[i].draw();
        }
        ofPopView();
        ofDisableBlendMode();
    }
    fbo.end();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255);
    fbo.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == '1'){
        MODE = 1;
        ofLog() << "MODE 1";
    }else if(key == '2'){
        MODE = 2;
        ofLog() << "MODE 2";
    }
}
