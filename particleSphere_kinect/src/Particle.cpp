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
    pos = p;
    
    angleX = ofRandom(-2,2);
    angleY = ofRandom(-2,2);
    angleZ = ofRandom(-2,2);
    
    isClose = false;
    
}

//--------------------------------------------------------------
void Particle::update(int i){
    
    // Calculate origin position in relation to sphere/ellipse, as a basis position for each frame
    glm::vec3 p;
    p.x = pow(this->lambda, 1/3) * sqrt(1.0 - this->u * this->u) * cos(this->phi) * 200;
    p.y = pow(this->lambda, 1/3) * sqrt(1.0 - this->u * this->u) * sin(this->phi) * 200;
    p.z = pow(this->lambda, 1/3) * u * 200;
    
    pos = p;
    
}

//--------------------------------------------------------------
void Particle::draw(int i, int MODE){
    
    
    if(MODE==1){  // MODE 1 Star constellation
        if (i % 40 == 1) {
            ofSetColor(245,120,24);
            ofDrawSphere(pos, 2);
            
        } else if (i % 75 == 1){
            ofSetColor(255,255,255);
            ofSetLineWidth(3);
            ofDrawLine(pos.x-3, pos.y-1.5, pos.x+3, pos.y+1.5);
            ofDrawLine(pos.x-3, pos.y+1.5, pos.x+3, pos.y-1.5);
            ofDrawLine(pos.x, pos.y-3, pos.x, pos.y+3);
            
        } else {
            ofSetColor(163,218,255);
            ofDrawSphere(pos, 0.7);
        }
        
    }else if(MODE ==2){ // MODE 2 Large blobs
        if (isClose){
            ofSetColor(252,244,190);
            ofDrawSphere(pos, 8);
        } else if (i % 40 == 1) {
            ofSetColor(255, 120, 89);
            ofDrawSphere(pos, 5);
        } else {
            ofSetColor(242, 198, 78);
            ofDrawSphere(pos, 10);
        }
    }
}

//--------------------------------------------------------------
void Particle::addNoise(int i, int distC){
    
    // Separate noise values for each axis
    float offsetX, offsetY, offsetZ;
    
    float time = ofGetElapsedTimef() * 0.4;
    int timestepX = 10 * i;
    int timestepY = 200 * i;
    int timestepZ = 800 * i;
    
    if(distC > 100 && distC < 240){
        // Regular noise setting
        offsetX = ofMap((ofNoise(time, timestepX)), 0, 1, -1, 1); // (Map the noise to have values between -1 and 1)
        offsetY = ofMap((ofNoise(time, timestepY)), 0, 1, -1, 1);
        offsetZ = ofMap((ofNoise(time, timestepZ)), 0, 1, -1, 1);
        
    } else if (distC < 100){
        // Shake!!
        int intensity = ofMap(distC, 0, 100, 80, 0);
        offsetX = ofMap((ofNoise(time * intensity, timestepX)), 0, 1, -1, 1);
        offsetY = ofRandom(-0.5, 0.5);
        offsetZ = ofRandom(-0.5, 0.5);
        
    } else if (distC > 240){
        // Regular noise setting
        int intensity = ofMap(distC, 240, 400, 1, 0);
        offsetX = ofMap((ofNoise(time * intensity, timestepX)), 0, 1, -1, 1);
        offsetY = ofMap((ofNoise(time * intensity, timestepY)), 0, 1, -1, 1);
        offsetZ = ofMap((ofNoise(time * intensity, timestepZ)), 0, 1, -1, 1);
    }
        
    nOffset = {offsetX, offsetY, offsetZ};
    pos += nOffset;
}

//--------------------------------------------------------------
void Particle::revolve(int i, int distC){
    
    float scalar; // For scaling the circular motion
    if(distC > 180 || distC < 100){
        scalar = 0.0;
    }else{
        scalar = 0.5;
    }
    
    // Negative and positive angles -> revolving in different directions
    if(angleX < 0){angleX -= 1;} else {angleX += 1;}
    if(angleY < 0){angleY -= 1;} else {angleY += 1;}
    if(angleZ < 0){angleX -= 1;} else {angleZ += 1;}
    
    pos.x = pos.x + sin(ofDegToRad(this->angleX) + i/10 ) * scalar;
    pos.y = pos.y + cos(ofDegToRad(this->angleY) + i/10 ) * scalar;
    pos.z = pos.z + sin(ofDegToRad(this->angleZ) + i/10) * scalar;
    
}


//--------------------------------------------------------------
void Particle::attractedTo(glm::vec3 attractor, float G){

    glm::vec3 dir = attractor - this->pos; // Get direction between particle and attractor
    glm::normalize(dir);
    
    float dist = glm::distance(this->pos, attractor);
    dist = ofClamp(dist, 240, 800);
    
    //float magnitude = (G * mass * attractor.mass) / (distance * distance);
    float magnitude = G / (dist * dist);
    
    dir *= magnitude;
    this->pos += dir;
    
}

