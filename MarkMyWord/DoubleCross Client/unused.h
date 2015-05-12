#if !defined(_unused)
#define _unused

#include "os.h"
#include "sci.h"

#define ROOM_INTRO		9002
#define ROOM_END		9004
#define ROOM_CREDITS	9006

typedef struct {
	char name[1];
} BOARD_PLAYER;

typedef struct {
	char name[1];
} CASINO_PLAYER;

class SIGS
	{
	public:
		SIGS() { }
		~SIGS() { }
	};

#endif
