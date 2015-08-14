#pragma once

#include "../platform.hpp"
#include "../winapi/string.hpp"
#include <boost/utility/string_ref.hpp>
#include <boost/optional.hpp>

namespace mmaccel
{
	template <DWORD ID>
	class tabs
	{
		HWND ctrl_;

	public:
		tabs() = default;

		tabs(HWND parent) :
			ctrl_( GetDlgItem( parent, ID ) )
		{ }

		std::size_t size() const noexcept
		{
			return static_cast<std::size_t>( SendMessageW( ctrl_, TCM_GETITEMCOUNT, 0, 0 ) );
		}

		void clear() noexcept
		{
			SendMessageW( ctrl_, TCM_DELETEALLITEMS, 0, 0 );
		}

		boost::optional< int > current_index() const noexcept
		{
			auto const index = SendMessageW( ctrl_, TCM_GETCURSEL, 0, 0 );
			if( index == -1 ) {
				return{};
			}

			return index;
		}

		void set_current_index( int index ) noexcept
		{
			SendMessageW( ctrl_, TCM_SETCURSEL, index, 0 );
		}

		void insert( int index, boost::string_ref str )
		{
			std::wstring wstr( winapi::multibyte_to_widechar( str, CP_UTF8 ) );

			TCITEMW item;
			item.mask = TCIF_TEXT;
			item.pszText = &wstr[0];

			SendMessageW( ctrl_, TCM_INSERTITEMW, index, reinterpret_cast<LPARAM>( &item ) );
		}

		void push_back(boost::string_ref str)
		{
			insert( size(), str );
		}

		HWND handle() const noexcept
		{
			return ctrl_;
		}
	};

} // namespace mmaccel