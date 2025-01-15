#include "player.h"


void Player::loadmedia(SDL_Renderer* _renderer)
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
	standingSprites[LEFT] = { 48, 9, 19, 23 };
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

	int startLeft = 348;
	walkingLeftSprites[0] = { startLeft + 32 * 0, 77, 19, 23 };
	walkingLeftSprites[1] = { startLeft - 2 + 32 * 1, 77, 19, 23 };
	walkingLeftSprites[2] = { startLeft + 32 * 2, 77, 19, 23 };
	walkingLeftSprites[3] = { startLeft + 32 * 3, 77, 19, 23 };
	walkingLeftSprites[4] = { startLeft - 2 + 32 * 4, 77, 19, 23 };
	walkingLeftSprites[5] = { startLeft + 32 * 5, 77, 19, 23 };
	walkingLeftSprites[6] = { startLeft - 3 + 32 * 6, 77, 19, 23 };
	walkingLeftSprites[7] = { startLeft + 32 * 7, 77, 19, 23 };
	walkingLeftSprites[8] = { startLeft + 3 + 32 * 8, 77, 19, 23 };
	walkingLeftSprites[9] = { startLeft - 2 + 32 * 9, 77, 19, 23 };

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
		state = STATE_WALKING;
		moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		direction = DOWN;
		state = STATE_WALKING;
		moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		direction = LEFT;
		state = STATE_WALKING;
		moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		direction = RIGHT;
		state = STATE_WALKING;
		moveBools[RIGHT] = true;
		break;
	default:
		state = STATE_IDLE;
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

	// adjusts the collider to the sprite. link's sprite specifically
	setColliderCenter((int)(getxPos() + 17 * scale / 2), (int)(getyPos() + 23 * scale / 2));
}
void Player::onCollision(Collider* other)
{
	if (other->getType() == TYPE_WALL)
	{
		// get difference in positions to make calculations easier
		// TODO set position based on the center with one line only
		int xdif = getCenterx() - getxPos();
		int ydif = getCentery() - getyPos();

		switch (getPrevCollision(other))
		{
		case TYPE_VERTICAL:
			if (verticalVelocity < 0) {
				// bottom up
				//std::cout << "bottom up\n";
				setColliderCenter(getCenterx(), other->getCentery() + other->getHalfHeight() + getHalfHeight());
			}
			else if (verticalVelocity > 0) {
				// up to bottom
				//std::cout << "up to bottom\n";
				setColliderCenter(getCenterx(), other->getCentery() - other->getHalfHeight() - getHalfHeight());
			}
			setyPos(getCentery() - ydif);
			break;
		case TYPE_HORIZONTAL:
			if (HorizontalVelocity < 0) {
				// right to left
				//std::cout << "right to left\n";
				setColliderCenter(other->getCenterx() + other->getHalfWidth() + getHalfWidth(), getCentery());
			}
			else if (HorizontalVelocity > 0) {
				// left to right
				//std::cout << "left to right\n";
				setColliderCenter(other->getCenterx() - other->getHalfWidth() - getHalfWidth(), getCentery());
			}
			setxPos(getCenterx() - xdif);
			break;
		}
	}
}
void Player::render(SDL_Renderer* _renderer)
{
	SDL_Rect dstQuad = { (int)getxPos(), (int)getyPos(), 18 * scale, 23 * scale };

	if (state == STATE_IDLE)
	{
		frameNum = 0;

		if (direction == RIGHT) {

			SDL_RenderCopyEx(_renderer, texture, &standingSprites[LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
		}

		SDL_RenderCopyEx(_renderer, texture, &standingSprites[direction], &dstQuad, 0, NULL, SDL_FLIP_NONE);
	}
	if (state == STATE_WALKING)
		//state == STATE_WALKING
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
		if (frameNum / animationDelay >= 10) {
			frameNum = 0;
		}
	}

	drawCollisionBox(_renderer);
}