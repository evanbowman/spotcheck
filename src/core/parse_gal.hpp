#pragma once

#include <type_traits>
#include <string>

#include "option.hpp"

namespace core {
	struct gal_data {
		gal_data() {
			static_assert(!std::is_pod<gal_data>::value, "please make structs trivially constructible");
		}
		// Implement the structure...
	};

	option<gal_data> parse_gal(const std::string & filename);
}

