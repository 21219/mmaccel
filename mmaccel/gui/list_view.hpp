#pragma once

#include "../platform.hpp"
#include "../winapi/string.hpp"
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
		{ 
			auto const ex = SendMessageW( ctrl_, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0 );
			SendMessageW( ctrl_, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, ex | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
		}

		void clear() noexcept
		{
			if( size() == 0 ) {
				return;
			}
			SendMessageW( ctrl_, LVM_DELETEALLITEMS, 0, 0 );
		}

		std::size_t size() const noexcept
		{
			return SendMessageW( ctrl_, LVM_GETITEMCOUNT, 0, 0 );
		}

		boost::optional< int > current_index() const noexcept
		{
			auto const index = SendMessageW( ctrl_, LVM_GETNEXTITEM, static_cast< WPARAM >( -1 ), LVNI_ALL | LVNI_SELECTED );
			if( index == -1 ) {
				return{};
			}

			return static_cast< int >( index );
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

		void insert_column( int index, boost::string_ref str, int cx )
		{
			std::wstring wstr( winapi::multibyte_to_widechar( str, CP_UTF8 ) );

			LVCOLUMNW col;
			col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
			col.fmt = LVCFMT_LEFT | LVCFMT_FIXED_WIDTH;
			col.pszText = &wstr[0];
			col.cchTextMax = static_cast< int >( wstr.size() );
			col.cx = cx;

			SendMessageW( ctrl_, LVM_INSERTCOLUMNW, index, reinterpret_cast<LPARAM>( &col ) );
		}

		void insert( int index, int sub_index, boost::string_ref str )
		{
			std::wstring wstr( winapi::multibyte_to_widechar( str, CP_UTF8 ) );

			LVITEMW item;
			item.iItem = index;
			item.iSubItem = sub_index;
			item.mask = LVIF_TEXT;
			item.pszText = &wstr[0];
			item.cchTextMax = static_cast< int >( wstr.size() );

			SendMessageW( ctrl_, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>( &item ) );
		}

		void set_item_text(int index, int sub_index, boost::string_ref str)
		{
			std::wstring wstr( winapi::multibyte_to_widechar( str, CP_UTF8 ) );

			LVITEMW item;
			item.iSubItem = sub_index;
			item.mask = LVIF_TEXT;
			item.pszText = &wstr[0];
			item.cchTextMax = static_cast< int >( wstr.size() );

			SendMessageW( ctrl_, LVM_SETITEMTEXTW, index, reinterpret_cast<LPARAM>( &item ) );
		}

		HWND handle() const noexcept
		{
			return ctrl_;
		}
	};

} // namespace mmaccel