#include "player.h"

void PlayerState::changeState(Player* player, PlayerState* state) 
{
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
void IdleState::render(Player* player, SDL_Renderer* renderer)
{
	shiftPressed = false;
	//TODO idle animation
	if (player->direction == RIGHT) {
		
		player->renderSprite(renderer, &player->standingSprites[LEFT], SDL_FLIP_HORIZONTAL);
	}
	
	player->renderSprite(renderer, &player->standingSprites[player->direction], SDL_FLIP_NONE);
	player->drawCollisionBox(renderer);
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
		std::cout << "increased speed, changed states\n";
		player->frameNum = 0;
		player->speed *= 2;
		changeState(player, RollState::instance());
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
	//player->moveCollider(dx, dy);
	// adjusts the collider to the sprite. link's sprite specifically

	player->setColliderCenter((int)(player->getxPos() + 17 ), (int)(player->getyPos() + 23));
	//std::cout << player->getCenterx() << ", " << player->getCentery() << "\n";
}
void WalkingState::render(Player* player, SDL_Renderer* renderer)
{
	switch (player->direction) 
	{
	case DOWN:
		player->renderSprite(renderer, &player->walkingDownSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->walkingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->walkingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_HORIZONTAL);
		break;
	case UP:
		player->renderSprite(renderer, &player->walkingUpSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
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
	player->move(player->HorizontalVelocity * player->speed * player->diagonalFactor, player->verticalVelocity * player->speed * player->diagonalFactor);
	player->moveSprite(player->HorizontalVelocity * player->speed * player->diagonalFactor, player->verticalVelocity * player->speed * player->diagonalFactor);
	//player->moveCollider(player->HorizontalVelocity * player->speed * player->diagonalFactor, player->verticalVelocity * player->speed * player->diagonalFactor);

	player->setColliderCenter((int)(player->getxPos() + 17 * player->scale / 2), (int)(player->getyPos() + 23 * player->scale / 2));
}
void RollState::render(Player* player, SDL_Renderer* renderer)
{
	switch (player->direction)
	{
	case DOWN:
		player->renderSprite(renderer, &player->rollingDownSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
		break;
	case LEFT:
		player->renderSprite(renderer, &player->rollingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
		break;
	case RIGHT:
		player->renderSprite(renderer, &player->rollingLeftSprites[player->frameNum / player->animationDelay], SDL_FLIP_HORIZONTAL);
		break;
	case UP:
		player->renderSprite(renderer, &player->rollingUpSprites[player->frameNum / player->animationDelay], SDL_FLIP_NONE);
		break;
	}

	// end at 8th frame
	player->frameNum++;
	if (player->frameNum / player->animationDelay >= 8) {
		player->frameNum = 0;
		player->speed = 0.25;
		changeState(player, WalkingState::instance());
	}
	player->drawCollisionBox(renderer);
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


void Player::defineSrcSprites()
{
	standingSprites[DOWN] = { 15, 9, 19, 23 };
	//standingSprites[DOWN] = { 327 -33, 1102, 19, 23 };
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

	rollingDownSprites[0] = { 19, 1102, 19, 23 };
	rollingDownSprites[1] = { 19 + 32, 1102, 19, 23 };
	rollingDownSprites[2] = { 19 + 32 * 2, 1102, 19, 23 };
	rollingDownSprites[3] = { 19 + 32 * 3, 1102, 19, 23 };
	rollingDownSprites[4] = { 19 + 32 * 4, 1102, 19, 23 };
	rollingDownSprites[5] = { 19 + 32 * 5, 1102, 19, 23 };
	rollingDownSprites[6] = { 19 - 4 + 32 * 6, 1102 + 2, 19, 23 };;
	rollingDownSprites[7] = { 19-4+32*7, 1102+3, 19, 23 };

	rollingLeftSprites[0] = { 327 - 33, 1102, 19, 23 };
	rollingLeftSprites[1] = { 327 + 32 * 0, 1100, 19, 23 };
	rollingLeftSprites[2] = { 328 + 32 * 1, 1100, 19, 23 };
	rollingLeftSprites[3] = { 323 + 32 * 2, 1102, 19, 23 };
	rollingLeftSprites[4] = { 326 + 32 * 3, 1101, 19, 23 };
	rollingLeftSprites[5] = { 326 + 32 * 4, 1101, 19, 23 };
	rollingLeftSprites[6] = { 326 + 32 * 5, 1105, 19, 23 };
	rollingLeftSprites[7] = { startLeft - 2 + 32 * 9, 77, 19, 23 };


	rollingUpSprites[0] = { 539 + 32 * 0, 1104, 19, 23 };
	rollingUpSprites[1] = { 539 + 32 * 1, 1104, 19, 23 };
	rollingUpSprites[2] = { 539 + 32 * 2, 1104, 19, 23 };
	rollingUpSprites[3] = { 539 + 32 * 3, 1104, 19, 23 };
	rollingUpSprites[4] = { 539 + 32 * 4, 1104, 19, 23 };
	rollingUpSprites[5] = { 539 + 32 * 5, 1104, 19, 23 };
	rollingUpSprites[6] = { 539 - 4 + 32 * 6, 1104, 19, 23 };
	rollingUpSprites[7] = { 539 - 4 + 32 * 7, 1104, 19, 23 };
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
	//std::cout << getxPos() << " " << getyPos() << " | " << getCenterx() << " " << getCentery() << " | " << getSpritePosx() << " " << getSpritePosy() << "\n";
	_state->render(this, _renderer);
}

void Player::onCollision(Collider* other)
{
	//std::cout << "before " << getCenterx() << ", " << getCentery() << " center diff " << std::abs(this->getCentery() - other->getCentery()) <<  " height diff " << this->getHalfHeight() +other->getHalfHeight() <<"\n";
	if (other->getType() == TYPE_WALL)
	{
		// get difference in positions to make calculations easier
		// TODO set position based on the center with one line only
		// or TODO set position based on the difference in distance that the collider got into the other collider
		int xdif = (int)(getCenterx() - getxPos());
		int ydif = (int)(getCentery() - getyPos());

		switch (getPrevCollision(other))
		{
		case TYPE_VERTICAL:
			
			if (verticalVelocity < 0) {
				// bottom up
				setColliderCenter(getCenterx(), other->getCentery() + other->getHalfHeight() + getHalfHeight());
				while (isColliding(other)) moveCollider(0, 1);
			}
			else if (verticalVelocity > 0) {
				// up to bottom
				setColliderCenter(getCenterx(), other->getCentery() - other->getHalfHeight() - getHalfHeight());
				while (isColliding(other)) {
					moveCollider(0, -1);

				}
			}
			setyPos(getCentery() - ydif);
			break;
		case TYPE_HORIZONTAL:
			
			if (HorizontalVelocity < 0) {
				// right to left
				setColliderCenter(other->getCenterx() + other->getHalfWidth() + getHalfWidth(), getCentery());
				while (isColliding(other)) moveCollider(1, 0);
			}
			else if (HorizontalVelocity > 0) {
				// left to right
				setColliderCenter(other->getCenterx() - other->getHalfWidth() - getHalfWidth(), getCentery());
				while (isColliding(other)) moveCollider(-1, 0);
			}
			setxPos(getCenterx() - xdif);
			
			
			break;
		}
	}
	//std::cout << "after " << getCenterx() << ", " << getCentery() << " center diff " << std::abs(this->getCentery() - other->getCentery()) << " height diff " << this->getHalfHeight() + other->getHalfHeight() << "\n";
}