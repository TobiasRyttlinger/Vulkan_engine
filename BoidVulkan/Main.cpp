
#pragma comment(lib, "shell32.lib")
#include "app.h"

//std
#include<cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	srand(time(NULL));
	lve::App app{};

	try {
		app.run();

	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}