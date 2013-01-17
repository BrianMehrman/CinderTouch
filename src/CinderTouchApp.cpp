#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Channel.h"
#include "cinder/Perlin.h"

#include "ParticleController.h"

#define NUM_PARTICLES_TO_SPAWN 100

using namespace ci;
using namespace ci::app;

#include <vector>
#include <map>
#include <list>
using namespace std;


struct TouchPoint {
	TouchPoint() {}
	TouchPoint( const Vec2f &initialPt, const Color &color ) : mColor(color), mTimeOfDeath( -1.0 ) 
	{
		mLoc = initialPt; 
	}
	
	void updatePoint( const Vec2f &pt ) 
    { 
        mVel = (pt - mLoc);
        mLoc = pt;
    }
	
	void startDying() { mTimeOfDeath = getElapsedSeconds() + 2.0f; } // two seconds til dead
	
	bool isDead() const { return getElapsedSeconds() > mTimeOfDeath; }
	
	Vec2f           mLoc;
    Vec2f           mVel;
    Color           mColor;
	float			mTimeOfDeath;
};


class CinderTouchApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	resize( ResizeEvent event );
	virtual void	update();
	virtual void	draw();
    
    void prepareSettings( Settings *settings );
	
    void mouseDown( MouseEvent event );
    void mouseUp( MouseEvent event );
    void mouseMove( MouseEvent event );
    void mouseDrag( MouseEvent event );
	
    void	touchesBegan( TouchEvent event );
    void    touchMove(    vector<TouchEvent::Touch>::const_iterator touchIt );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );
    
    Perlin mPerlin;
    
    ParticleController mParticleController;
    
    Color mColor;
    Vec2i mMouseLoc;
    Vec2f mMouseVel;
    float minRandColor;
    float maxRandColor;
    bool mDrawParticles;
    bool mDrawImage;
    bool mIsMovingForward;
    bool mIsPressed;
    bool mIsTouching;
    
    Matrix44f	mCubeRotation;
	gl::Texture mTex;
	CameraPersp	mCam;
    
    map<uint32_t, TouchPoint>   mActivePoints;
    list<TouchPoint>            mDyingPoints;
};

void CinderTouchApp::setup()
{
    mPerlin             = Perlin();
    mParticleController = ParticleController();
    mMouseLoc           = Vec2i(0,0);
    mMouseVel           = Vec2f::zero();
    minRandColor        = 0.75;
    maxRandColor        = 1.25;
    mDrawParticles      = true;
    mDrawImage          = false;
    mIsMovingForward    = false;
    mIsTouching         = false;
    mIsPressed          = false;
    mColor              = Color(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
    
}

void CinderTouchApp::prepareSettings( Settings *settings )
{
    settings->enableMultiTouch();
}
void CinderTouchApp::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, event.getAspectRatio(), 1, 1000 );
}

void CinderTouchApp::update()
{
    //if ( (! mPerlin) && (! mMouseLoc) ) return;
    
    // kill any points that may need to be killed
    for( list<TouchPoint>::iterator dyingIt = mDyingPoints.begin(); dyingIt != mDyingPoints.end(); ) {
		
		if( dyingIt->isDead() )
			dyingIt = mDyingPoints.erase( dyingIt );
		else
			++dyingIt;
	}
    
    // add more particles for those points still active
    for ( map<uint32_t,TouchPoint>::const_iterator activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt ) {
        mParticleController.addParticles(NUM_PARTICLES_TO_SPAWN,activeIt->second.mLoc, activeIt->second.mVel, activeIt->second.mColor);
    }
    if (mIsPressed) {
        mParticleController.addParticles(NUM_PARTICLES_TO_SPAWN, mMouseLoc, mMouseVel,mColor);
    }
    mParticleController.update(mPerlin, mMouseLoc);
}

void CinderTouchApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ), true );
	
    if (mDrawParticles) {
        mParticleController.draw();
    }
}

void CinderTouchApp::mouseDown( MouseEvent event )
{
    mColor     *= Color(Rand::randFloat(minRandColor,maxRandColor), Rand::randFloat(minRandColor,maxRandColor),Rand::randFloat(minRandColor,maxRandColor));
    mMouseLoc   = event.getPos();
    mIsPressed  = true;
	console() << "Mouse down @ " << event.getPos() << std::endl;
}

void CinderTouchApp::mouseUp(MouseEvent event)
{
    mIsPressed = false;
}

void CinderTouchApp::mouseMove( MouseEvent event )
{
    mMouseVel = ( event.getPos() - mMouseLoc );
    mMouseLoc = event.getPos();
}

void CinderTouchApp::mouseDrag( MouseEvent event )
{
    mouseMove( event );
}

void CinderTouchApp::touchesBegan( TouchEvent event )
{
    if ( ! mIsTouching ) mIsTouching = true;
    // Loop through touches that just began and create a new active point with 
    // create
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Color newColor = mColor * Color(Rand::randFloat(minRandColor,maxRandColor), Rand::randFloat(minRandColor,maxRandColor),Rand::randFloat(minRandColor,maxRandColor));
        mColor         = newColor;
		mActivePoints.insert( make_pair( touchIt->getId(), TouchPoint( touchIt->getPos(), newColor ) ) );
	}

}
void CinderTouchApp::touchMove( vector<TouchEvent::Touch>::const_iterator touchIt )
{
    mActivePoints[touchIt->getId()].updatePoint( touchIt->getPos() );
}

void CinderTouchApp::touchesMoved( TouchEvent event )
{
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
    {
        touchMove(touchIt);
    }
}

void CinderTouchApp::touchesEnded( TouchEvent event )
{
    
    for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touchIt->getId()] );
		mActivePoints.erase( touchIt->getId() );
	}
    if ( mActivePoints.size() <= 0 ) mIsTouching = false;

}

CINDER_APP_COCOA_TOUCH( CinderTouchApp, RendererGl )
