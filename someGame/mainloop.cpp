// main loop 
#include "mainloop.h"

Engine engine;
Engine* pe = &engine;

// test function to create players at runtime
void makePlayer() {
	Player* link = new Player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 4);
	pe->addInputObserver(link);
	pe->addColliderObserver(link);
	link->loadmedia(_renderer, "assets/link2.png");
	pe->addRenderedObserver(link);
	pe->addCameraTarget(link);
}

int main(int argc, char* args[])
{
	Background bg;
	bg.loadmedia(_renderer, "assets/bg.png");
	engine.addRenderedObserver(&bg);
	
	// makes a player, the function could be mapped to a button
	//  to create copies of the player zelda minish cap style 
	makePlayer();

	Table f(250, 200);
	f.loadmedia(_renderer, "assets/table.png");
	engine.addObserver(&f);
	engine.addRenderedObserver(&f);
	engine.addColliderObserver(&f);
	
	engine.run();

	return 0;
}