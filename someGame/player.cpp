//#include <SDL.h>
//#include <SDL_image.h>
//#include <stdio.h>
//#include <string>
//#include <iostream>
//
//
//
////class SpriteSheetManager
////{
////public:
////	SpriteSheetManager() {}
////	~SpriteSheetManager() {}
////
////	bool loadSheet()
////
////private:
////
////
////};
//
//
//
//
//class GameObject
//{
//public:
//	GameObject() : xpos(0), ypos(0){}
//	GameObject(int xpos, int ypos) : xpos(xpos), ypos(ypos) {}
//	virtual ~GameObject() {}
//
//	void virtual setPos(int inxpos, int inypos) {
//		xpos = inxpos;
//		ypos = inypos;
//	}
//	// moves the total amount of units input
//	void virtual move(int inxmove, int inymove) {
//		xpos = xpos + inxmove;
//		ypos = ypos + inymove;
//	}
//
//	int virtual getxPos() {
//		return xpos;
//	}
//	int virtual getyPos() {
//		return ypos;
//	}
//
//private:
//	int xpos;
//	int ypos;
//};
//
//
//
//class Player: public GameObject
//{
//public: 
//	Player(): GameObject(0, 0) 
//	{
//	}
//	~Player() {}
//	Player(int posx, int posy) : GameObject(posx, posy) {}
//
//	void render() 
//	{
//		//TODO load media needed to animate link
//		SDL_Surface* loadedSurface = IMG_Load("someGame/link.png");
//		//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
//		//SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, loadedSurface);
//		SDL_FreeSurface(loadedSurface);
//
//		SDL_Rect srcQuad = { 0, 0, 100, 100 };
//		SDL_Rect dstQuad = { 0, 0, 100, 100 };
//
//		//SDL_RenderCopy(_renderer, texture, &srcQuad, &dstQuad);
//
//		//SDL_Rect spriteclip[3];
//
//	}
//
//private:
//	
//
//};
