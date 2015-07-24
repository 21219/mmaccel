#pragma once

#include <string>
#include <array>
#include "../platform.hpp"

namespace winapi
{
	inline std::wstring get_module_file_name( HMODULE mod = nullptr ) noexcept
	{
		std::array< wchar_t, MAX_PATH * 2 > tmp;

		if( !GetModuleFileNameW( mod, &tmp[0], static_cast< DWORD >( tmp.size() ) ) ) {
			return{};
		}

		return{ &tmp[0] };
	}

	inline std::wstring get_module_path( HMODULE mod = nullptr ) noexcept
	{
		auto const name = get_module_file_name( mod );
		if( name.empty() ) {
			return{};
		}

		auto const pos = name.find_last_of( '\\' );
		if( pos == name.npos ) {
			return{};
		}

		return name.substr( 0, pos );
	}

	inline std::wstring get_system_directory() noexcept
	{
		std::array< wchar_t, MAX_PATH > tmp;
		auto const sz = GetSystemDirectoryW( &tmp[0], static_cast< UINT >( tmp.size() ) );
		if( sz == 0 ) {
			return{};
		}

		return{ &tmp[0] };
	}

} // namespace winapi