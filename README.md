# kinect-hand-tracker-osc
This openFrameworks application uses the Kinect V1 (model 1414) to track a user's hands and transmits the their positions over OSC for use as a gesture controller in multimedia, audiovisual, and creative coding applications.

## Description
This project extends the openFrameworks kinectExample project to implement the basic hand tracking functionality demonstrated in this video from Dan Shiffman, which provides a great explainer as to what's going on here. Many thanks to him for providing so many great creative coding tutorials.
[![Coding Train: Average Point Hand Tracking](https://img.youtube.com/vi/Kr4s5sLoROY/0.jpg)](https://www.youtube.com/watch?v=Kr4s5sLoROY)

*Click the image above to watch the video*

The Microsoft Kinect camera, originally released for the Xbox 360 in 2010, uses an infrared projector and detector to create a depth image in addition to the RGB image produced by its camera. This depth image can be used to easily isolate objects from the background and track their positions. Using a stationary camera with a single human subject standing a fixed distance away, the subject's hands can be isolated from the rest of the image and tracked in 3 dimensions. These x, y, and z coordinates are sent as OSC messages for use in controlling multimedia or creative coding applications.

This project was developed for fun because I already had an old Kinect sensor laying around. Users looking to add OSC-based gesture control to their projects are encouraged to check out these alternatives, which offer superior skeleton tracking and don't rely on 12 year old sensors whose drivers haven't been supported since 2014.

- If you are using Windows: [ofxKinectForWindows2](https://github.com/elliotwoods/ofxKinectForWindows2) uses the updated Kinect V2 and the Kinect For Windows SDK 2.0
- If you have a LIDAR equipped iPhone or iPad: [ZIG SIM PRO](https://apps.apple.com/us/app/zig-sim-pro/id1481556614) exposes the data from all of the sensors on your device and sends them out of OSC, including skeleton keypoints from ARKIT.
- If you have an Android phone: [PoseHook](https://play.google.com/store/apps/details?id=com.hollyhook.posehook&hl=en_US&gl=US) uses a machine learning model running on your device to perform skeleton tracking and sends out the keypoints over OSC.

## Dependencies
If you are on MacOS, there are known problems with certain Xbox Kinect model numbers and ofxKinect. [See here for more information.](https://github.com/openframeworks/openFrameworks/tree/master/addons/ofxKinect)

All of the ofx addons used in this application are included in the base install of openFrameworks.
- ofxGui
- ofxKinect
- ofxOpenCV
- ofxOsc

## Installation
1. Download [openFrameworks](https://openframeworks.cc/) (tested on 0.11.0 and 0.11.2)
2. clone this repository into apps/myApps of your oF install
3. import with the projectGenerator to update it for your system
4. build and run using your IDE 

## Usage
The following instructions assume that the subject is standing a fixed distance away from the Kinect in a relatively clear area. This can be maintained by placing a marker on the floor where the subject should stand.

The GUI exposes the following controls:
- camera angle
- near/far threshold
- min/max blob area
- blobs to look for

**camera angle** aims the camera up or down. I've found that the best isolation of the hands is achieved when the kinect is mounted at about shoulder height. Resets to zero when the application is closed.

**near/far threshold** set the target area in which to look for the hands. Start with the far threshold set to a low value and increase until torso of the subject is excluded but arms held in front of the body are still seen in the tracking view. Next, set the near threshold to the maximum and decrease until just before the tips of the fingers are exlcuded. These two values together set the min/max range that the Z ouput is scaled to.

**min/max blob area** help openCV find the hands by telling it how big they are within the frame. Increasing min blob area can be used exlcude small objects that are in the frame or parts of the body that edge into the tracking area. 

**blobs to look for** tells openCV how many objects to track. Should be set to 2 to look for the left and right hand.

Currently, there is no smart assignment of the blobs to the left or right hand. Whatever is found on the left half of the image is treated as the left hand, and whatever is found on the right half of the image is treated as the right hand.

### OSC

[Open Sound Control](https://opensoundcontrol.stanford.edu/) is a protocol for sending real time communication messages between synthesizers, computers, and other multimedia devices. 

By default, this application sends the OSC messages to port 8000 on localhost. If you want to change this, the relevant #defines are at the top of ofApp.h

I use this with the OSC routing application [OSCulator](https://osculator.net/), which is highly recommended if you're using MacOS. [OSCRouter](https://github.com/ETCLabs/OSCRouter) from ETCLabs is another option.