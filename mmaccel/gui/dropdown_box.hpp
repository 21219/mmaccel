#pragma once

#include "../platform.hpp"
#include "../winapi/string.hpp"
#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>

namespace mmaccel
{
	template <DWORD ID>
	class dropdown_box
	{
		HWND ctrl_;

	public:
		dropdown_box() = default;

		dropdown_box( HWND parent ) :
			ctrl_( GetDlgItem( parent, ID ) )
		{}

		void clear() noexcept
		{
			SendMessageW( ctrl_, CB_RESETCONTENT, 0, 0 );
		}

		boost::optional< int > current_index() const noexcept
		{
			int const index = static_cast< int >( SendMessageW( ctrl_, CB_GETCURSEL, 0, 0 ) );
			if( index == CB_ERR ) {
				return{};
			}

			return index;
		}

		void set_current_index( int index ) noexcept
		{
			SendMessageW( ctrl_, CB_SETCURSEL, index, 0 );
		}

		void push_back( boost::string_ref str )
		{
			std::wstring  wstr( winapi::multibyte_to_widechar( str, CP_UTF8 ) );
			SendMessageW( ctrl_, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>( &wstr[0] ) );
		}

		HWND handle() const noexcept
		{
			return ctrl_;
		}
	};

} // namespace mmaccel