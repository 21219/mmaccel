#pragma once

#include "../platform.hpp"
#include <boost/utility/string_ref.hpp>
#include <memory>
#include <sstream>

namespace winapi
{
	inline int message_box(boost::wstring_ref caption, boost::wstring_ref str, DWORD flag) noexcept
	{
		return MessageBoxW( nullptr, str.data(), caption.data(), flag );
	}

	inline void last_error_message_box( boost::wstring_ref caption, boost::wstring_ref str ) 
	{
		LPWSTR buffer;

		FormatMessageW( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			(LPWSTR)&buffer, 0, nullptr
		);

		std::unique_ptr< typename std::remove_pointer< LPWSTR >::type, decltype( &LocalFree ) > buf_ptr( buffer, &LocalFree );

		std::wostringstream oss;
		oss << str.data() << " : " << buf_ptr.get() << std::endl;

		message_box( caption.data(), oss.str().c_str(), MB_OK | MB_ICONERROR );
	}

} // namespace winapi