#pragma once

#include "parse_tiff.hpp"
#include "parse_gal.hpp"

namespace core {
	struct analysis_result {
		// TODO: implement
	};
	
	analysis_result analyze_group(const work_group & group, const tiff_data & tiff);
}
