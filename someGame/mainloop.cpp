// main loop 
#include "mainloop.h"

Engine engine;
Engine* pe = &engine;

void makePlayer() {
	Player* link = new Player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
	pe->addInputObserver(link);
	pe->addColliderObserver(link);
	link->loadmedia(_renderer, "link2.png");
	pe->addRenderedObserver(link);
	pe->addCameraTarget(link);
}

int main(int argc, char* args[])
{
	Background bg;
	bg.loadmedia(_renderer, "bg.png");
	engine.addRenderedObserver(&bg);

	makePlayer();

	Table f(250, 200);
	f.loadmedia(_renderer, "table.png");
	engine.addObserver(&f);
	engine.addRenderedObserver(&f);
	engine.addColliderObserver(&f);
	
	engine.run();

	return 0;
}