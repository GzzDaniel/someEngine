// main loop 
#include "mainloop.h"



void GameObject::setPos(int inxpos, int inypos) {
	xpos = inxpos;
	ypos = inypos;
}
void GameObject::move(int inxmove, int inymove) {
	// moves the total amount of units input
	xpos = xpos + inxmove;
	ypos = ypos + inymove;
}
int GameObject::getxPos() {
	return xpos;
}
int GameObject::getyPos() {
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
}
void Player::render(PlayerSprite _sprite)
{
	prevSprite = _sprite;

	SDL_Rect quadsArray[NUMBER_OF_SPRITES];

	int scale = 5;

	quadsArray[FACING_DOWN] = { 15, 9, 18, 23 };
	quadsArray[FACING_LEFT] = { 51, 9, 18, 23 };
	quadsArray[FACING_UP] = { 79, 9, 19, 23 };

	SDL_Rect dstQuad = { getxPos(), getyPos(), 18 * scale, 23 * scale };
	

	if (_sprite == FACING_RIGHT) {

		SDL_RenderCopyEx(_renderer, texture, &quadsArray[FACING_LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
	}

	SDL_RenderCopyEx(_renderer, texture, &quadsArray[_sprite], &dstQuad, 0, NULL, SDL_FLIP_NONE);
}
void Player::update()
{
	//TODO implement this
	return;
}
void Player::onNotify(Event _event)
{

	switch (_event) 
	{
	case KEY_PRESS_UP:
		render(FACING_UP);
		move(0, -1);
		break;
	case KEY_PRESS_DOWN:
		render(FACING_DOWN);
		move(0, 1);
		break;
	case KEY_PRESS_LEFT:
		render(FACING_LEFT);
		move(-1, 0);
		break;
	case KEY_PRESS_RIGHT:
		render(FACING_RIGHT);
		move(1, 0);
		break;
	default:
		keyPress = KEY_PRESS_NULL;
		render(prevSprite);
		break;
	}
	
}





int main(int argc, char* args[])
{
	
	Engine engine;

	Player link;
	//link.loadmedia();
	Player link2(100, 0);
	Player link3(200, 0);
	Player link4(300, 0);
	//link2.loadmedia();
	engine.addInputObserver(&link);
	engine.addInputObserver(&link2);
	engine.addInputObserver(&link3);
	engine.addInputObserver(&link4);


	engine.gameLoop();

	return 0;
}