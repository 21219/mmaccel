#pragma once

#include "../platform.hpp"
#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>

namespace mmaccel
{
	template <DWORD ID>
	class list_view
	{
		HWND ctrl_;

	public:
		list_view() = default;

		list_view(HWND parent) :
			ctrl_( GetDlgItem( parent, ID ) )
		{ }

		void clear() noexcept
		{
			SendMessageW( ctrl_, LVM_DELETEALLITEMS, 0, 0 );
		}

		boost::optional< int > current_index() const noexcept
		{
			auto const index = SendMessageW( ctrl_, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED );
			if( index == -1 ) {
				return{};
			}

			return index;
		}

		boost::optional< RECT > get_subitem_rect( int item, int subitem ) const noexcept
		{
			RECT rc;
			rc.top = subitem;
			rc.left = LVIR_BOUNDS;

			if( !SendMessageW( ctrl_, LVM_GETSUBITEMRECT, item, reinterpret_cast<LPARAM>( &rc ) ) ) {
				return{};
			}

			return rc;
		}

		void insert_column( int index, boost::wstring_ref str, int cx )
		{
			std::wstring wstr( str.data() );

			LVCOLUMNW col;
			col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			col.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
			col.pszText = &wstr[0];
			col.cchTextMax = wstr.size();
			col.cx = cx;

			SendMessageW( ctrl_, LVM_INSERTCOLUMNW, index, reinterpret_cast<LPARAM>( &col ) );
		}

		void insert( int index, int sub_index, boost::wstring_ref str )
		{
			std::wstring wstr( str.data() );

			LVITEMW item;
			item.iSubItem = sub_index;
			item.mask = LVIF_TEXT;
			item.pszText = &wstr[0];

			SendMessageW( ctrl_, LVM_SETITEMTEXTW, index, reinterpret_cast<LPARAM>( &item ) );
		}
	};

} // namespace mmaccel