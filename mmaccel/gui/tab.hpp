#pragma once

#include "../platform.hpp"
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

		void insert( int index, boost::wstring_ref str )
		{
			std::wstring wstr( str );

			TCITEMW item;
			item.mask = TCIF_TEXT;
			item.pszText = &wstr[0];

			SendMessageW( ctrl_, TCM_INSERTITEMW, index, reinterpret_cast<LPARAM>( &item ) );
		}
	};

} // namespace mmaccel