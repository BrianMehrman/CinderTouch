//
//  Particle.h
//  CinderTouch
//
//  Created by Brian Mehrman on 3/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"

#include <vector>

class Particle {
public:
    Particle();
    Particle(ci::Vec2f, ci::Vec2f, ci::Color);
    void update(const ci::Perlin &perlin, const ci::Vec2i &mouseLoc);
    void draw();
    
    ci::Vec2f mLoc;
    ci::Vec2f mVel;
    
    float     mRadius;
    float     mDecay;
    float     mScale;
    float     mAgePer;
    int       mAge;
    int       mLifespan;
    bool      mIsDead;
    
    ci::Color mColor;
};