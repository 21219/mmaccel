#pragma once

#include <string>
#include <array>
#include "../platform.hpp"
#include "handle.hpp"
#include "string.hpp"

namespace winapi
{
	inline std::string get_module_file_name( HMODULE mod = nullptr ) noexcept
	{
		std::array< wchar_t, MAX_PATH * 2 > tmp;

		if( !GetModuleFileNameW( mod, &tmp[0], static_cast< DWORD >( tmp.size() ) ) ) {
			return{};
		}

		return winapi::widechar_to_multibyte( &tmp[0], CP_UTF8 );
	}

	inline std::string get_module_path( HMODULE mod = nullptr ) noexcept
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

	inline std::string get_system_directory() noexcept
	{
		std::array< wchar_t, MAX_PATH > tmp;
		auto const sz = GetSystemDirectoryW( &tmp[0], static_cast< UINT >( tmp.size() ) );
		if( sz == 0 ) {
			return{};
		}

		return winapi::widechar_to_multibyte( &tmp[0], CP_UTF8 );
	}

	class directory_handle
	{
		unique_handle handle_;

	public:
		directory_handle() = default;
		directory_handle( directory_handle&& ) = default;
		directory_handle& operator=( directory_handle&& ) = default;

		directory_handle(boost::string_ref path, DWORD share, DWORD disposition, DWORD attr, SECURITY_ATTRIBUTES* sa = nullptr) :
			handle_( CreateFileW( winapi::multibyte_to_widechar( path, CP_UTF8 ).c_str(), FILE_LIST_DIRECTORY, share, sa, disposition, attr, nullptr ) )
		{ }

		HANDLE get() const noexcept
		{
			return handle_.get();
		}
	};

} // namespace winapi