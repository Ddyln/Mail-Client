#include "Data.h"
#include "Control.h"
#include "View.h"

int main() {
	HideCursor(1);
	FixConsoleWindow();
	SetConsoleSize(1200, 700);
	while (true) {
		MainMenu();
	}
	return 0;
}