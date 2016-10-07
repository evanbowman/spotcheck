#include "main_window.hpp"

namespace gui {
	main_window::main_window() {
		this->window_set_default_properties();
		this->add(m_box);
		m_box.pack_start(m_sidebar, Gtk::PACK_SHRINK);
		m_box.pack_start(m_stack, Gtk::PACK_EXPAND_WIDGET);
		m_stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
		m_sidebar.set_stack(m_stack);
		this->inflate_analysis_page();
		this->inflate_history_page();
		this->inflate_preferences_page();
		this->inflate_about_page();
		this->show_all();
	}

	void main_window::window_set_default_properties() {
		static const uint16_t DEFAULT_WIDTH = 800;
		static const uint16_t DEFAULT_HEIGHT = 520;
		this->set_default_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		this->set_size_request(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		this->set_position(Gtk::WIN_POS_CENTER);
	}

	void main_window::inflate_analysis_page() {
		Gtk::Box * box = Gtk::manage(new Gtk::Box);
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		Gtk::Widget * button_box = Gtk::manage(new Gtk::Box);
		Gtk::Widget * analyze_btn = Gtk::manage(new Gtk::Button);
		Gtk::Widget * tiff_btn = Gtk::manage(new Gtk::Button);
		Gtk::Widget * gal_btn = Gtk::manage(new Gtk::Button);
		static_cast<Gtk::Button *>(analyze_btn)->set_label("analyze");
	    static_cast<Gtk::Button *>(tiff_btn)->set_label("import tiff");
	    static_cast<Gtk::Button *>(gal_btn)->set_label("import gal");
		static_cast<Gtk::Box *>(button_box)->pack_start(*tiff_btn, Gtk::PACK_SHRINK);
		static_cast<Gtk::Box *>(button_box)->pack_start(*gal_btn, Gtk::PACK_SHRINK);
		box->pack_start(*button_box, Gtk::PACK_SHRINK);
		box->pack_start(*analyze_btn, Gtk::PACK_SHRINK);
		static const char * PAGE_NAME = "Analyze";
		m_stack.add(*box, PAGE_NAME, PAGE_NAME);
	}

	void main_window::inflate_history_page() {
		Gtk::Box * box = Gtk::manage(new Gtk::Box);
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		static const char * PAGE_NAME = "History";
		m_stack.add(*box, PAGE_NAME, PAGE_NAME);
	}

	void main_window::inflate_preferences_page() {
		Gtk::Box * box = Gtk::manage(new Gtk::Box);
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		static const char * PAGE_NAME = "Preferences";
		m_stack.add(*box, PAGE_NAME, PAGE_NAME);
	}

	void main_window::inflate_about_page() {
		Gtk::Box * box = Gtk::manage(new Gtk::Box);
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		static const char * PAGE_NAME = "About";
		m_stack.add(*box, PAGE_NAME, PAGE_NAME);
	}

	main_window::~main_window() {}
}
