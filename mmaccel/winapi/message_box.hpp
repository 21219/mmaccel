#pragma once

#include "../platform.hpp"
#include "string.hpp"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <sstream>

namespace winapi
{
	inline int message_box(boost::string_ref caption, boost::string_ref str, DWORD flag) noexcept
	{
		return MessageBoxW( nullptr, winapi::multibyte_to_widechar( str, CP_UTF8 ).c_str(), winapi::multibyte_to_widechar( caption, CP_UTF8 ).c_str(), flag );
	}

	inline void last_error_message_box( boost::string_ref caption, boost::string_ref str ) 
	{
		LPWSTR buffer;

		FormatMessageW( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			(LPWSTR)&buffer, 0, nullptr
		);

		std::unique_ptr< typename std::remove_pointer< LPWSTR >::type, decltype( &LocalFree ) > buf_ptr( buffer, &LocalFree );

		std::ostringstream oss;
		oss << str << " : " << winapi::widechar_to_multibyte( buf_ptr.get(), CP_UTF8 ) << std::endl;

		message_box( caption, oss.str().c_str(), MB_OK | MB_ICONERROR );
	}

} // namespace winapi