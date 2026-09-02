#include "player.h"

void PlayerState::changeState(Player* player, PlayerState* state, ControllerManager* controller) 
{
	player->frameNum = 0;
	std::cout << "changed states from " << player->getState()->getName() << " to " << state->getName() << "\n";
	player->changeState(state);
	// TODO refactor
	// changes for the next state
	switch (state->getStateID()) {
		case ROLLING:
			state->setStateDirection(player->direction);
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
			state->setStateDirection(player->direction);
			player->_state->initialize(player);
			player->speed = 0.0005;
			break;
		case WALKING:
			player->verticalVelocity = 0;
			player->HorizontalVelocity = 0;
			player->diagonalFactor = 1;
			player->speed = 0.18;
		}		
}

void Player::updateDirection(ControllerManager* controller)
{
	moveBools[RIGHT] = false;
	moveBools[LEFT] = false;
	moveBools[UP] = false;
	moveBools[DOWN] = false;

	// The first dpress determines the direction in which the player faces.
	// currently this only affects the sprite that is rendered, but it could be used for other things like attacks or rolls.
	switch (controller->getFirstDpress())
	{
	case KEY_PRESS_UP:
		direction = UP;
		moveBools[UP] = true;
		break;
	case KEY_PRESS_DOWN:
		direction = DOWN;
		moveBools[DOWN] = true;
		
		break;
	case KEY_PRESS_LEFT:
		direction = LEFT;
		moveBools[LEFT] = true;
		break;
	case KEY_PRESS_RIGHT:
		direction = RIGHT;
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

void Player::moveCharacter() 
{
	// TODO Dynamic and Static classes, or maybe only dynamic bc it adds move functiuons based on velocity. it will store the
   // Velocity variables
	verticalVelocity = 0;
	HorizontalVelocity = 0;

	if (moveBools[UP]) {
		verticalVelocity = -speed;
	}
	else if (moveBools[DOWN]) {
		verticalVelocity = speed;
	}

	if (moveBools[RIGHT]) {
		HorizontalVelocity = speed;
	}
	else if (moveBools[LEFT]) {
		HorizontalVelocity = -speed;
	}

	if (HorizontalVelocity != 0 && verticalVelocity != 0) {
		// used to normalize diagonal movement. otherwise diagonal movement f e e l s slightly faster
		verticalVelocity *= DIAGONAL_FACTOR;
		HorizontalVelocity *= DIAGONAL_FACTOR;
	}

	move(HorizontalVelocity, verticalVelocity);
	moveSprite(HorizontalVelocity, verticalVelocity);
}

void IdleState::handleInput(Player* player, ControllerManager* controller)  
{
	if(controller->getFirstDpress() != KEY_PRESS_NULL){
		
		changeState(player, WalkingState::instance(), controller);
	}
	switch (controller->getLastKeyEvent())
	{
	case KEY_PRESS_SPACE:
		changeState(player, JumpingState::instance());
		break;
	case KEY_PRESS_ATTACK:
		changeState(player, AttackState::instance());
		break;
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
	if (controller->getFirstDpress() == KEY_PRESS_NULL) {
		changeState(player, IdleState::instance());
	}
	else {
		player->updateDirection(controller);
	}
	switch (controller->getLastKeyEvent())
	{
	case KEY_PRESS_SHIFT:
		changeState(player, RollState::instance());
		break;
	case KEY_PRESS_SPACE:
		changeState(player, JumpingState::instance());
		break;
	case KEY_PRESS_ATTACK:
		changeState(player, AttackState::instance());
		break;
	}

}
void WalkingState::update(Player* player)  {
	player->moveCharacter();
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
		changeState(player, JumpingState::instance(), controller);
	}
}
void RollState::update(Player* player)  
{
	player->move(player->HorizontalVelocity, player->verticalVelocity);
	player->moveSprite(player->HorizontalVelocity, player->verticalVelocity);
}
void RollState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	switch (getStateDirection())
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
		if (player->willMove()) {
			changeState(player, WalkingState::instance());
		}
		else {
			changeState(player, IdleState::instance());
		}
	}
	
}
void JumpingState::initialize(Player* player) {
	maxSpeed = player->speed;
}
void JumpingState::handleInput(Player* player, ControllerManager* controller)
{

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

	player->move(player->HorizontalVelocity, player->verticalVelocity);
	player->moveSprite(player->HorizontalVelocity, player->verticalVelocity);
}
void JumpingState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	//std::cout << player->verticalVelocity - player->speed << " " << maxSpeed << "\n";
	//std::cout << player->HorizontalVelocity - player->speed << " " << maxSpeed << "\n";
	int offset = (int)( -((-(505 / 504) * pow((int)(player->frameNum / player->animationDelay), 2)) + ((7135 / 504) * (int)(player->frameNum / player->animationDelay)) + 30));
	

	switch (getStateDirection())
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
		if (player->willMove()) {
			changeState(player, WalkingState::instance());
		}
		else {
			changeState(player, IdleState::instance());
		}
	}
}

void AttackState::handleInput(Player* player, ControllerManager* controller)
{
	switch (controller->getLastKeyEvent())
	{
	case KEY_PRESS_ATTACK:
		changeState(player, AttackState::instance());
		break;
	}
}

void AttackState::update(Player* player)
{
	
}

void AttackState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
{
	int spriteDuration = 2;
	int spriteToRender = player->frameNum / spriteDuration;

	switch (player->direction)
	{
	case DOWN:
		switch (spriteToRender)
		{
		case 0:
			player->renderSprite(renderer, &player->attackDownSprites[0], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[0], SDL_FLIP_NONE, camera, -8, 6);
			break;

		case 1:
			player->renderSprite(renderer, &player->attackDownSprites[1], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[1], SDL_FLIP_NONE, camera, -3, 14);
			break;

		case 2:
			player->renderSprite(renderer, &player->attackDownSprites[2], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[2], SDL_FLIP_NONE, camera, -2, 20);
			break;

		case 3:
			player->renderSprite(renderer, &player->attackDownSprites[3], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[3], SDL_FLIP_NONE, camera, 0, 19);
			break;

		case 4:
			player->renderSprite(renderer, &player->attackDownSprites[4], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[4], SDL_FLIP_NONE, camera, 7, 19);
			break;

		case 5:
			player->renderSprite(renderer, &player->attackDownSprites[5], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[5], SDL_FLIP_NONE, camera, 12, 13);
			break;

		case 6:
			player->renderSprite(renderer, &player->attackDownSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordVerticalSprites[6], SDL_FLIP_NONE, camera, 13, 3);
			break;

		case 7:
			player->renderSprite(renderer, &player->attackDownSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			break;
		}
		break;


	case LEFT:
		switch (spriteToRender)
		{
		case 0:
			player->renderSprite(renderer, &player->swordHorizontalSprites[0], SDL_FLIP_NONE, camera, -3, -5);
			player->renderSprite(renderer, &player->attackLeftSprites[0], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 1:
			player->renderSprite(renderer, &player->swordHorizontalSprites[1], SDL_FLIP_NONE, camera, -11, -5);
			player->renderSprite(renderer, &player->attackLeftSprites[1], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 2:
			player->renderSprite(renderer, &player->attackLeftSprites[2], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[2], SDL_FLIP_NONE, camera, -15, 0);
			break;

		case 3:
			player->renderSprite(renderer, &player->attackLeftSprites[3], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[3], SDL_FLIP_NONE, camera, -15, 4);
			break;

		case 4:
			player->renderSprite(renderer, &player->attackLeftSprites[4], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[4], SDL_FLIP_NONE, camera, -14, 9);
			break;

		case 5:
			player->renderSprite(renderer, &player->attackLeftSprites[5], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[5], SDL_FLIP_NONE, camera, -7, 11);
			break;

		case 6:
			player->renderSprite(renderer, &player->attackLeftSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[6], SDL_FLIP_NONE, camera, 0, 16);
			break;

		case 7:
			player->renderSprite(renderer, &player->attackLeftSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			break;
		}
		break;

	case RIGHT:
		switch (spriteToRender)
		{
		case 0:
			player->renderSprite(renderer, &player->swordHorizontalSprites[0], SDL_FLIP_HORIZONTAL, camera, 3, -5);
			player->renderSprite(renderer, &player->attackLeftSprites[0], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			break;

		case 1:
			player->renderSprite(renderer, &player->swordHorizontalSprites[1], SDL_FLIP_HORIZONTAL, camera, 11, -5);
			player->renderSprite(renderer, &player->attackLeftSprites[1], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			break;

		case 2:
			player->renderSprite(renderer, &player->attackLeftSprites[2], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[2], SDL_FLIP_HORIZONTAL, camera, 15, 0);
			break;

		case 3:
			player->renderSprite(renderer, &player->attackLeftSprites[3], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[3], SDL_FLIP_HORIZONTAL, camera, 15, 4);
			break;

		case 4:
			player->renderSprite(renderer, &player->attackLeftSprites[4], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[4], SDL_FLIP_HORIZONTAL, camera, 14, 9);
			break;

		case 5:
			player->renderSprite(renderer, &player->attackLeftSprites[5], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[5], SDL_FLIP_HORIZONTAL, camera, 7, 11);
			break;

		case 6:
			player->renderSprite(renderer, &player->attackLeftSprites[6], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			player->renderSprite(renderer, &player->swordHorizontalSprites[6], SDL_FLIP_HORIZONTAL, camera, 0, 16);
			break;

		case 7:
			player->renderSprite(renderer, &player->attackLeftSprites[6], SDL_FLIP_HORIZONTAL, camera, 0, 0);
			break;
		}
		break;

	case UP:
		switch (spriteToRender)
		{
		case 0:
			player->renderSprite(renderer, &player->swordVerticalSprites[0],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, 7, 5);
			player->renderSprite(renderer, &player->attackUpSprites[0], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 1:
			player->renderSprite(renderer, &player->swordVerticalSprites[1],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera,2, -5);
			player->renderSprite(renderer, &player->attackUpSprites[1], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 2:
			player->renderSprite(renderer, &player->swordVerticalSprites[2],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, 2, -12);
			player->renderSprite(renderer, &player->attackUpSprites[2], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 3:
			player->renderSprite(renderer, &player->swordVerticalSprites[3],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, 0, -13);
			player->renderSprite(renderer, &player->attackUpSprites[3], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 4:
			player->renderSprite(renderer, &player->swordVerticalSprites[4],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, -7, -9);
			player->renderSprite(renderer, &player->attackUpSprites[4], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 5:
			player->renderSprite(renderer, &player->swordVerticalSprites[5],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, -12, -2);
			player->renderSprite(renderer, &player->attackUpSprites[5], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 6:
			player->renderSprite(renderer, &player->swordVerticalSprites[6],
				(SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
				camera, -11, 4);
			player->renderSprite(renderer, &player->attackUpSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			break;

		case 7:
			player->renderSprite(renderer, &player->attackUpSprites[6], SDL_FLIP_NONE, camera, 0, 0);
			break;
		}
		break;


	}

	player->frameNum++;

	if (player->frameNum >= 8 * spriteDuration)
	{
		if (player->willMove())
			changeState(player, WalkingState::instance());
		else
			changeState(player, IdleState::instance());
	}
}


void LockedInState::handleInput(Player* player, ControllerManager* controller)
{

}

void LockedInState::update(Player* player)
{
}

void LockedInState::render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera)
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
	
	// shadow
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

	// ATTACKING
	for (int i = 0; i < 8; i++) {
		attackDownSprites[i] = { 0 + (32 * i), 96, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		attackLeftSprites[i] = { 256 + (32 * i), 96, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 8; i++) {
		attackUpSprites[i] = { 512 + (32 * i), 96, spriteWidth, spriteHeight };
	}
	//sword
	for (int i = 0; i < 7; i++) {
		swordVerticalSprites[i] = { 0 + (32 * i), 128, spriteWidth, spriteHeight };
	}
	for (int i = 0; i < 7; i++) {
		swordHorizontalSprites[i] = { 256 + (32 * i), 128, spriteWidth, spriteHeight };
	}

}


void Player::handleInput(ControllerManager* controller)
{
	updateDirection(controller);
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


// LOCK IN SYSTEM
double getAngle(const Player& player, const GameObject& to)
{
	double dx = to.getxPos() - player.getxPos();
	double dy = to.getyPos() - player.getyPos();

	return std::atan2(dy, dx);
}
