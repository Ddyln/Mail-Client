#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

int main() {
	HideCursor(1);
	FixConsoleWindow();
	FontSize(21);
	SetConsoleSize(1200, 700); 
	LIST mail;
	Init(mail);
	return 0;
	ifstream f{ "config.json" };
	auto tmp = nlohmann::json::parse(f);
	CONFIG cnf = {
		tmp["username"],
		tmp["mail"],
		tmp["password"],
		tmp["server"],
		tmp["SMTP"],
		tmp["POP3"],
		tmp["autoload"]
	};
	while (true) {
		MainMenu(cnf);
	}
	return 0;
}