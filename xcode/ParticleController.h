//
//  ParticleController.h
//  CinderTouch
//
//  Created by Brian Mehrman on 3/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/Channel.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"

#include <list>

class ParticleController {
public:
    ParticleController();
    
    void update(const ci::Perlin &perlin, const ci::Vec2i &mouseLoc);
    void draw();
    void addParticle( int xi, int yi, int res );
    void addParticles( int amt, const ci::Vec2i &mouseLoc , const ci::Vec2f &mouseVel, const ci::Color &color);
    void removeParticles( int amt );
    
    std::list<Particle> mParticles;
    
    int mXRes, mYRes;
};