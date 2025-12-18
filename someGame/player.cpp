#include "player.h"

void PlayerState::changeState(Player* player, PlayerState* state) 
{
	player->frameNum = 0;
	std::cout << "changed states from " << player->getState()->getName() << " to " << state->getName() << "\n";
	player->changeState(state);

	// changes for the next state
	switch (state->getStateID()) {
	case ROLLING:
		player->speed *= 2;
		player->HorizontalVelocity *= 2;
		player->verticalVelocity *= 2;
		break;
	case IDLE:
		player->speed *= 0;
		for (int i = 0; i < 4; i++) {
			player->moveBools[i] = false;
		}
		player->verticalVelocity = 0;
		player->HorizontalVelocity = 0;
		player->diagonalFactor = 1;
		break;
	case JUMPING:
		player->_state->initialize(player);
		player->speed = 0.0005;
		break;
	case WALKING:
		player->speed = 0.18;
	}	
	
}

void IdleState::handleInput(Player* player, ControllerManager* controller)  
{
	if(controller->getFirstDpress() != KEY_PRESS_NULL){
		/*if (shiftPressed) {
			changeState(player, RollState::instance());
		}
		else {
			changeState(player, WalkingState::instance());
		}*/
		changeState(player, WalkingState::instance());
	}
	if (controller->getLastKeyEvent() == KEY_PRESS_SPACE) {
		changeState(player, JumpingState::instance());
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
	case KEY_PRESS_SPACE:
		changeState(player, JumpingState::instance());
		break;
	}

}
void WalkingState::update(Player* player)  
{
	 // TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
	// Velocity variables
	player->verticalVelocity = 0;
	player->HorizontalVelocity = 0;

	if (player->moveBools[UP]) {
		player->verticalVelocity = -player->speed ;
	}
	else if (player->moveBools[DOWN]) {
		player->verticalVelocity = player->speed;
	}

	if (player->moveBools[RIGHT]) {
		player->HorizontalVelocity = player->speed;
	}
	else if (player->moveBools[LEFT]) {
		player->HorizontalVelocity = -player->speed;
	}

	if (player->HorizontalVelocity != 0 && player->verticalVelocity != 0) {
		// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
		player->verticalVelocity *= DIAGONAL_FACTOR;
		player->HorizontalVelocity *= DIAGONAL_FACTOR;
	}
	
	player->move(player->HorizontalVelocity, player->verticalVelocity);
	player->moveSprite(player->HorizontalVelocity, player->verticalVelocity);

	
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
	if (controller->getLastKeyEvent() == KEY_PRESS_SPACE && player->frameNum > 4) {
		changeState(player, JumpingState::instance());
	}
}
void RollState::update(Player* player)  
{
	player->move(player->HorizontalVelocity, player->verticalVelocity);
	player->moveSprite(player->HorizontalVelocity, player->verticalVelocity);
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
void JumpingState::initialize(Player* player) {
	std::cout << " jump initialized " << maxSpeed << " " << player->speed << "\n";
	maxSpeed = player->speed;
}
void JumpingState::handleInput(Player* player, ControllerManager* controller)
{

	player->moveBools[RIGHT] = false;
	player->moveBools[LEFT] = false;
	player->moveBools[UP] = false;
	player->moveBools[DOWN] = false;

	switch (controller->getFirstDpress())
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
void JumpingState::update(Player* player) 
{
	if (player->moveBools[UP]) {
		if (abs(player->verticalVelocity - player->speed) < maxSpeed) {
			player->verticalVelocity -= player->speed;
		}
	}
	else if (player->moveBools[DOWN]) {
		if (abs(player->verticalVelocity + player->speed) < maxSpeed) {
			player->verticalVelocity += player->speed;
		}
	}

	if (player->moveBools[RIGHT]) {
		if (abs(player->HorizontalVelocity + player->speed) < maxSpeed) {
			player->HorizontalVelocity += player->speed;
		}
	}
	else if (player->moveBools[LEFT]) {
		if (abs(player->HorizontalVelocity - player->speed) < maxSpeed) {
			player->HorizontalVelocity -= player->speed;
		}
	}

	//if (player->HorizontalVelocity != 0 && player->verticalVelocity != 0) {
	//	// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
	//	player->verticalVelocity *= DIAGONAL_FACTOR;
	//	player->HorizontalVelocity *= DIAGONAL_FACTOR;
	//}
	
	player->move(player->HorizontalVelocity, player->verticalVelocity);
	player->moveSprite(player->HorizontalVelocity, player->verticalVelocity);
}
void JumpingState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	//std::cout << player->verticalVelocity - player->speed << " " << maxSpeed << "\n";
	//std::cout << player->HorizontalVelocity - player->speed << " " << maxSpeed << "\n";
	int offset = (int)( -((-(505 / 504) * pow((int)(player->frameNum / player->animationDelay), 2)) + ((7135 / 504) * (int)(player->frameNum / player->animationDelay)) + 30));
	

	switch (player->direction)
	{
	case DOWN:
		player->renderSprite(renderer, &player->rollingDownSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->rollingLeftSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->rollingLeftSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_HORIZONTAL, camera, 0, offset);
		break;
	case UP:
		player->renderSprite(renderer, &player->rollingUpSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	}

	// end at 8th frame
	player->frameNum++;
	if (player->frameNum / player->animationDelay >= 16) {
		changeState(player, WalkingState::instance());
	}
	
}

void AttackState::handleInput(Player* player, ControllerManager* controller)
{

}

void AttackState::update(Player* player)
{
	
}

void AttackState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	switch (player->direction)
	{
	case DOWN:
		player->renderSprite(renderer, &player->rollingDownSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->rollingLeftSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->rollingLeftSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_HORIZONTAL, camera, 0, offset);
		break;
	case UP:
		player->renderSprite(renderer, &player->rollingUpSprites[(player->frameNum / player->animationDelay) % 6], SDL_FLIP_NONE, camera, 0, offset);
		break;
	}
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
	
	// shadow
	shadowSprite = { 320, 0, spriteWidth, spriteHeight };

	// sword
	swordHorizontal = { 352, 0, 16, 16 };
	swordVertical = { 368, 0, 16, 16 };

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

	// ATTACKING
	for (int i = 0; i < 8; i++) {
		rollingDownSprites[i] = { 0 + (32 * i), 96, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		rollingLeftSprites[i] = { 288 + (32 * i), 96, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		rollingUpSprites[i] = { 576 + (32 * i), 96, spriteWidth, spriteHeight };
	}
}


void Player::handleInput(ControllerManager* controller)
{
	_state->handleInput(this, controller);
}
void Player::update() 
{
	setYcamValue((int)getyPos());
	_state->update(this);
	//setColliderCenter((int)getxPos() + getColliderOffsetx(), (int)getyPos() + getColliderOffsety());
	setColliderArrayCenter((int)getxPos(), (int)getyPos());
}
void Player::render(SDL_Renderer* _renderer, SDL_Rect* camera)
{
	renderSprite(_renderer, &shadowSprite, SDL_FLIP_NONE, camera);

	_state->render(this, _renderer, camera);

	//drawCollisionBox(_renderer, camera);
	drawCollisionBoxes(_renderer, camera);
	drawGOPoint(_renderer, camera);
}

void Player::onCollision(Collider* thisCollider, Collider* other)
{

	if (other->getType() == TYPE_PUSHOUT)
	{
		int startColliderx = thisCollider->getCenterx();
		int startCollidery = thisCollider->getCentery();

		switch (thisCollider->getPrevCollision(other))
		{
		case TYPE_VERTICAL:

			if ((thisCollider->getPrevCentery() - thisCollider->getCentery()) > 0) {
				// bottom up
				std::cout << "colliding from bottom \n";
				setColliderArrayCenter(thisCollider->getCenterx(), other->getCentery() + other->getHalfHeight() + thisCollider->getHalfHeight(), false);
			}
			else if ((thisCollider->getPrevCentery() - thisCollider->getCentery()) < 0) {
				// up to bottom
				std::cout << "colliding from top \n";
				setColliderArrayCenter(thisCollider->getCenterx(), other->getCentery() - other->getHalfHeight() - thisCollider->getHalfHeight(), false);
			}
			break;

		case TYPE_HORIZONTAL:
			
			if ((thisCollider->getPrevCenterx() - thisCollider->getCenterx()) > 0) {
				// right to left
				std::cout << "colliding from right \n";
				setColliderArrayCenter(other->getCenterx() + other->getHalfWidth() + thisCollider->getHalfWidth(), thisCollider->getCentery(), false);
			}
			else if ((thisCollider->getPrevCenterx() - thisCollider->getCenterx()) < 0) {
				// left to right
				std::cout << "colliding from left \n";
				setColliderArrayCenter(other->getCenterx() - other->getHalfWidth() - thisCollider->getHalfWidth(), thisCollider->getCentery(), false);
			}
			break;
		case TYPE_TOTAL:

			break;
		case TYPE_NONE:
			break;
		}
		

		int colliderDifx = thisCollider->getCenterx() - startColliderx;
		int colliderDify = thisCollider->getCentery() - startCollidery;

		moveSprite(colliderDifx, colliderDify);
		move(colliderDifx, colliderDify);
	}
	
}