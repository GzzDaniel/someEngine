#include "player.h"

void PlayerState::changeState(Player* player, PlayerState* state) 
{
	player->frameNum = 0;

	// next state
	if (state->getStateID() == ROLLING) {
		std::cout << "increased speed, changed states\n";
		player->speed *= 2;
	}

	// prev state
	if (player->getState()->getStateID() == ROLLING) {
		player->speed = 0.18;
	}

	std::cout << "changed states from " << player->getState()->getName() << " to " << state->getName() << "\n";
	player->changeState(state);
}

void IdleState::handleInput(Player* player, ControllerManager* controller)  
{
	if(controller->getFirstDpress() != KEY_PRESS_NULL){
		if (shiftPressed) {
			changeState(player, RollState::instance());
		}
		else {
			changeState(player, WalkingState::instance());
		}
	}
}
void IdleState::update(Player* player)  
{

}
void IdleState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	//TODO idle animation
	if (player->direction == RIGHT) {
		
		player->renderSprite(renderer, &player->standingSprites[LEFT], SDL_FLIP_HORIZONTAL, camera);
	}
	
	player->renderSprite(renderer, &player->standingSprites[player->direction], SDL_FLIP_NONE, camera);
}

// TODO give variables to WalkingState Class and make it legible
void WalkingState::handleInput(Player* player, ControllerManager* controller)  
{
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
	switch (controller->getLastKeyEvent())
	{
	case KEY_PRESS_SHIFT:
		changeState(player, RollState::instance());
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

	double dx = (player->HorizontalVelocity * player->speed * player->diagonalFactor);
	double dy = (player->verticalVelocity * player->speed * player->diagonalFactor);
	
	player->move(dx, dy);
	player->moveSprite(dx, dy);
}
void WalkingState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	switch (player->direction) 
	{
	case DOWN:
		player->renderSprite(renderer, &player->walkingDownSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->walkingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->walkingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_HORIZONTAL, camera);
		break;
	case UP:
		player->renderSprite(renderer, &player->walkingUpSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	}

	player->frameNum++;
	if (player->frameNum / player->animationDelay >= 10) {
		player->frameNum = 0;
	}

}

void RollState::handleInput(Player* player, ControllerManager* controller)  
{

}
void RollState::update(Player* player)  
{
	double dx = (player->HorizontalVelocity * player->speed * player->diagonalFactor);
	double dy = (player->verticalVelocity * player->speed * player->diagonalFactor);

	player->move(dx, dy);
	player->moveSprite(dx, dy);
}
void RollState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	switch (player->direction)
	{
	case DOWN:
		player->renderSprite(renderer, &player->rollingDownSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->rollingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->rollingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_HORIZONTAL, camera);
		break;
	case UP:
		player->renderSprite(renderer, &player->rollingUpSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE, camera);
		break;
	}

	// end at 8th frame
	player->frameNum++;
	if (player->frameNum / player->animationDelay >= 8) {
		changeState(player, WalkingState::instance());
	}
	
}

void JumpingState::handleInput(Player* player, ControllerManager* controller)
{

}
void JumpingState::update(Player* player) 
{
}
void JumpingState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{

}


void Player::defineSrcSprites()
{
	int spriteWidth = getSpriteWidth();
	int spriteHeight = getSpriteHeight();

	// STANDING
	standingSprites[DOWN] = { 0, 0, spriteWidth, spriteHeight};
	standingSprites[LEFT] = {32 , 0, spriteWidth, spriteHeight };
	standingSprites[UP] = { 64, 0, spriteWidth, spriteHeight };

	// OTHERS
	shadowSprite = { 320, 0, spriteWidth, spriteHeight };

	// WALKING
	for (int i = 0; i < 10; i++){
		walkingDownSprites[i] = { 0 + (32 * i), 32, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 10; i++) {
		walkingLeftSprites[i] = { 352 + (32 * i), 32, spriteWidth, spriteHeight };
	}

	for (int i = 0; i < 10; i++) {
		walkingUpSprites[i] = { 704 + (32 * i), 32, spriteWidth, spriteHeight };
	}

	// ROLLIMG
	for (int i = 0; i < 8; i++) {
		rollingDownSprites[i] = { 0 + (32 * i), 64, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		rollingLeftSprites[i] = { 288 + (32 * i), 64, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		rollingUpSprites[i] = { 576 + (32 * i), 64, spriteWidth, spriteHeight };
	}
	
}


void Player::handleInput(ControllerManager* controller)
{
	_state->handleInput(this, controller);
}
void Player::update() 
{
	_state->update(this);
	setColliderCenter((int)getxPos() + getColliderOffsetx(), (int)getyPos() + getColliderOffsety());
}
void Player::render(SDL_Renderer* _renderer, SDL_Rect* camera)
{
	renderSprite(_renderer, &shadowSprite, SDL_FLIP_NONE, camera);

	_state->render(this, _renderer, camera);

	drawCollisionBox(_renderer, camera);
	drawGOPoint(_renderer, camera);
}

void Player::onCollision(Collider* other)
{
	if (other->getType() == TYPE_WALL)
	{
		int startColliderx = getCenterx();
		int startCollidery = getCentery();

		switch (getPrevCollision(other))
		{
		case TYPE_VERTICAL:
			
			if ((getPrevCentery() - getCentery()) > 0) {
				// bottom up
				setColliderCenter(getCenterx(), other->getCentery() + other->getHalfHeight() + getHalfHeight());
			}
			else if ((getPrevCentery() - getCentery()) < 0) {
				// up to bottom
				setColliderCenter(getCenterx(), other->getCentery() - other->getHalfHeight() - getHalfHeight());
			}
			break;

		case TYPE_HORIZONTAL:
			
			if ((getPrevCenterx() - getCenterx()) > 0) {
				// right to left
				setColliderCenter(other->getCenterx() + other->getHalfWidth() + getHalfWidth(), getCentery());
			}
			else if ((getPrevCenterx() - getCenterx()) < 0) {
				// left to right
				setColliderCenter(other->getCenterx() - other->getHalfWidth() - getHalfWidth(), getCentery());
			}
			break;
		}

		int colliderDifx = getCenterx() - startColliderx;
		int colliderDify = getCentery() - startCollidery;

		moveSprite(colliderDifx, colliderDify);
		move(colliderDifx, colliderDify);
	}
	
}