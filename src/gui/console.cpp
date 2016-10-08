#include "console.hpp"

namespace gui {
	console::console() {
		m_textview.set_editable(false);
		Pango::FontDescription font_descr("monospace 11");
		m_textview.override_font(font_descr);
		Gdk::RGBA rgba;
		rgba.set_rgba(0.0, 0.169, 0.212);
		m_textview.override_background_color(rgba);
		rgba.set_rgba(0.514, 0.580, 0.588);
		m_textview.override_color(rgba);
		auto buffer = m_textview.get_buffer();
		buffer->create_mark("last_line", buffer->end(), true);
		// Gtk::TextView::set_*_margin are confusingly named, they
		// actually set the padding.
		static const int padding = 6;
		m_textview.set_bottom_margin(padding);
		m_textview.set_top_margin(padding);
		m_textview.set_left_margin(padding);
		m_textview.set_right_margin(padding);
		this->add(m_textview);
	}

	void console::append_line(const std::string & str) {
	    auto buffer = m_textview.get_buffer();
		buffer->insert_at_cursor(str + "\n");
		Gtk::TextIter it = buffer->end();
		it.set_line_offset(0);
		auto mark = buffer->get_mark("last_line");
		buffer->move_mark(mark, it);
		m_textview.scroll_to(mark);
	}

	console::~console() {}
}
