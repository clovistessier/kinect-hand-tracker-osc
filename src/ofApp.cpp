#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    // enable depth->video image calibration
    kinect.setRegistration(true);

    kinect.init();
    //kinect.init(true);                // shows infrared instead of RGB video image
    //kinect.init(false, false);        // disable video image (faster fps)

    kinect.open(); // opens first available kinect
    //kinect.open(1);	                // open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00366902078051A");  // open a kinect using it's unique serial #

    // print the intrinsic IR sensor values
    if (kinect.isConnected())
    {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist: " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }

    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    grayThreshRes.allocate(kinect.width, kinect.height);

    ofSetFrameRate(60);

    cameraAngle.addListener(this, &ofApp::cameraAngleChanged);

    // give ourselves some sliders to adjust the CV params
    gui.setup();

    gui.add(cameraAngle.set("camera angle", 0, -30, 30));
    gui.add(nearThreshold.set("near threshold", 255, 0, 255));
    gui.add(farThreshold.set("far threshold", 200, 0, 255));
    gui.add(minBlobArea.set("min blob area", 1000, 1, (kinect.width * kinect.height / 2)));
    gui.add(maxBlobArea.set("max blob area", 10000, 1, (kinect.width * kinect.height)));
    gui.add(nBlobsConsidered.set("blobs to look for", 2, 1, 10));

    sender.setup("localhost", SEND_PORT);

    // start off by drawing the gui
    bDrawGui = true;
    left.setup(0.25, 0.5, 0.5, ofColor::red);
    right.setup(0.75, 0.5, 0.5, ofColor::blue);
}

//--------------------------------------------------------------
void ofApp::update()
{
    kinect.update();

    // there is a new frame and we are connected
    if (kinect.isFrameNew())
    {

        // load grayscale image from the kinect source
        ofPixels depthPixels = kinect.getDepthPixels();
        depthPixels.mirror(false, true);

        grayImage.setFromPixels(depthPixels);

        // we do two thresholds - for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold.get(), true);
        grayThreshFar.threshold(farThreshold.get());
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayThreshRes.getCvImage(), NULL);

        // update the cv images
        grayImage.flagImageChanged();
        grayThreshRes.flagImageChanged();

        // find the countours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // don't find holes because hands shouldn't have interior contours
        contourFinder.findContours(grayThreshRes, minBlobArea.get(), maxBlobArea.get(), nBlobsConsidered.get(), false);

        //store all the hand positions we found, normalized 0-1
        hands.clear();
        for (auto bit = contourFinder.blobs.begin(); bit != contourFinder.blobs.end(); bit++)
        {
            ofDefaultVec3 hand;
            float x = bit->centroid.x;
            float y = bit->centroid.y;
            float depth = depthPixels.getColor(x, y).getBrightness();
            hand.x = ofMap(x, 0, contourFinder.getWidth(), 0.0f, 1.0f, true);
            hand.y = ofMap(y, 0, contourFinder.getHeight(), 0.0f, 1.0f, true);
            hand.z = ofMap(depth, farThreshold.get(), nearThreshold.get(), 0.0f, 1.0f, true);
            hands.push_back(hand);
        }

        int nPrevMatched = static_cast<int>(left.matched) + static_cast<int>(right.matched);

        if (nPrevMatched == 0)
        {
            // match them to left and right
            for (vector<ofDefaultVec3>::iterator itr = hands.begin(); itr != hands.end(); itr++)
            {
                if (itr->x > 0.5)
                {
                    right.update(*itr);
                }
                else if (itr->x < 0.5)
                {
                    left.update(*itr);
                }
            }
        }
        else if (nPrevMatched == 1)
        {
            Hand *prev;
            Hand *other;
            if (left.matched)
            {
                prev = &left;
                other = &right
            }
            else
            {
                prev = &right;
                other = &left;
            }
            
            vector<ofDefaultVec3>::iterator next;
            float recordDistance = 10.0;
            for (vector<ofDefaultVec3>::iterator itr = hands.begin(); itr != hands.end(); itr++)
            {
                float distSq = ofDistSquared(prev->pos.x, prev->pos.y, prev->pos.z, itr->x, itr->y, itr->z);
                if (distSq < recordDistance)
                {
                    next = itr;
                    recordDistance = distSq;
                }
            }
            prev->update(*next);
            hands.erase(next);
            other->update(*(hands.begin()));

            prev = nullptr;
            other = nullptr;
            next = nullptr;


        }
    }
    ofxOscMessage m;

    if (right.changed && right.valid)
    {
        m.setAddress("/kinect/right");
        m.addFloatArg(right.pos.x);
        m.addFloatArg(right.pos.y);
        m.addFloatArg(right.pos.z);
        sender.sendMessage(m, false);
        right.changed = false;
    }
    if (left.changed && left.valid)
    {
        m.clear();
        m.setAddress("/kinect/left");
        m.addFloatArg(left.pos.x);
        m.addFloatArg(left.pos.y);
        m.addFloatArg(left.pos.z);
        sender.sendMessage(m, false);
        left.changed = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // draw from the live kinect
    // kinect.drawDepth(10, 10, 400, 300);
    // kinect.draw(420, 10, 400, 300);

    // grayImage.draw(10, 320, 400, 300);
    // contourFinder.draw(10, 320, 400, 300);

    ofSetColor(255, 255, 255);

    kinectRGB = kinect.getPixels();
    kinectRGB.mirror(false, true);
    kinectRGB.draw(0, 0, 640, 480);

    grayImage.draw(0, 480, 640, 480);

    grayThreshRes.draw(0, 960, 640, 480);

    ofSetColor(255, 0, 0);
    ofNoFill();
    contourFinder.draw(0, 960, 640, 480);

    // ofSetColor(255, 0, 0);
    // float rRad = ofMap(rh.z, farThreshold.get(), nearThreshold.get(), 5.0, 20.0, true);
    // ofDrawCircle(rh.x, rh.y, rRad);

    // ofSetColor(0, 0, 255);
    // float lRad = ofMap(lh.z, farThreshold.get(), nearThreshold.get(), 5.0, 20.0, true);
    // ofDrawCircle(lh.x, lh.y, lRad);

    left.draw(0, 0, 640, 480);
    right.draw(0, 0, 640, 480);
    left.draw(0, 960, 640, 480);
    right.draw(0, 960, 640, 480);

    if (bDrawGui)
    {
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
    cameraAngle = 0;
    kinect.setCameraTiltAngle(0);
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
    case 'g':
        bDrawGui = !bDrawGui;
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp::cameraAngleChanged(int &cameraAngle)
{
    // clamp the angle between -30 and 30 degrees
    int new_angle = min(30, max(cameraAngle, -30));
    kinect.setCameraTiltAngle(new_angle);
}
