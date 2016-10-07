#pragma once

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <string>

namespace gui {
	class console : public Gtk::ScrolledWindow {
	public:
		console();
		void append_line(const std::string &);
		virtual ~console();
	private:
		Gtk::TextView m_textview;
	};
}
