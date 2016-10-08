#include "main_window.hpp"

namespace gui {
	main_window::main_window() : m_workq(std::thread::hardware_concurrency()) {
		this->window_set_default_properties();
		this->add(m_box);
		m_box.pack_start(m_sidebar, Gtk::PACK_SHRINK);
		m_box.pack_start(m_stack);
		m_sidebar.set_stack(m_stack);
		m_stack.set_transition_type(Gtk::STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
		m_run_complete_dispatch.connect(sigc::mem_fun(*this, &main_window::on_run_complete));
		m_tiff_dispatch.connect(sigc::mem_fun(*this, &main_window::on_import_tiff_complete));
		m_gal_dispatch.connect(sigc::mem_fun(*this, &main_window::on_import_gal_complete));
		m_run_progress_dispatch.connect(sigc::mem_fun(*this, &main_window::on_run_progress));
		this->init_buttons();
		this->inflate_analysis_page();
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

	void main_window::on_import_gal_complete() {
		if (m_work_items) {
			m_console.append_line("[success] gal accepted!");
			if (m_tiff_data) {
				this->enable_run();
			}
		} else {
			m_console.append_line("[error] failed to parse gal");
			m_gal_btn.set_sensitive(true);
		}
	}

	void main_window::on_import_tiff_complete() {
		if (m_tiff_data) {
			m_console.append_line("[success] tiff accepted!");
			if (m_work_items) {
				this->enable_run();
			}
		} else {
			m_console.append_line("[error] failed to parse tiff");
			m_tiff_btn.set_sensitive(true);
		}
	}

	void main_window::on_run_progress() {
	    m_console.append_line("finished a work item...");
	}

	void main_window::on_run_complete() {
		m_console.append_line("run complete!");
	}

	void main_window::on_run_clicked() {
		for (auto & work_item : m_work_items.unwrap()) {
			m_workq.submit([this]() {
					// TODO: process work item and tiff
					this->notify_run_progress();
				});
		}
		// Because the work_queue is FIFO, last asynchronous request (below)
		// that checks for completion won't run until the work is complete
		// or nearly complete. Isn't that convenient!
		m_workq.submit([this]() {
				while (this->m_workq.has_work() /* or 1 active worker */) {
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				this->notify_run_complete();
			});
		m_run_btn.set_sensitive(false);
	}

	void main_window::enable_run() {
		m_run_btn.set_sensitive(true);
		m_console.append_line("\nReady to go! Click run to continue...");
	}

	void main_window::on_import_gal_clicked() {
		Gtk::FileChooserDialog dialog("Choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.set_transient_for(*this);
		dialog.add_button("cancel", Gtk::RESPONSE_CANCEL);
		dialog.add_button("open", Gtk::RESPONSE_OK);
		auto filter_gal = Gtk::FileFilter::create();
		filter_gal->set_name("gal files");
		filter_gal->add_pattern("*.gal");
		dialog.add_filter(filter_gal);
		int result = dialog.run();
		if (dialog.run() == Gtk::RESPONSE_OK) {
			m_gal_btn.set_sensitive(false);
			const std::string path = dialog.get_filename();
			m_workq.submit([&path, this]() {
					this->m_work_items = core::parse_gal(path);
					this->notify_imprt_gal_complete();
				});
		}
	}

	void main_window::on_import_tiff_clicked() {
		Gtk::FileChooserDialog dialog("Choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.set_transient_for(*this);
		dialog.add_button("cancel", Gtk::RESPONSE_CANCEL);
		dialog.add_button("open", Gtk::RESPONSE_OK);
		auto filter_tiff = Gtk::FileFilter::create();
		filter_tiff->set_name("tiff files");
		filter_tiff->add_pattern("*.tiff");
		dialog.add_filter(filter_tiff);
		if (dialog.run() == Gtk::RESPONSE_OK) {
			m_tiff_btn.set_sensitive(false);
			const std::string path = dialog.get_filename();
			m_workq.submit([&path, this]() {
					this->m_tiff_data = core::parse_tiff(path);
					this->notify_imprt_tiff_complete();
				});
		}
	}

	template <int left, int right, int top, int bottom, typename T>
	void apply_margin(T & widget) {
		widget.set_margin_left(left);
		widget.set_margin_right(right);
		widget.set_margin_top(top);
		widget.set_margin_bottom(bottom);
	}

	void main_window::inflate_analysis_page() {
		Gtk::Box * box = Gtk::manage(new Gtk::Box);
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		Gtk::Box * frames_box = Gtk::manage(new Gtk::Box);
		Gtk::Frame * tiff_frame = Gtk::manage(new Gtk::Frame);
		Gtk::Frame * gal_frame = Gtk::manage(new Gtk::Frame);
		Gtk::Label * tiff_label = Gtk::manage(new Gtk::Label);
		Gtk::Label * gal_label = Gtk::manage(new Gtk::Label);
		Gtk::Box * gal_box = Gtk::manage(new Gtk::Box);
		Gtk::Box * tiff_box = Gtk::manage(new Gtk::Box);
		tiff_label->set_text("Choose a height map for analysis");
		apply_margin<4, 4, 10, 8>(*tiff_label);
		tiff_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		tiff_box->pack_start(*tiff_label);
		tiff_box->pack_start(m_tiff_btn, Gtk::PACK_SHRINK);
		tiff_frame->add(*tiff_box);
		tiff_frame->set_border_width(10);
		frames_box->pack_start(*tiff_frame);
		gal_label->set_text("Choose a corresponding metadata file");		
		apply_margin<4, 4, 10, 8>(*gal_label);
		gal_box->set_orientation(Gtk::ORIENTATION_VERTICAL);
		gal_box->pack_start(*gal_label);
		gal_box->pack_start(m_gal_btn, Gtk::PACK_SHRINK);
		gal_frame->add(*gal_box);
		gal_frame->set_border_width(10);
		frames_box->pack_start(*gal_frame);
		box->pack_start(*frames_box, Gtk::PACK_SHRINK);
		apply_margin<10, 10, 0, 8>(m_console);
		box->pack_start(m_console);
		Gtk::Box * footer_box = Gtk::manage(new Gtk::Box);
		Gtk::Box * whitespace_left = Gtk::manage(new Gtk::Box);
		Gtk::Box * whitespace_right = Gtk::manage(new Gtk::Box);
		footer_box->pack_start(*whitespace_left);
		footer_box->pack_start(m_run_btn, Gtk::PACK_SHRINK);
		footer_box->pack_start(*whitespace_right);
		box->pack_start(*footer_box, Gtk::PACK_SHRINK);
		static const char * PAGE_NAME = "Analyze";
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

	void main_window::init_buttons() {
		m_run_btn.set_label("run");
		m_run_btn.set_sensitive(false);
		apply_margin<0, 0, 0, 6>(m_run_btn);
		apply_margin<4, 4, 4, 4>(m_gal_btn);
		apply_margin<4, 4, 4, 4>(m_tiff_btn);
		m_tiff_btn.set_label("import .tiff");
		m_gal_btn.set_label("import .gal");
		m_tiff_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_import_tiff_clicked));
		m_gal_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_import_gal_clicked));
		m_run_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_run_clicked));
	}

	void main_window::prepare_new_run() {
		m_tiff_data = {};
		m_work_items = {};
		m_run_btn.set_sensitive(false);
		m_tiff_btn.set_sensitive(true);
		m_gal_btn.set_sensitive(true);
	}

	void main_window::notify_run_progress() {
		m_run_progress_dispatch.emit();
	}

	void main_window::notify_run_complete() {
		m_run_complete_dispatch.emit();
	}

	void main_window::notify_imprt_tiff_complete() {
		m_tiff_dispatch.emit();
	}

	void main_window::notify_imprt_gal_complete() {
		m_gal_dispatch.emit();
	}
	
	main_window::~main_window() {}
}
