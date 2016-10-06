#include "main_window.hpp"

static const std::array<const char *, 3> PAGE_NAMES = {
	"Analyze", "History", "Preferences"
};

namespace gui {
	main_window::main_window() :
		m_separator(Gtk::ORIENTATION_VERTICAL)
	{
		const auto screen_ref = this->get_screen();
		static const float INIT_SCALE = 0.75f;
		const int default_width = screen_ref->get_width() * INIT_SCALE;
		const int default_height = screen_ref->get_height() * INIT_SCALE;
		this->set_default_size(default_width, default_height);
		this->set_position(Gtk::WIN_POS_CENTER);
		this->add(m_box);
		m_box.pack_start(m_sidebar, Gtk::PACK_SHRINK);
		m_box.pack_start(m_separator, Gtk::PACK_SHRINK);
		m_box.pack_start(m_stack, Gtk::PACK_EXPAND_WIDGET);
		m_stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
		m_sidebar.set_stack(m_stack);
		for (int i = 0; i < 3; i += 1) {
			Gtk::Widget * box = Gtk::manage(new Gtk::Box);
			m_stack.add(*box, ::PAGE_NAMES[i], ::PAGE_NAMES[i]);
		}
		this->show_all();
	}
	
    main_window::~main_window() {}
}
