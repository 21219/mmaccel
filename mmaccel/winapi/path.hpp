#pragma once

#include "../platform.hpp"
#include "string.hpp"

namespace winapi {

	inline bool path_file_exists(boost::string_ref path) noexcept
	{
		return PathFileExistsW( multibyte_to_widechar( path, CP_UTF8 ).c_str() ) == TRUE;
	}

} // namespace winapi