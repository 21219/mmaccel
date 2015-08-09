#pragma once

#include "../platform.hpp"
#include <vector>
#include <string>
#include <boost/utility/string_ref.hpp>

namespace winapi
{
	inline std::wstring multibyte_to_widechar(boost::string_ref src, int code)
	{
		auto const sz = MultiByteToWideChar( code, 0, src.data(), static_cast< int >( src.size() ), nullptr, 0 );
		if( !sz ) {
			return{};
		}

		std::wstring dst;
		dst.resize( sz );
		auto const result = MultiByteToWideChar( code, 0, src.data(), static_cast< int >( src.size() ), &dst[0], static_cast< int >( dst.size() ) );
		if( !result ) {
			return{};
		}

		return dst;
	}

	inline std::string widechar_to_multibyte( boost::wstring_ref src, int code )
	{
		auto const sz = WideCharToMultiByte( code, 0, src.data(), static_cast<int>( src.size() ), nullptr, 0, nullptr, nullptr );
		if( !sz ) {
			return{};
		}

		std::string dst;
		dst.resize( sz );
		auto const result = WideCharToMultiByte( code, 0, src.data(), static_cast<int>( src.size() ), &dst[0], static_cast<int>( dst.size() ), nullptr, nullptr );
		if( !result ) {
			return{};
		}

		return dst;
	}

	inline std::string convert_string( boost::string_ref src, int src_enc, int dst_enc )
	{
		return widechar_to_multibyte( multibyte_to_widechar( src, src_enc ), dst_enc );
	}

} // namespace winapi