// main loop 
#include "mainloop.h"

Engine engine;
Engine* pe = &engine;

void makePlayer() {
	Player* link = new Player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
	pe->addInputObserver(link);
	pe->addColliderObserver(link);
	link->loadmedia(_renderer, "link.png");
	pe->addRenderedObserver(link);
}
int main(int argc, char* args[])
{
	Background bg;
	bg.loadmedia(_renderer, "bg.png");
	engine.addRenderedObserver(&bg);

	Obstacle f(250, 200);
	f.loadmedia(_renderer, "stuff.png");
	engine.addObserver(&f);
	engine.addRenderedObserver(&f);
	engine.addColliderObserver(&f);
	
	makePlayer();

	engine.run();

	return 0;
}