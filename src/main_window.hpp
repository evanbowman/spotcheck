#pragma once

#include <gtkmm/filechooserdialog.h>
#include <pangomm/fontdescription.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stacksidebar.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/separator.h>
#include <gtkmm/textview.h>
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
		// copyable).
		main_window(const main_window &) = delete;
		main_window(const main_window &&) = delete;
		void operator=(const main_window &) = delete;
		void operator=(const main_window &&) = delete;
		virtual ~main_window();
		void notify();
	private:
		// In macOS and Windows, GUI code needs to happen only
		// on the main thread. The dispatcher member may be used
		// to communicate progress from the worker thread to the
		// main thread.
		Glib::Dispatcher m_dispatcher;
		// For GTKmm widget creation, if the widget needs to be
		// referenced later, include it as a member of the main
		// window. Otherwise, create it on the heap and tie its
		// lifetime to its parent with Gtk::manage(). This has
		// the benefit of abstracting data the people don't need
		// to see.
		Gtk::ProgressBar m_progress_bar;
		Gtk::StackSidebar m_sidebar;
		Gtk::Button m_run_button;
		Gtk::TextView m_console;
		Gtk::Stack m_stack;
		Gtk::Box m_box;
		void inflate_analysis_page();
		void inflate_preferences_page();
		void inflate_about_page();
		void window_set_default_properties();
		void init_console();
		void on_import_tiff_clicked();
		void on_import_gal_clicked();
		void on_worker_thread_msg();
	};
}
