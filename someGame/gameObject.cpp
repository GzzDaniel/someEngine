#include "gameObject.h"


void GameObject::setxPos(double inxpos) {
	xpos = inxpos;
}
void GameObject::setyPos(double inypos) {
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


void SpriteRenderer::loadmedia(SDL_Renderer* _renderer, std::string path)
{
	SDL_Surface* loadedSurface = NULL;
	loadedSurface = IMG_Load(path.c_str());

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

	defineSrcSprites();
}
void SpriteRenderer::moveSprite(double x, double y)
{
	xPos += x;
	yPos += y;
}
void SpriteRenderer::renderSprite(SDL_Renderer* renderer, SDL_Rect* srcQuad, SDL_RendererFlip flip, SDL_Rect* camera, int inOffsetx, int inOffsety)
{
	dstQuad = { (int)(xPos - camera->x + inOffsetx + (xOffset * scale)), (int)(yPos - camera->y + inOffsety + (yOffset * scale)), (int)(width * scale), (int)(height * scale) };
	
	SDL_RenderCopyEx(renderer, texture, srcQuad, &dstQuad, 0, NULL, flip);
}



bool Collider::isverticalColliding(Collider* c)
{
	return std::abs(this->centerx - c->centerx) < (this->halfWidth + c->halfWidth);
}
bool Collider::isHorizontalColliding(Collider* c)
{
	return std::abs(this->centery - c->centery) < (this->halfHeight + c->halfHeight);
}
bool Collider::isColliding(Collider* c)
{
	return (isHorizontalColliding(c) && isverticalColliding(c));
}
void Collider::drawCollisionBox(SDL_Renderer* _renderer, SDL_Rect* camera)
{
	SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0xFF, 0xFF);
	// center point
	SDL_RenderDrawPoint(_renderer, centerx - camera->x, centery-camera->y);
	//vertical first line
	SDL_RenderDrawLine(_renderer, centerx - halfWidth - camera->x, centery - halfHeight - camera->y, centerx - halfWidth - camera->x, centery + halfHeight - camera->y);
	//vertical second line
	SDL_RenderDrawLine(_renderer, centerx + halfWidth - camera->x, centery - halfHeight - camera->y, centerx + halfWidth - camera->x, centery + halfHeight - camera->y);
	//Horizontal first line
	SDL_RenderDrawLine(_renderer, centerx - halfWidth - camera->x, centery + halfHeight - camera->y, centerx + halfWidth - camera->x, centery + halfHeight - camera->y);
	//Horizontal second line
	SDL_RenderDrawLine(_renderer, centerx - halfWidth - camera->x, centery - halfHeight - camera->y, centerx + halfWidth - camera->x, centery - halfHeight - camera->y);
}
void Collider::setColliderCenter(int x, int y)
{
	prevCenterx = centerx;
	prevCentery = centery;
	centerx = x;
	centery = y;
}
CollisionType Collider::getType()
{
	return type;
}
// TODO implement corner collisions
CollisionType Collider::getPrevCollision(Collider* c)
{
	bool horizontalCollision = std::abs(this->prevCentery - c->prevCentery) < (this->halfHeight + c->halfHeight);
	bool verticalCollision = std::abs(this->prevCenterx - c->prevCenterx) < (this->halfWidth + c->halfWidth);

	if (horizontalCollision && verticalCollision) {
		// Total collision
		return TYPE_TOTAL;
	}
	else if (horizontalCollision) {
		return TYPE_HORIZONTAL;
	}
	else if (verticalCollision) {
		return TYPE_VERTICAL;
	}
	else {
		return TYPE_NONE;
	}
}