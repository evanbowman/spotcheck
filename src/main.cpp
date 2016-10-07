#include <gtkmm/settings.h>
#include <gtkmm/main.h>
#include <thread>

#include "main_window.hpp"

int main(int argc, char ** argv) {
	auto app = Gtk::Application::create(argc, argv);
	gui::main_window window;
	return app->run(window);
}
