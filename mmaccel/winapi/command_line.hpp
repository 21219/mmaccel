#pragma once

#include "../platform.hpp"
#include "memory.hpp"
#include <vector>
#include <string>

namespace winapi
{
	inline std::vector< std::wstring > get_command_line_args()
	{
		using args_ptr = std::unique_ptr< LPWSTR, local_free_deleter >;

		int sz = 0;
		auto args = args_ptr( CommandLineToArgvW( GetCommandLineW(), &sz ) );
		
		std::vector< std::wstring > result;

		for( int i = 0; i < sz; ++i ) {
			result.emplace_back( args.get()[i] );
		}

		return result;
	}

} // namespace winapi