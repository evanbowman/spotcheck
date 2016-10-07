#pragma once

#include <string>

#include "option.hpp"

namespace core {
	struct tiff_data {
		// Implement the structure...
	};

	option<tiff_data> parse_tiff(const std::string & filename);
}

