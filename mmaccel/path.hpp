#pragma once

#include <string>
#include "winapi/directory.hpp"

namespace mmaccel
{
	inline std::wstring const& dll_path()
	{
		static std::wstring path( winapi::get_module_path() + L"\\mmaccel\\mmaccel.dll");
		return path;
	}

} // namespace mmaccel