#include "player.h"

void PlayerState::changeState(Player* player, PlayerState* state) 
{
	player->changeState(state);
}

void IdleState::handleInput(Player* player, ControllerManager* controller)  
{
	switch (controller->getFirstDpress())
	{
	case KEY_PRESS_UP:
	case KEY_PRESS_DOWN:
	case KEY_PRESS_LEFT:
	case KEY_PRESS_RIGHT:
		// updated right away for responsiveness
		WalkingState::instance()->handleInput(player, controller);
		changeState(player, WalkingState::instance());
	}
}
void IdleState::update(Player* player)  
{

}
void IdleState::render(Player* player, SDL_Renderer* renderer)
{
	//TODO idle animation
	SDL_Rect dstQuad = { (int)player->getxPos(), (int)player->getyPos(), 18 * player->scale, 23 * player->scale };

	if (player->direction == RIGHT) {

		SDL_RenderCopyEx(renderer, player->texture, &player->standingSprites[LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
	}

	SDL_RenderCopyEx(renderer, player->texture, &player->standingSprites[player->direction], &dstQuad, 0, NULL, SDL_FLIP_NONE);
	
	player->drawCollisionBox(renderer);
}

// TODO give variables to WalkingState Class and make it legible
void WalkingState::handleInput(Player* player, ControllerManager* controller)  
{

	switch (controller->getLastKeyEvent())
	{
	case KEY_PRESS_SHIFT:
		player->speed *= 3;
		changeState(player, RollState::instance());
		return;
	}

	player->moveBools[RIGHT] = false;
	player->moveBools[LEFT] = false;
	player->moveBools[UP] = false;
	player->moveBools[DOWN] = false;

	switch (controller->getFirstDpress())
	{
	case KEY_PRESS_UP:
		player->direction = UP;
		player->moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		player->direction = DOWN;
		player->moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		player->direction = LEFT;
		player->moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		player->direction = RIGHT;
		player->moveBools[RIGHT] = true;
		break;
	default:
		player->frameNum = 0;
		changeState(player, IdleState::instance());
	}
	switch (controller->getSecondDpress())
	{
	case KEY_PRESS_UP:
		player->moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		player->moveBools[DOWN] = true;
		break;
	case KEY_PRESS_LEFT:
		player->moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		player->moveBools[RIGHT] = true;
		break;
	}
}
void WalkingState::update(Player* player)  
{
	 // TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
	// Velocity variables
	player->verticalVelocity = 0;
	player->HorizontalVelocity = 0;
	player->diagonalFactor = 1;

	if (player->moveBools[UP]) {
		player->verticalVelocity -= 1;
	}
	else if (player->moveBools[DOWN]) {
		player->verticalVelocity += 1;
	}
	if (player->moveBools[RIGHT]) {
		player->HorizontalVelocity += 1;
	}
	else if (player->moveBools[LEFT]) {
		player->HorizontalVelocity -= 1;
	}
	if (player->HorizontalVelocity != 0 && player->verticalVelocity != 0) {
		// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
		player->diagonalFactor = 0.7071067811865476;
	}

	player->move(player->HorizontalVelocity * player->speed * player->diagonalFactor, player->verticalVelocity * player->speed * player->diagonalFactor);

	// adjusts the collider to the sprite. link's sprite specifically
	player->setColliderCenter((int)(player->getxPos() + 17 * player->scale / 2), (int)(player->getyPos() + 23 * player->scale / 2));
}
void WalkingState::render(Player* player, SDL_Renderer* renderer)
{
	SDL_Rect dstQuad = { (int)player->getxPos(), (int)player->getyPos(), 18 * player->scale, 23 * player->scale };

	switch (player->direction) 
	{
	case DOWN:
		SDL_RenderCopyEx(renderer, player->texture, &player->walkingDownSprites[player->frameNum / player->animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
		break;
	case LEFT:
		SDL_RenderCopyEx(renderer, player->texture, &player->walkingLeftSprites[player->frameNum / player->animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
		break;
	case RIGHT:
		SDL_RenderCopyEx(renderer, player->texture, &player->walkingLeftSprites[player->frameNum / player->animationDelay], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
		break;
	case UP:
		SDL_RenderCopyEx(renderer, player->texture, &player->walkingUpSprites[player->frameNum / player->animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
		break;
	}

	player->frameNum++;
	if (player->frameNum / player->animationDelay >= 10) {
		player->frameNum = 0;
	}

	player->drawCollisionBox(renderer);
}

void RollState::handleInput(Player* player, ControllerManager* controller)  
{

}
void RollState::update(Player* player)  
{

	if (_count > 120) {
		_count = 0;
		player->speed = 0.25;
		changeState(player, WalkingState::instance());
	}

	player->move(player->HorizontalVelocity * player->speed * player->diagonalFactor, player->verticalVelocity * player->speed * player->diagonalFactor);
	_count++;
}
void RollState::render(Player* player, SDL_Renderer* renderer)
{

}

void JumpingState::handleInput(Player* player, ControllerManager* controller)
{

}
void JumpingState::update(Player* player) 
{
}
void JumpingState::render(Player* player, SDL_Renderer* renderer)
{

}







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
	_state->handleInput(this, controller);
}
void Player::update() 
{
	_state->update(this);
}
void Player::render(SDL_Renderer* _renderer)
{
	_state->render(this, _renderer);
}
//void Player::handleInput(ControllerManager* controller)
//{
//	moveBools[RIGHT] = false;
//	moveBools[LEFT] = false;
//	moveBools[UP] = false;
//	moveBools[DOWN] = false;
//
//	switch (controller->getFirstDpress())
//	{
//	case KEY_PRESS_UP:
//		direction = UP;
//		moveBools[UP] = true;
//		break;
//	case KEY_PRESS_DOWN:
//		direction = DOWN;
//		moveBools[DOWN] = true;
//		break;
//	case KEY_PRESS_LEFT:
//		direction = LEFT;
//		moveBools[LEFT] = true;
//		break;
//	case KEY_PRESS_RIGHT:
//		direction = RIGHT;
//		moveBools[RIGHT] = true;
//		break;
//	default:
//	}
//	switch (controller->getSecondDpress())
//	{
//	case KEY_PRESS_UP:
//		moveBools[UP] = true;
//		break;
//	case KEY_PRESS_DOWN:
//		moveBools[DOWN] = true;
//		break;
//	case KEY_PRESS_LEFT:
//		moveBools[LEFT] = true;
//		break;
//	case KEY_PRESS_RIGHT:
//		moveBools[RIGHT] = true;
//		break;
//	}
//}
//void Player::update()
//{
//	// TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
//	// Velocity variables
//	verticalVelocity = 0;
//	HorizontalVelocity = 0;
//	diagonalFactor = 1;
//
//	if (moveBools[UP]) {
//		verticalVelocity -= 1;
//	}
//	else if (moveBools[DOWN]) {
//		verticalVelocity += 1;
//	}
//	if (moveBools[RIGHT]) {
//		HorizontalVelocity += 1;
//	}
//	else if (moveBools[LEFT]) {
//		HorizontalVelocity -= 1;
//	}
//	if (HorizontalVelocity != 0 && verticalVelocity != 0) {
//		// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
//		diagonalFactor = 0.7071067811865476;
//	}
//
//	move(HorizontalVelocity * speed * diagonalFactor, verticalVelocity * speed * diagonalFactor);
//
//	// adjusts the collider to the sprite. link's sprite specifically
//	setColliderCenter((int)(getxPos() + 17 * scale / 2), (int)(getyPos() + 23 * scale / 2));
//}
void Player::onCollision(Collider* other)
{
	if (other->getType() == TYPE_WALL)
	{
		// get difference in positions to make calculations easier
		// TODO set position based on the center with one line only
		// or TODO set position based on the difference in distance that the collider got into the other collider
		int xdif = getCenterx() - getxPos();
		int ydif = getCentery() - getyPos();

		switch (getPrevCollision(other))
		{
		case TYPE_VERTICAL:
			
			if (verticalVelocity < 0) {
				// bottom up
				setColliderCenter(getCenterx(), other->getCentery() + other->getHalfHeight() + getHalfHeight());
			}
			else if (verticalVelocity > 0) {
				// up to bottom
				setColliderCenter(getCenterx(), other->getCentery() - other->getHalfHeight() - getHalfHeight()-1);
			}
			setyPos(getCentery() - ydif);
			break;
		case TYPE_HORIZONTAL:
			
			if (HorizontalVelocity < 0) {
				// right to left
				setColliderCenter(other->getCenterx() + other->getHalfWidth() + getHalfWidth(), getCentery());
			}
			else if (HorizontalVelocity > 0) {
				// left to right
				setColliderCenter(other->getCenterx() - other->getHalfWidth() - getHalfWidth()-1, getCentery());
			}
			setxPos(getCenterx() - xdif);
			break;
		}
	}
}
//void Player::render(SDL_Renderer* _renderer)
//{
//	SDL_Rect dstQuad = { (int)getxPos(), (int)getyPos(), 18 * scale, 23 * scale };
//
//	if (state == STATE_IDLE)
//	{
//		frameNum = 0;
//
//		if (direction == RIGHT) {
//
//			SDL_RenderCopyEx(_renderer, texture, &standingSprites[LEFT], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
//		}
//
//		SDL_RenderCopyEx(_renderer, texture, &standingSprites[direction], &dstQuad, 0, NULL, SDL_FLIP_NONE);
//	}
//	if (state == STATE_WALKING)
//		//state == STATE_WALKING
//	{
//		switch (direction) {
//		case DOWN:
//			SDL_RenderCopyEx(_renderer, texture, &walkingDownSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
//			break;
//		case LEFT:
//			SDL_RenderCopyEx(_renderer, texture, &walkingLeftSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
//			break;
//		case RIGHT:
//			SDL_RenderCopyEx(_renderer, texture, &walkingLeftSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_HORIZONTAL);
//			break;
//		case UP:
//			SDL_RenderCopyEx(_renderer, texture, &walkingUpSprites[frameNum / animationDelay], &dstQuad, 0, NULL, SDL_FLIP_NONE);
//			break;
//		}
//
//		frameNum++;
//		if (frameNum / animationDelay >= 10) {
//			frameNum = 0;
//		}
//	}
//
//	drawCollisionBox(_renderer);
//}