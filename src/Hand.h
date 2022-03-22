#ifndef _HAND
#define _HAND

#include "ofMain.h"


class Hand
{
    public:
    Hand()=default;    
    void setup(float x_, float y_, float z_, ofColor col);
	void update();
    void update(ofDefaultVec3 target);
    void update(float x, float y, float z);
	void draw(float x, float y, float width, float height);	
    
	ofDefaultVec3 pos;
	ofColor color;

	bool matched;
    bool valid;
	bool changed;
	int lifespan;

};
#endif