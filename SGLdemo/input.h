#pragma once
#include <SGL_math.h>
typedef U8 Key; //this is so wrong, i know.
enum MOUSE_BUTTONS
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
	MOUSE_X1,
	MOUSE_X2,
	MOUSE_ANY,
	MOUSE_BTN_COUNT
};
enum KEY_MASKS
{
	KEY_DOWN = 1,
	KEY_UP = 2,
	KEY_PRESSED = 4,
};
struct Mouse
{
	SGL_Vec2i pos;
	SGL_Vec2 posf;
	Key buttons[MOUSE_BTN_COUNT];
	Mouse() : pos({ 0,0 }), posf({0.0f,0.0f})
	{}
};
class InputManager
{
private:
	Mouse mouse;
	Key* keys;
	I32 anyKey;
	I32 keyCount;
public:
	InputManager();
	~InputManager();
	inline void Update()
	{
		//this sort of updates the keyboard state in the depths of SDL
		SDL_PumpEvents();
		//get keystates
		const unsigned char* states = SDL_GetKeyboardState(NULL);
		//reset anykey
		keys[anyKey] = 0;
		//execute bitwise monstrosities to the keys
		//this might be an awful solution, but i liked fooling around with the bitwise operators
		for (int i = 0; i < keyCount; i++)
		{
			//if the SDL key is pressed
			if (states[i])
			{
				//if pressed mask is false
				if (!((keys[i] & KEY_PRESSED) == KEY_PRESSED))
				{
					//key is down for the first time;
					keys[i] |= KEY_DOWN;
				}
				else
				{
					//key was down already so remove the down mask from key
					keys[i] &= ~KEY_DOWN;
				}
				//set pressed mask to true
				keys[i] |= KEY_PRESSED;
				//this shouldnt never happen?
				//set up mask to false just in case
				keys[i] &= ~KEY_UP;
				//if key is down
				if ((keys[i] & KEY_DOWN) == KEY_DOWN)
				{
					//set anykey down as well
					keys[anyKey] |= KEY_DOWN;
				}
				//set anykeys pressed mask to true
				keys[anyKey] |= KEY_PRESSED;
			}
			//if the key was being pressed on the last frame
			else if ((keys[i] & KEY_PRESSED) == KEY_PRESSED)
			{
				//set up mask to true
				keys[i] |= KEY_UP;
				//set pressed mask to false
				keys[i] &= ~KEY_PRESSED;
				//set anykeys up to true aswell
				keys[anyKey] |= KEY_UP;
			}
			else
			{
				//set all masks to false
				//same as UP = false, DOWN = false, PRESSED = false
				keys[i] = 0;
			}
		}
	}
	inline bool IsKeyDown(I32 key)
	{
		return (keys[SDL_GetScancodeFromKey(key)] & KEY_DOWN) == KEY_DOWN;
	}
	inline bool IsKeyUp(I32 key)
	{
		return (keys[SDL_GetScancodeFromKey(key)] & KEY_UP) == KEY_UP;
	}
	inline bool IsKeyPressed(I32 key)
	{
		return (keys[SDL_GetScancodeFromKey(key)] & KEY_PRESSED) == KEY_PRESSED;
	}
	inline bool IsAnyKeyDown()
	{
		return (keys[anyKey] & KEY_DOWN) == KEY_DOWN;
	}
	inline bool IsAnyKeyUp()
	{
		return (keys[anyKey] & KEY_UP) == KEY_UP;
	}
	inline bool IsAnyKeyPressed()
	{
		return (keys[anyKey] & KEY_PRESSED) == KEY_PRESSED;
	}
};
InputManager::InputManager()
{
	SDL_GetKeyboardState(&keyCount);
	anyKey = keyCount;
	keyCount++;
	keys = (Key*)SDL_calloc(keyCount, sizeof(Key));
	for (size_t i = 0; i < MOUSE_BTN_COUNT; i++)
	{
		mouse.buttons[i] = 0;
	}
}

InputManager::~InputManager()
{
	SDL_free(keys);
}
