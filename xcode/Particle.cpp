//
//  Particle.cpp
//  CinderTouch
//
//  Created by Brian Mehrman on 3/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include <cmath>

using namespace ci;

Particle::Particle() 
{
    
}

Particle::Particle( Vec2f loc, Vec2f vel, ci::Color color )
{
    mLoc         = loc;
    
    mVel         = vel;
    mScale       = 3.0f;
    mAge         = 0;
    mAgePer      = 1.0f;
    mIsDead      = false;
    mLifespan    = Rand::randInt( 50, 250 );
    mDecay       = Rand::randFloat( 0.8f, 0.99f );
    mColor       = Color(mDecay,mDecay,mDecay) * color;
    
}

void Particle::update(const Perlin &perlin, const Vec2i &mouseLoc )
{
    
    float nX = mLoc.x * 0.005f;
    float nY = mLoc.y * 0.005f;
    float nZ = app::getElapsedSeconds() * 0.1f;
    float noise = perlin.fBm(nX, nY, nZ);
    float angle = noise * 15.0f;
    Vec2f noiseVector( cos(angle), sin(angle) );
    
    mVel += noiseVector * 0.2f * (1.0f - mAgePer);
    mLoc += mVel;
    mVel *= mDecay;
    mAge++;
    
    mRadius            = mScale * mAgePer;
    
    if(mAge > mLifespan )
    {
        mIsDead = true;
    }
    mAgePer       = 1.0f - ((float)mAge / (float)mLifespan );
    
    
}

void Particle::draw()
{
    gl::color(mColor);
    gl::drawSolidCircle( mLoc, mRadius * mAgePer);
}