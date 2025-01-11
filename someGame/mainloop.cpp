// main loop 
#include "mainloop.h"


void ControllerManager::pressKey(Keypress key){
	KeysPressed[key] = true;
	keypressDeque[numKeysPressed] = key;
	numKeysPressed++;

}
void ControllerManager::releaseKey(Keypress key) {
	KeysPressed[key] = false;
	numKeysPressed--;
	int i = 0;
	int j = 0;
	while (i < NUM_KEY_STATES) {
		if (KeysPressed[keypressDeque[i]]) {
			// if key is pressed copy it to the head of the list
			keypressDeque[j] = keypressDeque[i];
			j++;
			i++;
		}
		else {
			i++;
		}
	}
	for (int i = 0; i < NUM_KEY_STATES - numKeysPressed; i++) {
		// replace the rest of the deque with null presses (0)
		keypressDeque[numKeysPressed + i] = KEY_PRESS_NULL;
	}
}
int ControllerManager::getArrivalIndex(Keypress k)
{
	for (int i = 0; i < numKeysPressed; i++) {
		if (k == keypressDeque[i]) {
			return i;
		}
	}
	return NUM_KEY_STATES;
}
Keypress ControllerManager::getHorizontalDpress() 
{
	// the keys pressed later take priority
	if (isKeyPressed(KEY_PRESS_LEFT) && isKeyPressed(KEY_PRESS_RIGHT))
	{
		//both pressed
		if (getArrivalIndex(KEY_PRESS_LEFT) > getArrivalIndex(KEY_PRESS_RIGHT)) {
			return KEY_PRESS_LEFT;
		}
		else {
			return KEY_PRESS_RIGHT;
		}
	}
	else 
	{
		// one pressed
		if (isKeyPressed(KEY_PRESS_LEFT) ) { 
			return KEY_PRESS_LEFT;
		}
		if (isKeyPressed(KEY_PRESS_RIGHT)) {
			return KEY_PRESS_RIGHT;
		}
	}
	// none pressed
	return KEY_PRESS_NULL;
}
Keypress ControllerManager::getVerticalDpress() 
{
	// the keys pressed later take priority
	if (isKeyPressed(KEY_PRESS_UP) && isKeyPressed(KEY_PRESS_DOWN))
	{
		//both pressed
		if (getArrivalIndex(KEY_PRESS_UP) > getArrivalIndex(KEY_PRESS_DOWN)) {
			return KEY_PRESS_UP;
		}
		else {
			return KEY_PRESS_DOWN;
		}
	}
	else
	{
		// one pressed
		if (isKeyPressed(KEY_PRESS_UP)) {
			return KEY_PRESS_UP;
		}
		if (isKeyPressed(KEY_PRESS_DOWN)) {
			return KEY_PRESS_DOWN;
		}
	}
	// none pressed
	return KEY_PRESS_NULL;
}
//gets earliest press
Keypress ControllerManager::getFirstDpress() 
{
	Keypress hor = getHorizontalDpress();
	Keypress ver = getVerticalDpress();

	if (getArrivalIndex( hor ) < getArrivalIndex( ver ) ) {
		return hor;
	}
	else {
		return ver;
	}
}
Keypress ControllerManager::getSecondDpress()
{
	Keypress hor = getHorizontalDpress();
	Keypress ver = getVerticalDpress();

	if (getArrivalIndex(hor) > getArrivalIndex(ver)) {
		return hor;
	}
	else {
		return ver;
	}
}



void GameObject::setPos(double inxpos, double inypos) {
	xpos = inxpos;
	ypos = inypos;
}
void GameObject::move(double inxmove, double inymove) {
	// moves the total amount of units input
	xpos = xpos + inxmove;
	ypos = ypos + inymove;
}
double GameObject::getxPos() {
	return xpos;
}
double GameObject::getyPos() {
	return ypos;
}


void Player::loadmedia()
{
	SDL_Surface* loadedSurface = IMG_Load("link.png");

	if (loadedSurface == NULL) {
		std::cout << " failed to load image " << IMG_GetError();
	}

	//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
	texture = SDL_CreateTextureFromSurface(_renderer, loadedSurface);
	if (texture == NULL)
	{
		std::cout << " failed to make texture " << IMG_GetError();
	}
	SDL_FreeSurface(loadedSurface);

	quadsArray[FACING_DOWN] = { 15, 9, 18, 23 };
	quadsArray[FACING_LEFT] = { 51, 9, 18, 23 };
	quadsArray[FACING_UP] = { 79, 9, 19, 23 };

}

void Player::handleInput(ControllerManager* controller)
{

	moveBools[RIGHT] = false;
	moveBools[LEFT] = false;
	moveBools[UP] = false;
	moveBools[DOWN] = false;

	switch (controller->getFirstDpress())
	{
	case KEY_PRESS_UP:
		currSprite = FACING_UP;
		moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		currSprite = FACING_DOWN;
		moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		currSprite = FACING_LEFT;
		moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		currSprite = FACING_RIGHT;
		moveBools[RIGHT] = true;
		break;
	}
	switch (controller->getSecondDpress())
	{
	case KEY_PRESS_UP:
		moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		moveBools[RIGHT] = true;
		break;
	}


}
void Player::update()
{
	double const speed = 0.25;

	// TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
	// Velocity variables
	verticalVelocity = 0;
	HorizontalVelocity = 0;
	diagonalFactor = 1;

	if (moveBools[UP]) {
		verticalVelocity -= 1;
	}
	if (moveBools[DOWN]) {
		verticalVelocity += 1;
	}
	if (moveBools[RIGHT]) {
		HorizontalVelocity += 1;
	}
	if (moveBools[LEFT]) {
		HorizontalVelocity -= 1;
	}
	if (HorizontalVelocity != 0 && verticalVelocity != 0) {
		// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
		diagonalFactor = 0.7071067811865476;
	}

	move(HorizontalVelocity * speed * diagonalFactor, verticalVelocity * speed * diagonalFactor);

}
void Player::render()
{

	int scale = 3;

	SDL_Rect dstQuad = { (int)getxPos(), (int)getyPos(), 18 * scale, 23 * scale };

	if (currSprite == FACING_RIGHT) {

		SDL_RenderCopyEx(_renderer, texture, &quadsArray[FACING_LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
	}

	SDL_RenderCopyEx(_renderer, texture, &quadsArray[currSprite], &dstQuad, 0, NULL, SDL_FLIP_NONE);
	
}




int main(int argc, char* args[])
{

	Engine engine;

	Player link;

	//link2.loadmedia();
	engine.addInputObserver(&link);
	//engine.addInputObserver(&link);

	engine.gameLoop();

	return 0;
}