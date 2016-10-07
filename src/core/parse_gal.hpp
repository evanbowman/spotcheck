#pragma once

#include <string>

#include "option.hpp"

namespace core {
	struct gal_data {
		// Implement the structure...
	};

	option<gal_data> parse_gal(const std::string & filename);
}

