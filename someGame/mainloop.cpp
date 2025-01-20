// main loop 
#include "mainloop.h"

Engine engine;
Engine* pe = &engine;

void makePlayer() {
	Player* link = new Player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3);
	pe->addInputObserver(link);
	pe->addColliderObserver(link);
	link->loadmedia(_renderer, "link.png");
	pe->addRenderedObserver(link);
}
int main(int argc, char* args[])
{
	//Background bg;
	//engine.addRenderedObserver(&bg);
	makePlayer();
	Obstacle f(250, 200);
	engine.addObserver(&f);
	engine.addColliderObserver(&f);
	

	engine.run();

	return 0;
}