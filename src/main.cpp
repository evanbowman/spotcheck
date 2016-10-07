#include <gtkmm/settings.h>
#include <gtkmm/main.h>

#include "gui/main_window.hpp"

int main(int argc, char ** argv) {
	auto app = Gtk::Application::create(argc, argv);
	gui::main_window window;
	return app->run(window);
}
