#include "controllermanager.h"

void ControllerManager::pressDpadKey(Keypress key)
{
	KeysPressed[key] = true;
	keypressDeque[numKeysPressed] = key;
	numKeysPressed++;
}
void ControllerManager::releaseDpadKey(Keypress key)
{
	KeysPressed[key] = false;
	numKeysPressed--;
	int i = 0;
	int j = 0;
	while (i < NUM_KEY_STATES) {
		if (KeysPressed[keypressDeque[i]]) {
			// if key is pressed copy it to the head of the list
			keypressDeque[j] = keypressDeque[i];
			j++;
			i++;
		}
		else {
			i++;
		}
	}
	for (int i = 0; i < NUM_KEY_STATES - numKeysPressed; i++) {
		// replace the rest of the deque with null presses (0)
		keypressDeque[numKeysPressed + i] = KEY_PRESS_NULL;
	}
}
Keypress ControllerManager::getnKeyPressed(int n)
{
	return keypressDeque[n];
}
Keypress ControllerManager::getLastKeypress()
{
	if (numKeysPressed - 1 >= 0) {
		return keypressDeque[numKeysPressed - 1];
	}
	else {
		return KEY_PRESS_NULL;
	}
}
Keypress ControllerManager::getSecondLastKeypress()
{
	if (numKeysPressed - 2 >= 0) {
		return keypressDeque[numKeysPressed - 2];
	}
	else {
		return KEY_PRESS_NULL;
	}
}
int ControllerManager::getArrivalIndex(Keypress k)
{
	for (int i = 0; i < numKeysPressed; i++) {
		if (k == keypressDeque[i]) {
			return i;
		}
	}
	return NUM_KEY_STATES;
}
bool ControllerManager::isKeyPressed(Keypress k)
{
	return KeysPressed[k];
}
bool ControllerManager::isDpadPressed()
{
	return (KeysPressed[KEY_PRESS_UP] || KeysPressed[KEY_PRESS_DOWN] || KeysPressed[KEY_PRESS_LEFT] || KeysPressed[KEY_PRESS_RIGHT]);
}

Keypress ControllerManager::getHorizontalDpress()
{
	// the keys pressed later take priority
	if (isKeyPressed(KEY_PRESS_LEFT) && isKeyPressed(KEY_PRESS_RIGHT))
	{
		//both pressed
		if (getArrivalIndex(KEY_PRESS_LEFT) > getArrivalIndex(KEY_PRESS_RIGHT)) {
			return KEY_PRESS_LEFT;
		}
		else {
			return KEY_PRESS_RIGHT;
		}
	}
	else
	{
		// one pressed
		if (isKeyPressed(KEY_PRESS_LEFT)) {
			return KEY_PRESS_LEFT;
		}
		if (isKeyPressed(KEY_PRESS_RIGHT)) {
			return KEY_PRESS_RIGHT;
		}
	}
	// none pressed
	return KEY_PRESS_NULL;
}

Keypress ControllerManager::getVerticalDpress()
{
	// the keys pressed later take priority
	if (isKeyPressed(KEY_PRESS_UP) && isKeyPressed(KEY_PRESS_DOWN))
	{
		//both pressed
		if (getArrivalIndex(KEY_PRESS_UP) > getArrivalIndex(KEY_PRESS_DOWN)) {
			return KEY_PRESS_UP;
		}
		else {
			return KEY_PRESS_DOWN;
		}
	}
	else
	{
		// one pressed
		if (isKeyPressed(KEY_PRESS_UP)) {
			return KEY_PRESS_UP;
		}
		if (isKeyPressed(KEY_PRESS_DOWN)) {
			return KEY_PRESS_DOWN;
		}
	}
	// none pressed
	return KEY_PRESS_NULL;
}

//gets earliest press
Keypress ControllerManager::getFirstDpress()
{
	Keypress hor = getHorizontalDpress();
	Keypress ver = getVerticalDpress();

	if (getArrivalIndex(hor) < getArrivalIndex(ver)) {
		return hor;
	}
	else {
		return ver;
	}
}
Keypress ControllerManager::getSecondDpress()
{
	Keypress hor = getHorizontalDpress();
	Keypress ver = getVerticalDpress();

	if (getArrivalIndex(hor) > getArrivalIndex(ver)) {
		return hor;
	}
	else {
		return ver;
	}
}

const char* keypressToString(Keypress key)
{
	switch (key)
	{
	case KEY_PRESS_NULL:     return "KEY_PRESS_NULL";
	case KEY_PRESS_UP:       return "KEY_PRESS_UP";
	case KEY_PRESS_RIGHT:    return "KEY_PRESS_RIGHT";
	case KEY_PRESS_DOWN:     return "KEY_PRESS_DOWN";
	case KEY_PRESS_LEFT:     return "KEY_PRESS_LEFT";
	case KEY_PRESS_SHIFT:    return "KEY_PRESS_SHIFT";
	case KEY_PRESS_SPACE:    return "KEY_PRESS_SPACE";
	case KEY_RELE_SPACE:     return "KEY_RELE_SPACE";
	case KEY_PRESS_CTRL:     return "KEY_PRESS_CTRL";
	case KEY_RELE_CTRL:      return "KEY_RELE_CTRL";
	case KEY_PRESS_INTERACT: return "KEY_PRESS_INTERACT";
	case KEY_RELE_SHIFT:     return "KEY_RELE_SHIFT";
	default:                 return "UNKNOWN";
	}
}

void ControllerManager::showDeque()
{
	for (int i = 0; i < 4; i++)
	{
		std::cout << keypressToString(keypressDeque[i]) << " ";
	}

	std::cout << "\n";
}

void ControllerManager::setLastKeyEvent(Keypress keyEvent)
{
	lastKeyEvent = keyEvent;
}
Keypress ControllerManager::getLastKeyEvent()
{
	return lastKeyEvent;
}