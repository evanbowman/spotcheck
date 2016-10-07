#pragma once

#include <type_traits>
#include <string>

#include "option.hpp"

namespace core {
	struct tiff_data {
		tiff_data() {
			static_assert(!std::is_pod<tiff_data>::value, "please make structs trivially constructible");
		}
		// Implement the structure...
	};

	option<tiff_data> parse_tiff(const std::string & filename);
}

