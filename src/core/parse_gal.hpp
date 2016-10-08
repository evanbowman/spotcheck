#pragma once

#include <vector>

#include "option.hpp"

namespace core {
	struct work_unit {
		// Implement the structure...
	};

	option<std::vector<work_unit>> parse_gal(const std::string & filename);
}

