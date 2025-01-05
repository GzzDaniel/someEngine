// main loop 
#include "mainloop.h"



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
void Player::render()
{
	//PlayerSprite _sprite = currSprite;








	int scale = 3;

	SDL_Rect dstQuad = { (int)getxPos(), (int)getyPos(), 18 * scale, 23 * scale };

	if (currSprite == FACING_RIGHT) {

		SDL_RenderCopyEx(_renderer, texture, &quadsArray[FACING_LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
	}

	SDL_RenderCopyEx(_renderer, texture, &quadsArray[currSprite], &dstQuad, 0, NULL, SDL_FLIP_NONE);
}
void Player::update()
{
	double const speed = 0.3;
	double horizontal = 0;
	double vertical = 0;
	double diagonalFactor = 1;

	/*if (state == WALKING) {
		switch (direction)
		{

		case UP:
			prevSprite = FACING_UP;
			move(0, -speed);
			break;
		case DOWN:
			prevSprite = FACING_DOWN;
			move(0, speed);
			break;
		case LEFT:
			prevSprite = FACING_LEFT;
			move(-speed, 0);
			break;
		case RIGHT:
			prevSprite = FACING_RIGHT;
			move(speed, 0);
			break;
		}

	*/
	bool changeSprite = true;
	if (isKeyPressed[KEY_PRESS_UP]) {
		vertical -= 1;
		//currSprite = FACING_UP;
		}
	if (isKeyPressed[KEY_PRESS_DOWN]) {
		vertical += 1;
		//currSprite = FACING_DOWN;
		}
	if (isKeyPressed[KEY_PRESS_LEFT]) {
		horizontal -= 1;
		//currSprite = FACING_LEFT;
		}
	if (isKeyPressed[KEY_PRESS_RIGHT]) {
		horizontal += 1;
		//currSprite = FACING_RIGHT;
		}
	if (horizontal != 0 && vertical != 0) {
		diagonalFactor = 0.7071067811865476;
	}

	move(horizontal * speed * diagonalFactor, vertical * speed * diagonalFactor);
	
}

void Player::handleInput(Keypress k)
{
	
	switch (k) 
		// controller actions (pressing directional keys)
	{
	case KEY_PRESS_UP:
		direction = UP;
		currSprite = FACING_UP;
		state = WALKING;
		
		break;
	case KEY_PRESS_DOWN:
		direction = DOWN;
		currSprite = FACING_DOWN;
		state = WALKING;
		
		break;
	case KEY_PRESS_LEFT:
		direction = LEFT;
		currSprite = FACING_LEFT;
		state = WALKING;
		
		break;
	case KEY_PRESS_RIGHT:
		direction = RIGHT;
		currSprite = FACING_RIGHT;
		state = WALKING;
		
		break;
	/*case NO_KEY_PRESSED:
		state = IDLE;
		break;*/
	}
	
}



int main(int argc, char* args[])
{
	
	Engine engine;

	Player link;
	
	//link2.loadmedia();
	engine.addObserver(&link);
	engine.addInputObserver(&link);

	engine.gameLoop();

	return 0;
}