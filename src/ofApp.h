#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Particle.h"
#include "ParticleEmitter.h"

//Zack Costa
typedef enum { MoveStop, MoveLeft, MoveRight, MoveUp, MoveDown } MoveDir;

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	glm::vec3 trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(glm::vec3);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	float bulletAngle;
	float tumble;
	glm::vec3 velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;  
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem  {
public:
	void add(Sprite);
	void remove(int);
	void update();
	void draw();
	//void collisions(vector <Sprite>);
	vector<Sprite> sprites;
	vector<Sprite> collisionSystem;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter: public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(glm::vec3);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void update();
	SpriteSystem *sys;
	float rate;
	float characterSpeed;
	float shootingAngle;
	glm::vec3 velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	string childImageName;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	bool bShoot;
	bool bDamaged;
	float width, height;
	vector<vector<Sprite>> collisionSprites;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// vector<Emitter *> emitters;
		// int numEmitters;
		ofSoundPlayer firingSound;
		ofSoundPlayer tumbleweedSound;
		ofSoundPlayer music;
		ofSoundPlayer wilhelm;
		ofSoundPlayer ouch;

		ParticleEmitter *deathExplosion = NULL;
		ParticleEmitter *dustStorm = NULL;
		ImpulseRadialForce *ringForce = NULL;

		Emitter *turret = NULL;
		Emitter *tumbleweed = NULL;
		Emitter *tumbleweedflower = NULL;

		ofImage defaultImage;
		ofImage characterImage;
		ofImage background;
		ofImage tumbleweedImage;
		ofImage tumbleweedImageB;
		ofImage gameOver;
		ofImage menuImage;
		glm::vec3 mouse_last;
		glm::vec3 characterMovementXL;
		glm::vec3 characterMovementYU;
		glm::vec3 characterMovementXR;
		glm::vec3 characterMovementYD;

		ofTrueTypeFont font1;
		bool fontLoaded;
		bool imageLoaded;
		bool menuLoaded;
		bool characterImageLoaded;
		bool enemiesLoaded;
		bool gameOverLoaded;
		bool bHide;
		bool bIdle;
		bool bGameOver;
		bool bHit;
		float hitTime;
		bool bShootClicked;
		bool bMouseEntered;
		bool bUseMouse;
		int mouseX;
		int mouseY;
		float backgroundScroll;
		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxIntSlider characterSpeed;
		ofxIntSlider angle;
		ofxIntSlider bulletSpeed;
		ofxVec3Slider velocity;

		ofxFloatSlider tumbleweedRate;
		ofxIntSlider tumbleweedAngle;
		ofxFloatSlider tumbleweedVelocity;
		
		ofxLabel screenSize;

		ofxPanel gui;
};
