// main loop 
#include "mainloop.h"


void ControllerManager::pressKey(Keypress key)
{
	KeysPressed[key] = true;
	keypressDeque[numKeysPressed] = key;
	numKeysPressed++;
}
void ControllerManager::releaseKey(Keypress key) 
{
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
Keypress ControllerManager::getnKeyPressed(int n)
{
	return keypressDeque[n];
}
Keypress ControllerManager::getLastKeypress() 
{
	if (numKeysPressed - 1 >= 0) {
		return keypressDeque[numKeysPressed - 1];
	}
	else {
		return KEY_PRESS_NULL;
	}
} 
Keypress ControllerManager::getSecondLastKeypress()
{
	if (numKeysPressed - 2 >= 0) {
		return keypressDeque[numKeysPressed - 2];
	}
	else {
		return KEY_PRESS_NULL;
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
bool ControllerManager::isKeyPressed(Keypress k) 
{
	return KeysPressed[k];
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
void ControllerManager::showDeque() {
	for (int i = 0; i < 4; i++) {
		std::cout << keypressDeque[i] << " ";
	}
	std::cout << "\n";
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

	standingSprites[DOWN] = { 15, 9, 19, 23 };
	standingSprites[LEFT] = { 51, 9, 19, 23 };
	standingSprites[UP] = { 79, 9, 19, 23 };

	//TODO load sprite by multiplying and using for loops
	// sprites are separated every 32 pixels horizontally
	walkingDownSprites[0] = { 15, 77, 19, 23 };
	walkingDownSprites[1] = { 47, 77, 19, 23 };
	walkingDownSprites[2] = { 79, 77, 19, 23 };
	walkingDownSprites[3] = { 111, 77, 19, 23 };
	walkingDownSprites[4] = { 143, 77, 19, 23 };
	walkingDownSprites[5] = { 175, 77, 19, 23 };
	walkingDownSprites[6] = { 207, 77, 19, 23 };
	walkingDownSprites[7] = { 239, 77, 19, 23 };
	walkingDownSprites[8] = { 271, 77, 19, 23 };
	walkingDownSprites[9] = { 303, 77, 19, 23 };

	int startLeft = 349;
	walkingLeftSprites[0] = { startLeft + 32 * 0, 77, 19, 23 };
	walkingLeftSprites[1] = { startLeft + 32 * 1, 77, 19, 23 };
	walkingLeftSprites[2] = { startLeft + 32 * 2, 77, 19, 23 };
	walkingLeftSprites[3] = { startLeft + 32 * 3, 77, 19, 23 };
	walkingLeftSprites[4] = { startLeft + 32 * 4, 77, 19, 23 };
	walkingLeftSprites[5] = { startLeft + 32 * 5, 77, 19, 23 };
	walkingLeftSprites[6] = { startLeft + 32 * 6, 77, 19, 23 };
	walkingLeftSprites[7] = { startLeft + 32 * 7, 77, 19, 23 };
	walkingLeftSprites[8] = { startLeft + 32 * 8, 77, 19, 23 };
	walkingLeftSprites[9] = { startLeft + 32 * 9, 77, 19, 23 };

	int startUp = 683;
	walkingUpSprites[0] = { startUp + 32 * 0, 77, 19, 23 };
	walkingUpSprites[1] = { startUp + 32 * 1, 77, 19, 23 };
	walkingUpSprites[2] = { startUp + 32 * 2, 77, 19, 23 };
	walkingUpSprites[3] = { startUp + 32 * 3, 77, 19, 23 };
	walkingUpSprites[4] = { startUp + 32 * 4, 77, 19, 23 };
	walkingUpSprites[5] = { startUp + 32 * 5, 77, 19, 23 };
	walkingUpSprites[6] = { startUp + 32 * 6, 77, 19, 23 };
	walkingUpSprites[7] = { startUp + 32 * 7, 77, 19, 23 };
	walkingUpSprites[8] = { startUp + 32 * 8, 77, 19, 23 };
	walkingUpSprites[9] = { startUp + 32 * 9, 77, 19, 23 };


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
		direction = UP;
		state = WALKING;
		moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		direction = DOWN;
		state = WALKING;
		moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		direction = LEFT;
		state = WALKING;
		moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		direction = RIGHT;
		state = WALKING;
		moveBools[RIGHT] = true;
		break;
	default:
		state = IDLE;
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
	// TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
	// Velocity variables
	verticalVelocity = 0;
	HorizontalVelocity = 0;
	diagonalFactor = 1;

	if (moveBools[UP]) {
		verticalVelocity -= 1;
	}
	else if (moveBools[DOWN]) {
		verticalVelocity += 1;
	}
	if (moveBools[RIGHT]) {
		HorizontalVelocity += 1;
	}
	else if (moveBools[LEFT]) {
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
	SDL_Rect dstQuad = { (int)getxPos(), (int)getyPos(), 18 * scale, 23 * scale };

	if (state == IDLE)
	{
		frameNum = 0;

		if (direction == RIGHT) {

			SDL_RenderCopyEx(_renderer, texture, &standingSprites[LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
		}

		SDL_RenderCopyEx(_renderer, texture, &standingSprites[direction], &dstQuad, 0, NULL, SDL_FLIP_NONE);
	}
	if (state == WALKING) 
	{
		switch (direction) {
		case DOWN:
			SDL_RenderCopyEx(_renderer, texture, &walkingDownSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
			break;
		case LEFT:
			SDL_RenderCopyEx(_renderer, texture, &walkingLeftSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
			break;
		case RIGHT:
			SDL_RenderCopyEx(_renderer, texture, &walkingLeftSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
			break;
		case UP:
			SDL_RenderCopyEx(_renderer, texture, &walkingUpSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
			break;
		}
		
		frameNum++;
		if (frameNum/animationDelay >= 10) {
			frameNum = 0;
		}
	}
	
}


int main(int argc, char* args[])
{

	Engine engine;

	Player link(SCREEN_WIDTH/2, SCREEN_HEIGHT-100);
	engine.addInputObserver(&link);

	engine.gameLoop();

	return 0;
}