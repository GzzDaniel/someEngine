#ifndef CONTROLLERMANAGER_H_
#define CONTROLLERMANAGER_H_

#include <SDL.h>
#include <iostream>

// TODO keypress history
// ControllerManager class offers methods to help other class with managinf inputs
enum Keypress {
	KEY_PRESS_NULL,

	// directional
	KEY_PRESS_UP,
	KEY_PRESS_RIGHT, // 2
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT, // 4, horizontal presses are even numbers 

	// key specific
	KEY_PRESS_SHIFT,

	// actions
	KEY_PRESS_INTERACT,

	// key releases
	KEY_RELE_SHIFT,

	NUM_KEY_STATES
};

class ControllerManager
{
public:
	ControllerManager() : numKeysPressed(0) {}
	~ControllerManager() {}

	void pressDpadKey(Keypress k);
	void releaseDpadKey(Keypress k);

	// returns the nth currently pressed key
	Keypress getnKeyPressed(int n);

	//returns last keypress
	Keypress getLastKeypress();

	//returns second to last keypress
	Keypress getSecondLastKeypress();

	// returns the chronological index at which the specified keypress arrived
	int getArrivalIndex(Keypress k);

	//returns true if the keypress given was pressed
	bool isKeyPressed(Keypress k);

	//prints the deque
	void showDeque();

	// Gets the Keypress taking into account physical limitantions of a Dpad
	Keypress getHorizontalDpress();
	Keypress getVerticalDpress();
	Keypress getFirstDpress();
	Keypress getSecondDpress();

	void setLastKeyEvent(Keypress keyEvent);
	Keypress getLastKeyEvent();

private:
	// array of boolean values for the keypresses
	bool KeysPressed[NUM_KEY_STATES] = { false };

	// use to keep track of the index
	int numKeysPressed;

	// array that shoes the order at which the keys were pressed
	Keypress keypressDeque[NUM_KEY_STATES] = { KEY_PRESS_NULL };

	// last key event
	Keypress lastKeyEvent;

};

#endif /* CONTROLLERMANAGER_H_ */