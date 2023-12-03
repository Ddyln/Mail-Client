#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

int main() {
	HideCursor(1);
	FixConsoleWindow();
	FontSize(21);
	SetConsoleSize(1200, 700); 
	ifstream f{ "config.json" };
	auto tmp = nlohmann::json::parse(f);
	f.close();
	CONFIG cnf = {
		tmp["username"],
		tmp["mail"],
		tmp["password"],
		tmp["server"],
		tmp["SMTP"],
		tmp["POP3"],
		tmp["autoload"]
	};
	LIST mail;
	Init(mail, cnf);

	while (true) {
		MainMenu(mail, cnf);
	}
	return 0;
}