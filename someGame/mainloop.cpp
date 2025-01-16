// main loop 
#include "mainloop.h"




int main(int argc, char* args[])
{

	Engine engine;

	Player link(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 3);
	engine.addInputObserver(&link);
	engine.addColliderObserver(&link);
	link.loadmedia(_renderer);

	Obstacle f(250, 200);
	engine.addObserver(&f);
	engine.addColliderObserver(&f);

	engine.run();

	return 0;
}