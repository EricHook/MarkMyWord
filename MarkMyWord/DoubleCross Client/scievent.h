#ifndef _scievent
#define _scievent

#include "sciobj.h"

class sciPlane;

typedef enum
	{
	SK_UPARROW = 1,
	SK_DOWNARROW,
	SK_LEFTARROW,
	SK_RIGHTARROW,
	SK_HOME,
	SK_END,
	SK_PAGEUP,
	SK_PAGEDOWN,
	SK_F1,
	SK_F2,
	SK_F3,
	SK_F4,
	SK_F5,
	SK_F6,
	SK_F7,
	SK_F8,
	SK_F9,
	SK_F10,
	SK_F11,
	SK_F12,
	} SPECIAL_KEYS;

typedef enum
	{
	SHIFT_DOWN	= 0x0001,
	CTRL_DOWN	= 0x0002,
	ALT_DOWN	= 0x0004,
	LEFT_MOUSE	= 0x0008,
	RIGHT_MOUSE	= 0x0010,
	SPECIAL_KEY	= 0x0020,
	JOYSTICK_1	= 0x0040,
	JOYSTICK_2	= 0x0080,
	} EVENT_MODIFIER;

typedef enum
	{
	EVENT_TYPE_NONE,
	MOUSE_DOWN,
	MOUSE_UP,
	MOUSE_MOVE,
	KEY_PRESS,
	GAMEPAD_NORTH_UP,
	GAMEPAD_NORTH_DOWN,
	GAMEPAD_SOUTH_UP,
	GAMEPAD_SOUTH_DOWN,
	GAMEPAD_EAST_UP,
	GAMEPAD_EAST_DOWN,
	GAMEPAD_WEST_UP,
	GAMEPAD_WEST_DOWN,
	JOYSTICK_BUTTON_1_DOWN,
	JOYSTICK_BUTTON_1_UP,
	JOYSTICK_BUTTON_2_DOWN,
	JOYSTICK_BUTTON_2_UP,
	JOYSTICK_BUTTON_3_DOWN,
	JOYSTICK_BUTTON_3_UP,
	JOYSTICK_BUTTON_4_DOWN,
	JOYSTICK_BUTTON_4_UP,
	KEY_DOWN,
	KEY_UP,
	JOYSTICK_MOVE,
	MOUSE_WHEEL
	} EVENT_TYPE;

class sciEvent : public sciObject
	{
	public:
		sciEvent();
		~sciEvent();

		Boolean claimed;
		int x;
		int y;
		int z; // wheel rotation
		char c;
		EVENT_TYPE type;
		int modifiers;
		sciPlane *plane;
		unsigned int message;
		unsigned long lparam;
		unsigned long wparam;
	};

#endif //_scievent
