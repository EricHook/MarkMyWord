#include "miniwin.h"
#include "DoubleCrossServer.h"
#include "RoomServer.h"

DoubleCrossServer::DoubleCrossServer()
{
	dictionary = new WGDict();
	dictionary->InitDict();
}

DoubleCrossServer::~DoubleCrossServer()
{
	if (dictionary) { delete dictionary; dictionary = NULL; }
}

void DoubleCrossServer::InitPlayerSpecificInfo(PlayerInfo *playerInfo)
{
}

void DoubleCrossServer::FillGameRecord(DoubleCrossServerPlayer *client, int *grLength, char *gameRecord)
{
	(*grLength) = 1;
	gameRecord[0] = 0;
}

int DoubleCrossServer::GetPreferredApproverSeat(int requesterSeat)
{
	return 0; // default approver for watchers
}

int DoubleCrossServer::GetPreferredInvitationSeat(int inviterSeat)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

void DoubleCrossServer::GetGameMessageDescription(int messageType, char *message, char *description)
{
	switch (messageType) {
		case MFS_DOUBLECROSS_RESETGAME: strcpy(description, "MFS_DOUBLECROSS_RESETGAME"); break;
		case MTS_DOUBLECROSS_REPLACEPLAYER: strcpy(description, "MTS_DOUBLECROSS_REPLACEPLAYER"); break;
		case MFS_DOUBLECROSS_REQUESTPLAY: strcpy(description, "MFS_DOUBLECROSS_REQUESTPLAY"); break;
		case MTS_DOUBLECROSS_PLAY: strcpy(description, "MTS_DOUBLECROSS_PLAY"); break;
		case MFS_DOUBLECROSS_EVENT: strcpy(description, "MFS_DOUBLECROSS_EVENT"); break;
		case MFS_DOUBLECROSS_EVENTS_DONE: strcpy(description, "MFS_DOUBLECROSS_EVENTS_DONE"); break;
		case MTS_DOUBLECROSS_DEFINITION: strcpy(description, "MTS_DOUBLECROSS_DEFINITION"); break;
		case MFS_DOUBLECROSS_DEFINITION: strcpy(description, "MFS_DOUBLECROSS_DEFINITION"); break;
		case MFS_DOUBLECROSS_REQUESTSHOWDOWN: strcpy(description, "MFS_DOUBLECROSS_REQUESTSHOWDOWN"); break;
		case MTS_DOUBLECROSS_SHOWDOWN: strcpy(description, "MTS_DOUBLECROSS_SHOWDOWN"); break;
		default:
			ASSERT(_WARN_, FALSE);
			strcpy(description, "unknown");
			break;
	}
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////

DblCrossPartWord::DblCrossPartWord()
{
	m_start.x = m_start.y = 0;
	m_finish.x = m_finish.y = 0;
	strcpy(m_word, "");
	m_direction = DBLCROSS_NODIR;
}

DblCrossPartWord::~DblCrossPartWord()
{
}		

int DblCrossPartWord::GetLength()
{
	if (m_direction == DBLCROSS_NODIR)
		return -1;
	
	if (m_direction == DBLCROSS_HORIZONTAL)
		return (m_finish.x - m_start.x) + 1;
	
	return (m_finish.y - m_start.y) + 1;
}

DblCrossPartWordList::DblCrossPartWordList(DoubleCrossServerTable *table)
{
	m_nextWord = 0;
	m_table = table;
}

DblCrossPartWordList::~DblCrossPartWordList()
{
}

void DblCrossPartWordList::Reset()
{
	m_nextWord = 0;
}

int DblCrossPartWordList::Size()
{
	if (m_nextWord == 0)
		return 0;
	
	return m_nextWord - 1; // assume the list is terminated
}

DblCrossPartWord *DblCrossPartWordList::Add(DCPoint start, DCPoint finish, char *word, DblCrossDirection direction)
{
	if (m_nextWord >= 50)
		{ ASSERT(_WARN_, FALSE); return NULL; }
	
	words[m_nextWord].m_start = start;
	words[m_nextWord].m_finish = finish;
	strnzcpy(words[m_nextWord].m_word, word, DBLCROSS_BOARDWIDTH);
	words[m_nextWord].m_direction = direction;
	
	m_nextWord++;
	
	return &words[m_nextWord - 1];
}

DblCrossPartWord *DblCrossPartWordList::At(int index)
{
	if (index < 0 || index >= 50)
		{ ASSERT(_WARN_, FALSE); return NULL; }
	
	return &words[index];
}

bool DblCrossPartWordList::CreateWordList(DCPoint square, DblCrossDirection whichDir)
{
	bool stableH = true, stableV = true;
	DblCrossPartWord wordH, wordV;
	
	/* Here the goal is to find all the substrings that match one of the
	   following criteria and add them to the word list:

		1. Begins with a stable letter so that no stable letters are
		   immediately to its left/top, and the word contains the letter at
		   position start

		or 2. Begins with the leftmost/topmost letter

		AND:
		1. Ends with a stable letter so that no stable letter is
		   immediately to its right/bottom, and the word contains the letter at
		   position start

		or 2. Ends with the rightmost/bottommost letter

		Any combo of the two top and two bottom criteria is allowed
	*/
	
	/*  For the given tile, we need to check that no illegal partial
	    words are created, and also that at least one direction has a
		stable letter in it.  If not, we return false from this function

		Also, if this function is being called as the result of a colored bomb,
		we need to stop looking if we hit any unstable tiles that are the color 
		that is being destroyed.  Reason:  these are slated for destruction,
		and must not be counted when determining partial words
	*/
	
	// make list of all partial words emanating from this tile in each direction
	
	if (whichDir == DBLCROSS_HORIZONTAL || whichDir == DBLCROSS_NODIR) {
		stableH = GetLongestWord(square, m_table->m_engine->MakeDCPoint(1, 0), &wordH);
		AddWordsToList(&wordH, square.x - wordH.m_start.x, m_table->m_engine->MakeDCPoint(1, 0));
	}
	
	if (whichDir == DBLCROSS_VERTICAL || whichDir == DBLCROSS_NODIR) {
		stableV = GetLongestWord(square, m_table->m_engine->MakeDCPoint(0, 1), &wordV);
		AddWordsToList(&wordV, square.y - wordV.m_start.y, m_table->m_engine->MakeDCPoint(0, 1));
	}
	
	// terminate the list
	
	Add(m_table->m_engine->MakeDCPoint(0, 0), m_table->m_engine->MakeDCPoint(0, 0), "", DBLCROSS_NODIR);
	
	// if there is no stable tile connected with this tile it's a bad move
	
	return stableH || stableV;
}

bool DblCrossPartWordList::GetLongestWord(DCPoint square, DCPoint step, DblCrossPartWord *word)
{
	bool hasStable = false;
	int i;
	DCPoint start, finish;
	
	// walk backwards until we get to the "edge" of the word
	
	for (start.x = square.x, start.y = square.y ;
		 start.x >= 0 && start.y >= 0 && !m_table->m_engine->IsSquareEmpty(start) && m_table->m_engine->GetSquareOwner(start) != -1 ;
		 start.x -= step.x, start.y -= step.y)
		if (m_table->IsMarkedForDestruction(start))
			break;
	
	// we walked one past the start
	
	if (step.x == 1 && start.x != square.x)
		start.x++;
	
	if (step.y == 1 && start.y != square.y)
		start.y++;
	
	// start[1] = square.y - pt.y;
	// offset[1] = pt.y;
	
	// copy string of letters
	
	for (i = 0, finish = start ;
		 finish.x < DBLCROSS_BOARDWIDTH && finish.y < DBLCROSS_BOARDHEIGHT &&
		 !m_table->m_engine->IsSquareEmpty(finish) && m_table->m_engine->GetSquareOwner(finish) != -1 ;
		 i++, finish.x += step.x, finish.y += step.y) {
		if (m_table->IsMarkedForDestruction(finish))
			break;
		word->m_word[i] = m_table->m_engine->GetSquareLetter(finish);
		if (m_table->m_engine->IsSquareStable(finish))
			hasStable = true;
	}
	
	word->m_word[i] = 0;
	
	word->m_start = start;
	word->m_finish = finish;
	word->m_direction = (step.x == 1) ? DBLCROSS_HORIZONTAL : DBLCROSS_VERTICAL;
	
	return hasStable;
}

void DblCrossPartWordList::AddWordsToList(DblCrossPartWord *word, int startOffset, DCPoint step)
{
	bool stable[DBLCROSS_BOARDWIDTH], foundStable = false;
	char s[256];
	int i, j, length;
	DCPoint spot;
	
	if (strlen(word->m_word) <= 1)
		return;
	
	length = strlen(word->m_word);
	for (i = 0, spot = word->m_start ; i < length ; i++) {
		stable[i] = m_table->m_engine->IsSquareStable(spot);
		spot.x += step.x;
		spot.y += step.y;
	}
	
	// outer loop: walk backwards and find each possible partial word start
	for (i = startOffset ; i >= 0 ; i--) {
		if (i == 0 || (foundStable && !stable[i])) {
			if (foundStable && !stable[i]) // we walked off the end of the stable run
				i++;
			// inner loop: for each possible partial word start, find each possible word end
			//			   that involves the placed tile
			for (foundStable = false, j = startOffset ; j < length ; j++) {
				if (j == (length - 1) || (foundStable && !stable[j])) {
					if (foundStable && stable[j] == 0)
						j--; // we walked off the end of the stable run
					
					// found a substring; add it to the list
					
					foundStable = false;
					strnzcpy(s, &word->m_word[i], (j - i) + 1);
					
					if (word->m_direction == DBLCROSS_HORIZONTAL)
						Add(m_table->m_engine->MakeDCPoint(word->m_start.x + i, word->m_start.y),
							m_table->m_engine->MakeDCPoint(word->m_start.x + j, word->m_start.y),
							s, word->m_direction);
					else
						Add(m_table->m_engine->MakeDCPoint(word->m_start.x, word->m_start.y + i),
							m_table->m_engine->MakeDCPoint(word->m_start.x, word->m_start.y + j),
							s, word->m_direction);
				}
				else
					if (stable[j])
						foundStable = true;
			}
		}
		else
			if (stable[i])
				foundStable = true;
	}
}

DblCrossPartWord *DblCrossPartWordList::FindSmallestPartWord(DblCrossDirection dir)
{
	int i, least = (DBLCROSS_BOARDWIDTH + 1), ind = -1;
	DblCrossPartWord *word;
	
	for (i = 0 ; i < m_nextWord ; i++) {
		word = At(i);
		if (word->m_direction == dir && word->GetLength() < least) {
			least = word->GetLength();
			ind = i;
		}
	}
	
	if (ind >= 0)
		return At(ind);
	
	return NULL;

}
