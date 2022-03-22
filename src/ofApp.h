#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "Hand.h"

#define SEND_PORT 8000


class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxKinect kinect;

	ofImage kinectRGB;

	ofxCvGrayscaleImage grayImage;		// grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar;	// the far thresholded image
	ofxCvGrayscaleImage grayThreshRes;

	ofxCvContourFinder contourFinder;

	ofParameter<int> cameraAngle;
	ofParameter<int> nearThreshold;
	ofParameter<int> farThreshold;
	ofParameter<int> minBlobArea;
	ofParameter<int> maxBlobArea;
	ofParameter<int> nBlobsConsidered;
	ofxPanel gui;

	Hand left;
	Hand right;

	vector<ofDefaultVec3> hands;

	ofxOscSender sender;

	bool bDrawGui;

	void cameraAngleChanged(int &cameraAngle);
};
