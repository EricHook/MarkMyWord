#ifndef _doublecrossengine
#define _doublecrossengine

#include "DoubleCrossMessage.h"

#define DBLCROSS_DEFMINWORDSIZE 3

class DoubleCrossEngine
{
public:
	DoubleCrossEngine();
	~DoubleCrossEngine();
	
	void Init(GameState *theGameState);
	void InitPlayer(int seat);
	void InitSeat(int seat);
	void ResetForNewGame();
	void ResetForNewPlayer();
	
	int GetNthBonusValue(int n);
	bool IsSquareEmpty(DCPoint square);
	int GetSquareOwner(DCPoint square); // -1 = not owned
	char GetSquareLetter(DCPoint square);
	int GetSquareSpecial(DCPoint square);
	
	void PlayLetterOnSquare(int seat, char letter, DCPoint square);
	void SetSquareSpecial(DCPoint square, char value);
	void StabilizeSquare(DCPoint square, short score);
	bool IsSquareStable(DCPoint square);
	void ClearSquare(DCPoint square, short score);
	
	int CountLettersInRack(int seat);
	bool IsRackEmpty(int seat);
	bool IsRackFull(int seat);
	
	bool SquareIsValid(DCPoint square);
	bool SquareIsLegal(DCPoint square);
	
	DCPoint MakeDCPoint(int x, int y);
	
	GameState *gameState;
};

#endif // _doublecrossengine
