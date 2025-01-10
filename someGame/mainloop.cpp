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
	moveBools[RIGHT] = controller->isKeyPressed(KEY_PRESS_RIGHT);
	moveBools[LEFT] = controller->isKeyPressed(KEY_PRESS_LEFT);
	moveBools[UP] = controller->isKeyPressed(KEY_PRESS_UP);
	moveBools[DOWN] = controller->isKeyPressed(KEY_PRESS_DOWN);	

	Keypress firstInput = KEY_PRESS_NULL;
	Keypress horInput = KEY_PRESS_NULL;
	Keypress verInput = KEY_PRESS_NULL;

	if (controller->isKeyPressed(KEY_PRESS_DOWN) && controller->isKeyPressed(KEY_PRESS_UP)) 
	{
		if (controller->getArrivalIndex(KEY_PRESS_DOWN) > controller->getArrivalIndex(KEY_PRESS_UP)) {
			moveBools[UP] = false;
			verInput = KEY_PRESS_DOWN;
		}
		else {
			moveBools[DOWN] = false;
			verInput = KEY_PRESS_UP;
		}
	}
	else {
		if (moveBools[UP]) { verInput = KEY_PRESS_UP; }
		if (moveBools[DOWN]) { verInput = KEY_PRESS_DOWN; }
	}

	if (controller->isKeyPressed(KEY_PRESS_LEFT) && controller->isKeyPressed(KEY_PRESS_RIGHT))
	{
		if (controller->getArrivalIndex(KEY_PRESS_LEFT) > controller->getArrivalIndex(KEY_PRESS_RIGHT)) {
			moveBools[RIGHT] = false;
			horInput = KEY_PRESS_LEFT;
		}
		else {
			moveBools[LEFT] = false;
			horInput = KEY_PRESS_RIGHT;
		}
	}
	else {
		if (moveBools[LEFT]) { horInput = KEY_PRESS_LEFT; }
		if (moveBools[RIGHT]) { horInput = KEY_PRESS_RIGHT; }
	}

	if (controller->getArrivalIndex(verInput) < controller->getArrivalIndex(horInput)) {
		firstInput = verInput;
	}
	else {
		firstInput = horInput;
	}

	switch (firstInput) 
	{
	case KEY_PRESS_UP:
		currSprite = FACING_UP;
		break;
	case KEY_PRESS_DOWN:
		currSprite = FACING_DOWN;
		break;
	case KEY_PRESS_LEFT:
		currSprite = FACING_LEFT;
		break;
	case KEY_PRESS_RIGHT:
		currSprite = FACING_RIGHT;
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