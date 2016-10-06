#pragma once

#include <gtkmm/stacksidebar.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/stack.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <array>

namespace gui {
	class main_window : public Gtk::Window {
    public:
	    main_window();
		// The main window should be created once, and should
		// never need to be moved or copied. If you can think
		// of a good reason why it needs to be, we can allow
		// implementation of move operations (Gtk::Window non-
		// copyable)
	    main_window(const main_window &) = delete;
	    main_window(const main_window &&) = delete;
	    void operator=(const main_window &) = delete;
		void operator=(const main_window &&) = delete;
		virtual ~main_window();
	private:
		Gtk::Box m_box;
		Gtk::StackSidebar m_sidebar;
		Gtk::Separator m_separator;
		Gtk::Stack m_stack;
	};
}
