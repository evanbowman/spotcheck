#pragma once

#include <vector>

#include "option.hpp"

namespace core {
	struct work_group {
		// Implement the structure...
	};

	option<std::vector<work_group>> parse_gal(const std::string & filename);
}

