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
#include <cmath>

#include "../core/analyze_group.hpp"
#include "../core/work_queue.hpp"
#include "../core/parse_tiff.hpp"
#include "../core/parse_gal.hpp"
#include "../core/option.hpp"
#include "console.hpp"

namespace gui {
	class main_window : public Gtk::Window {
	public:
		main_window();
		main_window(const main_window &&) = delete;
		void operator=(const main_window &&) = delete;
		virtual ~main_window();
		void notify_run_progress();
		void notify_run_complete();
		void notify_imprt_tiff_complete();
		void notify_imprt_gal_complete();
	private:
		Glib::Dispatcher m_run_complete_dispatch;
		Glib::Dispatcher m_run_progress_dispatch;
		Glib::Dispatcher m_tiff_dispatch;
		Glib::Dispatcher m_gal_dispatch;
		Gtk::StackSidebar m_sidebar;
		core::work_queue m_workq;
		Gtk::Button m_run_btn;
		Gtk::Button m_tiff_btn;
		Gtk::Button m_gal_btn;
		Gtk::Stack m_stack;
		console m_console;
		Gtk::Box m_box;
		core::option<core::tiff_data> m_tiff_data;
		core::option<std::vector<core::work_group>> m_work_groups;
		std::vector<core::analysis_result> m_analysis_results;
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
		void on_run_progress();
		void on_run_complete();
		void init_buttons();
		void enable_run();
	};
}
