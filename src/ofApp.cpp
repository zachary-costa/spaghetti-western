#include "ofApp.h"
//Zack Costa
int score;
int lives;

BaseObject::BaseObject() {
	trans = glm::vec3(0,0,0);
	scale = glm::vec3(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(glm::vec3 pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = glm::vec3(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 60;
	height = 80;
	tumble = 0;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		if (name == "bullet")
		{
			ofPushMatrix();
			glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(-width / 2.0 + trans.x, -height / 2.0 + trans.y - 60, 0));
			float bulAngRad = bulletAngle * (PI / 180);
			glm::mat4 M = glm::rotate(m, glm::radians(bulAngRad), glm::vec3(0, 0, 1));
			//cout << bulAngRad << endl;
			ofMultMatrix(M);
			image.draw(0, 0);
			ofPopMatrix();
		}
		else if (name == "tumbleweed" || name == "tumbleweedb")
		{
			image.setAnchorPercent(0.5, 0.5);
			ofPushMatrix();
			glm::mat4 m = glm::translate(glm::mat4(1.0), glm::vec3(-width / 2.0 + trans.x + 60, -height / 2.0 + trans.y, 0));
			tumble += .0010;
			glm::mat4 M = glm::rotate(m, glm::degrees(tumble), glm::vec3(0, 0, 1));
			ofMultMatrix(M);
			image.draw(0, 0);
			ofPopMatrix();

		}
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
	}
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {
	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {

		// Collision system only checks if there are potential sprites to collide with
		if (collisionSystem.size() != 0)
		{
			vector<Sprite>::iterator os = collisionSystem.begin();
			vector<Sprite>::iterator tmp2;
			while (os != collisionSystem.end())
			{
				glm::vec3 dist = (s->trans - os->trans);
				float slargestSize;
				float oslargestSize;

				//Checks for the largest side of the sprite and the other sprite tested for collision
				if (os->width >= os->height)
				{
					oslargestSize = os->width;
				}
				else
				{
					oslargestSize = os->height;
				}
				if (s->width >= s->height)
				{
					slargestSize = s->width;
				}
				else
				{
					slargestSize = s->height;
				}
				float distlen = glm::length(dist);
				float cd = (slargestSize / 2) + (oslargestSize / 2);

				//If distance from other sprite is smaller than the minimum distance the two sprites can be without contact, there is a collision.

				if (distlen <= cd)
				{
					tmp2 = collisionSystem.erase(os);
					os = tmp;
					os = collisionSystem.end();
					s->lifespan = 0;
					if (s->name == "tumbleweed") score += 10;
					else if (s->name == "tumbleweedb") score += 20;
				}
				else
				{
					os++;
				}
			}
		}
		if ((s->lifespan != -1 && s->age() > s->lifespan)) {
				//cout << "deleting sprite: " << s->name << endl;
				tmp = sprites.erase(s);
				s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = glm::vec3(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childImageName = "UnamedSprite";
	bDamaged = false;
}

//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//	Changed slightly to alter drawing characteristics if it's the character.
//
void Emitter::draw() {
	if (drawable) {

		if (haveImage) {
			// If the Emitter is the character turret and it is being hit be an enemy, the character turret briefly turns red
			// to show the player was damaged.

			if (childImageName == "bullet" && bDamaged)
			{
				ofSetColor(ofColor::red);
				image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
				ofSetColor(ofColor::white);
			}
			else {
				image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
			}

		}
		else {
			ofSetColor(0, 0, 200);
			ofDrawRectangle(-width/2 + trans.x, -height/2 + trans.y, width, height);
		}
	}

	// draw sprite system
	//
	sys->draw();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();
if (bShoot && (time - lastSpawned) > (1000.0 / rate)) {
	// spawn a new sprite
	
	Sprite sprite;
	if (haveChildImage) sprite.setImage(childImage);
	sprite.bulletAngle = shootingAngle;
	sprite.velocity = velocity;
	sprite.lifespan = lifespan;
	sprite.name = childImageName;
	if (childImageName == "bullet") {
		sprite.setPosition(glm::vec3(trans.x + 60, trans.y, trans.z));
	}
	else
	{
		sprite.setPosition(trans);
	}
	sprite.birthtime = time;
	sys->add(sprite);
	lastSpawned = time;
}
sys->update();
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(glm::vec3 v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}


//--------------------------------------------------------------
void ofApp::setup() {
	score = 0;
	lives = 5;

	ofSetVerticalSync(true);
	//Initially set to idle waiting for spacebar to be pressed to activate.
	bIdle = true;
	bGameOver = false;
	bHit = false;
	bUseMouse = false;
	//Load all the images first, in painter order.
	background.load("images/background.png");
	backgroundScroll = -2000;

	if (defaultImage.load("images/bullet.png")) {
		defaultImage.resize(defaultImage.getWidth() / 10, defaultImage.getHeight() / 10);
		imageLoaded = true;
	}
	else {
		ofLogFatalError("can't load image: images/space-invaders-ship-scaled.png");
		ofExit();
	}
	if (characterImage.load("images/cowboyhatabove.png")) {
		characterImageLoaded = true;
		characterImage.resize(characterImage.getWidth() / 3.75, characterImage.getHeight() / 3.75);

	}
	else {
		ofLogFatalError("can't load image: cowboyhatabove.png");
		ofExit();

	}

	if (tumbleweedImage.load("images/tumbleweed.png") )
	{
		if (tumbleweedImageB.load("images/tumbleweedflower.png"))
		{
			enemiesLoaded = true;
			tumbleweedImage.resize(tumbleweedImage.getWidth() / 4, tumbleweedImage.getHeight() / 4);
			tumbleweedImage.setAnchorPercent(0.5, 0.5);
			tumbleweedImageB.resize(tumbleweedImageB.getWidth() / 2, tumbleweedImageB.getHeight() / 2);
			tumbleweedImageB.setAnchorPercent(0.5, 0.5);

		}
		else
		{
			ofLogFatalError("can't load image: tumbleweedflower.png");
			ofExit();
		}
	}
	else
	{
		ofLogFatalError("can't load image: tumbleweed.png");
		ofExit();

	}

	if (gameOver.load("images/game_over.png"))
	{
		gameOverLoaded = true;
		gameOver.setAnchorPercent(0.5, 0.5);

	}
	else
	{
		ofLogFatalError("can't load image: game_over.png");
		ofExit();

	}
	if (menuImage.load("images/Splash.png"))
	{
		menuLoaded = true;
	}
	else
	{
		ofLogFatalError("can't load image: Splash.png");
		ofExit();
	}
	if (font1.load("fonts/western.ttf", 60))
	{
		fontLoaded = true;
	}
	else
	{
		cout << ("can't load font: western.tff") << endl;
		ofExit();
	}
	//Death Explosion explodes character on death/game over
	//Dust storm particle system decreases visibility as game evolves.
	ringForce = new ImpulseRadialForce(1000);
	ringForce->setHeight(0);
	ringForce->setheightLimit(true);

	deathExplosion = new ParticleEmitter();
	deathExplosion->sys->addForce(ringForce);
	deathExplosion->setVelocity(ofVec3f(1000, 0 , 0));
	deathExplosion->setOneShot(true);
	deathExplosion->setEmitterType(RadialEmitter);
	deathExplosion->setGroupSize(1000);
	deathExplosion->setLifespan(-1);
	deathExplosion->setRate(1.0);
	deathExplosion->setParticleRadius(10);

	dustStorm = new ParticleEmitter();
	dustStorm->sys->addForce(ringForce);
	dustStorm->setPosition(ofVec3f(100, ofGetWindowHeight()/2, 5));
	dustStorm->particleColor = ofColor::tan;
	dustStorm->setOneShot(false);
	dustStorm->setGroupSize(1);
	dustStorm->setLifespan(2.5);
	dustStorm->setParticleRadius(4);
	dustStorm->setVelocity(ofVec3f(1800, 0, 0));
	dustStorm->sys->reset();
	dustStorm->start();



	turret = new Emitter(new SpriteSystem());
	turret->setPosition(glm::vec3(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 0));
	turret->drawable = true;            
	turret->setChildImage(defaultImage);
	turret->setImage(characterImage);
	turret->childImageName = "bullet";
	turret->start();
	music.load("sounds/backgroundMusic.mp3");
	firingSound.load("sounds/Gun.mp3");
	wilhelm.load("sounds/Wilhelm.mp3");
	ouch.load("sounds/ouch.mp3");
	tumbleweedSound.load("sounds/Tumbleweed.mp3");
	tumbleweedSound.setVolume(0.4f);

	tumbleweed = new Emitter(new SpriteSystem());
	tumbleweedflower = new Emitter(new SpriteSystem());


	tumbleweed->setImage(tumbleweedImage);
	tumbleweed->setPosition(glm::vec3(ofGetWindowWidth() / 2, -2*tumbleweed->image.getHeight(), 0));
	tumbleweed->setChildImage(tumbleweedImage);
	tumbleweed->childImageName = "tumbleweed";
	tumbleweed->drawable = true;
	tumbleweed->start();
	tumbleweed->bShoot = true;

	tumbleweedflower->setImage(tumbleweedImageB);
	tumbleweedflower->setPosition(glm::vec3(-tumbleweedflower->image.getWidth(), ofGetWindowHeight()/2, 0));
	tumbleweed->setImage(tumbleweedImageB);
	tumbleweedflower->setChildImage(tumbleweedImageB);
	tumbleweedflower->childImageName = "tumbleweedb";
	tumbleweedflower->drawable = true;
	tumbleweedflower->start();
	tumbleweedflower->bShoot = true;

	

	gui.setup();
	gui.add(rate.setup("Rate of Fire", 1, 1, 10));
	gui.add(life.setup("Bullet lifespan", 5, .1, 10));
	gui.add(characterSpeed.setup("character Speed", 5, 1, 20));
	gui.add(bulletSpeed.setup("Bullet Speed", 5, 0, 10));
	gui.add(angle.setup("Shooting Angle", 0, -700, 700 ));
	gui.add(tumbleweedRate.setup("Tumbleweed Rate of Fire", .10, 0, 1));
	gui.add(tumbleweedAngle.setup("Tumbleweed Angle Variance", 180, 1, 360));
	gui.add(tumbleweedVelocity.setup("TumbleweedVelocity", 100, 0, 200));
	bHide = true;
}

//--------------------------------------------------------------
//Turret updates its position based on either keyboard or mouse inputs. Checks in place to prevent the character from leaving the screen
//Music starts upon launching rather than upon non-idling. 
void ofApp::update() {
	if (!music.isPlaying())
	{
		music.play();
	}
	turret->setRate(rate);
	turret->setLifespan(life * 1000);    // convert to milliseconds 
	turret->setVelocity(glm::vec3((int)angle, -100*bulletSpeed, 0));
	turret->shootingAngle = angle;

	//Tumbleweed emitter's rates and velocity change as the score increases to increase difficulty.
	//Tumbleweed Flower is similar to tumbleweed, but is worth more points, spawns less and comes from the left side or right side depending on lives left
	tumbleweed->setPosition(glm::vec3((ofGetWindowWidth() / 2) + (rand()%500 -250), -2 * tumbleweed->image.getHeight(), 0));
	tumbleweed->setRate((rand()% 5+(score/20))*tumbleweedRate);
	tumbleweed->setLifespan(13500);
	tumbleweed->setVelocity(glm::vec3((rand()%(tumbleweedAngle + (score/10)))-((tumbleweedAngle+(score/10))/2), tumbleweedVelocity+(score/10), 0));
	tumbleweedflower->setRate(tumbleweedRate);
	tumbleweedflower->setLifespan(22000);
	if (lives % 2 == 1)
	{
		tumbleweedflower->setPosition(glm::vec3(-tumbleweedflower->image.getWidth(), ofGetWindowHeight() / 2, 0));
		tumbleweedflower->setVelocity(glm::vec3(tumbleweedVelocity + (score / 5), (rand() % tumbleweedAngle / 2) - (tumbleweedAngle / 4), 0));
	}
	else
	{
		tumbleweedflower->setPosition(glm::vec3((ofGetWindowWidth() + tumbleweedflower->image.getWidth()), ofGetWindowHeight() / 2, 0));
		tumbleweedflower->setVelocity(glm::vec3(-(tumbleweedVelocity + (score / 5)), (rand() % tumbleweedAngle / 2) - (tumbleweedAngle / 4), 0));
	}
	glm::vec3 futurePos = turret->trans + (characterMovementXL + characterMovementXR + characterMovementYU + characterMovementYD)*(int)characterSpeed;
	if (!bIdle) {
		backgroundScroll++;
		if (backgroundScroll == 0)
		{
			backgroundScroll = -2000;
		}
		if (!(futurePos.x > ofGetWindowWidth() - characterImage.getWidth() / 2 || futurePos.x < 0 + characterImage.getWidth() / 2))
		{
			turret->setPosition(turret->trans + (characterMovementXL + characterMovementXR)*(int)characterSpeed);
		}
		if (!(futurePos.y < 0 + characterImage.getHeight() / 2 || futurePos.y > ofGetWindowHeight() - characterImage.getHeight() / 2))
		{
			turret->setPosition(turret->trans + (characterMovementYU + characterMovementYD)*(int)characterSpeed);
		}
		if (bMouseEntered && bUseMouse && mouseX + characterImage.getWidth() / 2 < ofGetWindowWidth()
			&& mouseX - characterImage.getWidth() / 2 > 0
			&& mouseY + characterImage.getHeight() / 2 < ofGetWindowHeight() && mouseY - characterImage.getHeight() / 2 > 0)
		{
			turret->setPosition(glm::vec3(mouseX, mouseY, 0));
		}

	
		//If spacebar is clicked the turret shoots, and it will play a firing sound only if there is no sound playing.
		//This is to prevent messy audio overlap especially apparent at higher fire rates.
		turret->bShoot = bShootClicked;
		if (bShootClicked)
		{
			if (!firingSound.isPlaying()) {
				firingSound.play();
			}
		}
		if (tumbleweed->sys->sprites.size() != 0)
		{
			if (!tumbleweedSound.isPlaying()) {
				tumbleweedSound.play();
			}
		}

		tumbleweed->sys->collisionSystem = turret->sys->sprites;
		tumbleweedflower->sys->collisionSystem = turret->sys->sprites;
		turret->sys->collisionSystem.insert(turret->sys->collisionSystem.begin(), tumbleweed->sys->sprites.begin(), tumbleweed->sys->sprites.end());
		turret->sys->collisionSystem.insert(turret->sys->collisionSystem.end(), tumbleweedflower->sys->sprites.begin(), tumbleweedflower->sys->sprites.end());

		turret->collisionSprites = { tumbleweed->sys->sprites, tumbleweedflower->sys->sprites };

		//Health System
		if (ofGetElapsedTimeMillis() - hitTime > 3000.0)
		{
			
			bHit = false;
			turret->bDamaged = bHit;
		}
		for (vector<Sprite> sv : turret->collisionSprites)
		{
			for (Sprite s : sv)
			{
				glm::vec3 dist = (turret->trans - s.trans);
				float distlen = glm::length(dist);
				float cd = (turret->width / 2) + (s.width / 2);
				if (distlen <= cd && !bHit)
				{
					bHit = true;
					turret->bDamaged = bHit;
					lives--;
					hitTime = ofGetElapsedTimeMillis();
					if (lives == 0)
					{
						bGameOver = true;
						deathExplosion->sys->reset();
						deathExplosion->start();
						if (!wilhelm.isPlaying()) {
							wilhelm.play();
						}

					}
					else
					{
						if (!ouch.isPlaying()) {
							ouch.play();
						}

					}
				}
			}
		}
		tumbleweedflower->update();
		tumbleweed->update();
		turret->update();
	}
	deathExplosion->setPosition(ofVec3f(turret->trans.x, turret->trans.y, 5));
	deathExplosion->update();
	dustStorm->setRate(1.0 + score/5);
	dustStorm->setPosition(ofVec3f(-100, rand() % ofGetWindowHeight(), 10));
	dustStorm->update();
	
	turret->sys->collisionSystem.clear();

	if (bGameOver)
	{
		bIdle = true;

	}
}


//--------------------------------------------------------------
void ofApp::draw(){
	background.draw(0,backgroundScroll);
	if (bIdle && !bGameOver) {
		ofSetColor(ofColor::red);
		font1.drawString("Press 'Spacebar' to start.", ofGetWindowWidth() / 3 - 30, ofGetWindowHeight() / 3);
		ofSetColor(ofColor::white);
	}
	if (!bGameOver) {
		turret->draw();
	}
	tumbleweed->draw();
	tumbleweedflower->draw();
	dustStorm->draw();
	deathExplosion->draw();
	ofSetColor(ofColor::black);
	font1.drawString("Score: " + to_string(score), ofGetWindowWidth() - 300, 60);
	font1.drawString("Lives: " + to_string(lives), ofGetWindowWidth()/2 - 90, 60);
	ofSetColor(ofColor::white);

	if (bIdle && !bGameOver) {
		menuImage.draw(0, 0);
		ofSetColor(ofColor::red);
		font1.drawString("Press 'Spacebar' to start.", ofGetWindowWidth() / 2 + 120, ofGetWindowHeight() - 60);
		ofSetColor(ofColor::white);
		font1.drawString("Controls:\nUse Arrow Keys to move,\nand Spacebar to shoot", 75, ofGetWindowHeight() - 200);
	}

	if (bGameOver)
	{
		gameOver.draw(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
	}

	if (!bHide) {
		gui.draw();
	}
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;
	mouseX = x;
	mouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
	bMouseEntered = true;
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
	bMouseEntered = false;
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'r':
		break;
	case 's':
		break;
	case 'u':
		break;
	case 'm':
		bUseMouse = !bUseMouse;
		break;
	case ' ':
		if (bIdle)
		{
			bIdle = false;
		}
		else
		{
			bShootClicked = true;
		}
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case OF_KEY_LEFT:
		characterMovementXL = glm::vec3(-1, 0, 0);
		break;
	case OF_KEY_RIGHT:
		characterMovementXR = glm::vec3(1, 0, 0);
		break;
	case OF_KEY_UP:
		characterMovementYU = glm::vec3(0, -1, 0);
		break;
	case OF_KEY_DOWN:
		characterMovementYD = glm::vec3(0, 1, 0);
		break;
		}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_LEFT:
		characterMovementXL = glm::vec3(0, 0, 0);
		break;
	case OF_KEY_RIGHT:
		characterMovementXR = glm::vec3(0, 0, 0);
		break;
	case OF_KEY_UP:
		characterMovementYU = glm::vec3(0, 0, 0);
		break;
	case OF_KEY_DOWN:
		characterMovementYD = glm::vec3(0, 0, 0);
		break;
	case ' ':
		bShootClicked = false;
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

