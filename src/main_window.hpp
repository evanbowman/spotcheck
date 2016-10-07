#pragma once

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stacksidebar.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/stack.h>
#include <gtkmm/image.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <iostream>
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
		Gtk::StackSidebar m_sidebar;
		Gtk::Stack m_stack;
		Gtk::Box m_box;
		void inflate_analysis_page();
		void inflate_preferences_page();
		void inflate_about_page();
		void window_set_default_properties();
		void init_sidebar();
		void on_import_tiff_clicked();
		void on_import_gal_clicked();
	};
}
