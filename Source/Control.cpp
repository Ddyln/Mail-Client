#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

void GotoXY(int column, int line) {
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
