#include "Hand.h"

#define LIFESPAN 100
#define LERP_AMT 0.5

void Hand::setup(float x_, float y_, float z_, ofColor col)
{
    pos.x = x_;
    pos.y = y_;
    pos.z = z_;
    color = col;
    matched = false;
    valid = false;
    changed = false;
    lifespan = 0;
}

void Hand::update()
{
    if (!matched)
    {
        if (lifespan > 0)
        {
            lifespan--;
        }
        else
        {
            valid = false;
        }
    }
}

void Hand::update(ofDefaultVec3 target)
{
    pos.x = ofLerp(pos.x, target.x, LERP_AMT);
    pos.y = ofLerp(pos.y, target.y, LERP_AMT);
    pos.z = ofLerp(pos.z, target.z, LERP_AMT);
    matched = true;
    valid = true;
    lifespan = LIFESPAN;
    changed = true;
}

void Hand::update(float x, float y, float z)
{
    update(ofDefaultVec3(x, y, z));
}

void Hand::draw(float x, float y, float width, float height)
{
    if (valid)
    {
        color.a = static_cast<int>(ofMap(lifespan, 0, LIFESPAN, 0, 255));
        ofSetColor(color);
        ofFill();

        float drawX = ofMap(pos.x, 0.0f, 1.0f, x, x + width, true);
        float drawY = ofMap(pos.y, 0.0f, 1.0f, y, y + height, true);
        float radius = ofMap(pos.z, 0.0f, 1.0f, 5.0, 50.0, true);
        ofDrawCircle(drawX, drawY, radius);
    }
}