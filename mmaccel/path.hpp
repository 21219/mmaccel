#pragma once

#include <string>
#include "winapi/directory.hpp"

namespace mmaccel
{
	inline std::string const& dll_path()
	{
		static std::string path( winapi::get_module_path() + "\\mmaccel\\mmaccel.dll");
		return path;
	}

} // namespace mmaccel