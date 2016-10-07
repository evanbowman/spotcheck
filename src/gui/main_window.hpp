#pragma once

#include <gtkmm/filechooserdialog.h>
#include <pangomm/fontdescription.h>
#include <gtkmm/stacksidebar.h>
#include <glibmm/dispatcher.h>
#include <gtkmm/progressbar.h>
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

#include "../core/work_queue.hpp"
#include "../core/option.hpp"
#include "console.hpp"

// templates instantiated with TODO require an unimplemented type
using TODO = int;

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
		void notify_run_status();
		void notify_imprt_tiff_complete();
		void notify_imprt_gal_complete();
	private:
		// In macOS and Windows, GUI code needs to happen only
		// on the main thread. The dispatcher member may be used
		// to communicate progress from the worker thread to the
		// main thread.
		Glib::Dispatcher m_run_dispatch;
		Glib::Dispatcher m_tiff_dispatch;
		Glib::Dispatcher m_gal_dispatch;
		// For GTKmm widget creation, if the widget needs to be
		// referenced later, include it as a member of the main
		// window. Otherwise, create it on the heap and tie its
		// lifetime to its parent with Gtk::manage(). This has
		// the benefit of abstracting data the people don't need
		// to see.
		Gtk::StackSidebar m_sidebar;
		// The member m_workq provides an abstraction for a
		// threadpool. Submitting work to it is easy, just call
		// its submit method with a callback (lamdas are easiest),
		// it's as simple as that. Never do long running
		// calculations within a gtk widget signal handler, or the
		// app will become unresponsive. Delegate work to the thread
		// pool instead!
		core::work_queue m_workq;
		Gtk::Button m_run_btn;
		Gtk::Button m_tiff_btn;
		Gtk::Button m_gal_btn;
		Gtk::Stack m_stack;
		// The member m_console encapsulates a scrollable textview,
		// and has methods that make it easy to print messages to
		// it. While a thread is running a calculation, be nice
		// to the user and print some incremental status messages
		// to the console!
		console m_console;
		Gtk::Box m_box;
		core::option<TODO> m_tiff_data;
		core::option<TODO> m_gal_data;
		void prepare_new_run();
		void inflate_analysis_page();
		void inflate_preferences_page();
		void inflate_about_page();
		void window_set_default_properties();
		void on_import_tiff_clicked();
		void on_import_gal_clicked();
		void on_import_tiff_complete();
		void on_import_gal_complete();
		void on_run_clicked();
		void on_run_msg();
		void init_buttons();
		void enable_run();
	};
}