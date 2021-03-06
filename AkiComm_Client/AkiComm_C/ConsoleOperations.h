#ifndef _CONSOLE_OPERATIONS_H
#define _CONSOLE_OPERATIONS_H
#include <windows.h>

/*=========================*\
||Quick look-up Information||
\*=========================*/

// -- Keyboard inputs -- //
// Enter: 013
// Escape: 027
// Up Arrow: 072
// Down Arrow: 080
// Right Arrow: 077
// Left Arrow: 075

//===========================


/*=================*\
||Color definitions||
\*=================*/

#define Black       0x0000
#define DarkGray    FOREGROUND_INTENSITY

#define HalfBlue        FOREGROUND_BLUE
#define FullBlue       FOREGROUND_BLUE | FOREGROUND_INTENSITY

#define HalfGreen       FOREGROUND_GREEN
#define FullGreen      FOREGROUND_GREEN | FOREGROUND_INTENSITY

#define HalfRed         FOREGROUND_RED
#define FullRed        FOREGROUND_RED | FOREGROUND_INTENSITY

#define HalfTeal        FOREGROUND_BLUE | FOREGROUND_GREEN
#define FullTeal       FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#define HalfPurple      FOREGROUND_BLUE | FOREGROUND_RED
#define FullPurple     FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY

#define HalfYellow      FOREGROUND_GREEN | FOREGROUND_RED
#define FullYellow     FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY

#define LightGray   FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
#define White       FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY

//===========================


#define Max 0 // Provides a word to designate maximum screen size, 0 can be used if preferred.


class ConsoleOperationsImp;

class __declspec(dllexport) ConsoleOperations
{
public:
	// Constructors and Destructors//
	ConsoleOperations();
	~ConsoleOperations();

	// Clear Screen functions //
	void ClearScreen();
	void ClearScreen(unsigned short bgColor);

	// Defaulting functions //
	void SetDefaultColor();
	void SetDefaultCursorSize();
	void SetDefaultScreenSize();



	///////////////////////////
	// === Get functions === //
	///////////////////////////

	// Get Color functions  //
	unsigned short GetColorCell();
	unsigned short GetColorText();

	// Get Cursor functions //
	char GetCursorChar();
	short GetCursorSize();



	//////////////////////////////
	// === Retrieve functions === //
	//////////////////////////////

	// Color functions --
	void RetrieveColors(unsigned short& fgColor, unsigned short& bgColor);

	// Cursor functions --
	void RetrieveCursorCoords(short& X, short& Y);

	// Screen functions --
	void RetrieveScreenSize(short& X, short& Y);



	///////////////////////////
	// === Set functions === //
	///////////////////////////

	// Set Color functions //
	void SetColor(unsigned short fgColor, unsigned short bgColor);
	void SetColorCell(unsigned short bgColor);
	void SetColorText(unsigned short fgColor);

	// Set Cursor functions //
	void SetCursorCoords(short X, short Y);
	void SetCursorHome();
	void SetCursorSize(short size);

	// Set Screen funtions //
	void SetScreenSize(short X, short Y);

private:
	ConsoleOperationsImp *ConOpImp;
};

#endif