#pragma once

#include "parse_tiff.hpp"
#include "parse_gal.hpp"

namespace core {
	struct analysis_result {
		// TODO: implement
	};
	
	analysis_result analyze_unit(const work_unit & unit, const tiff_data & tiff);
}
