#pragma once

#include "../platform.hpp"
#include "directory.hpp"
#include "overlapped.hpp"
#include <vector>
#include <string>

namespace winapi
{
	struct file_notify_information_buffer
	{
		std::vector< BYTE > buf;

		file_notify_information_buffer( std::size_t sz ) :
			buf( sz )
		{ }
	};

	inline bool read_directory_change(directory_handle const& dir, file_notify_information_buffer& fnib, bool subtree, DWORD filter, overlapped_t& ol) noexcept
	{
		return ReadDirectoryChangesW( 
			dir.get(), &fnib.buf[0], static_cast< DWORD >( fnib.buf.size() ), static_cast< BOOL >( subtree ), filter, nullptr, &ol.obj, nullptr
		) != 0;
	}

	template <typename F>
	inline void enum_file_notify_information(file_notify_information_buffer const& fnib, F&& f)
	{
		auto data = reinterpret_cast<FILE_NOTIFY_INFORMATION const*>( &fnib.buf[0] );

		for( ;; ) {
			std::wstring str( data->FileName, data->FileName + ( data->FileNameLength / sizeof( wchar_t ) ) );

			if( f( str ) ) {
				break;
			}
			if( data->NextEntryOffset == 0 ) {
				break;
			}

			data = reinterpret_cast<FILE_NOTIFY_INFORMATION const* >( reinterpret_cast<BYTE const*>( data ) + data->NextEntryOffset );
		}
	}

} // namespace winapi