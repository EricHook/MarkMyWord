#include "os.h"
#include <io.h>

void OSOutputString(char *outputString)
	{
	DebugString(outputString);
	}

int OSFileLength(FILE *file)
	{
	return _filelength(_fileno(file));
	}
