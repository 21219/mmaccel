#pragma once

#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/window.hpp>
#include <mmaccel/gui/dropdown_box.hpp>
#include <mmaccel/gui/list_view.hpp>
#include <mmaccel/gui/tab.hpp>
#include <mmaccel/mmd_map.hpp>
#include <mmaccel/key_map.hpp>
#include "subitem_edit.hpp"
#include "resource.h"
#include <boost/container/flat_map.hpp>

namespace mmaccel { namespace key_config
{
	class window_impl
	{
		HWND wnd_;
		dropdown_box< IDC_SELECTOR > selector_;
		tabs< IDC_TAB > tabs_;
		list_view< IDC_LIST_VIEW > list_view_;
		subitem_edit< IDC_SUBITEM_EDIT > subitem_;

		boost::container::flat_map< std::string, keys_combination > keys_;
		json::data_type map_;

		window_impl():
			wnd_( winapi::modeless_dialog_box( IDD_KEY_CONFIG, nullptr, &proc ) ),
			selector_( wnd_ ),
			tabs_( wnd_ ),
			list_view_( wnd_ ),
			subitem_( wnd_, list_view_.handle() ),
			keys_( load_key_map( u8"key_map.txt" ) )
		{ 
			load_mmd_map();
			init_controls();
		}

	public:
		void show() const noexcept
		{
			ShowWindow( wnd_, SW_SHOW );
		}

		static window_impl& instance()
		{
			static std::unique_ptr< window_impl > obj( new window_impl );
			return *obj;
		}

	private:
		void init_controls()
		{
			RECT const rc = winapi::get_client_rect( list_view_.handle() );
			auto const cx = ( rc.right - rc.left ) / 2;

			list_view_.insert_column( 0, u8"“®ì", cx - GetSystemMetrics( SM_CXVSCROLL ) );
			list_view_.insert_column( 1, u8"ƒL[", cx );

			selector_.set_current_index( 0 );
		}

		void load_mmd_map()
		{
			map_ = mmd_map::load( u8"mmd_map.json" );

			auto const& root = mmd_map::get_category( map_ );
			for( auto const& i : root ) {
				selector_.push_back( i.first );
			}
		}

		void update_tabs()
		{
			tabs_.clear();

			auto const& data = mmd_map::get_sub_category( map_, *selector_.current_index() );
			for( auto const& i : data ) {
				tabs_.push_back( i.first );
			}

			tabs_.set_current_index( 0 );
		}

		void update_list_view()
		{
			list_view_.clear();

			auto const& data = mmd_map::get_elements( map_, *selector_.current_index(), *tabs_.current_index() );

			int cnt = 0;
			for( auto const i : data ) {
				list_view_.insert( cnt, 0, mmd_map::get_friendly_name( i ) );
				list_view_.set_item_text( cnt, 1, keys_to_string( keys_[i.first] ) );
				++cnt;
			}
		}
		
		void apply_subitem()
		{
			if( !subitem_.is_shown() ) {
				return;
			}

			auto const k = subitem_.get_text();
			if( k.empty() ) {
				subitem_.hide();
				return;
			}

			auto const sel_index = selector_.current_index();
			auto const tab_index = tabs_.current_index();
			auto const index = list_view_.current_index();

			if( !sel_index || !tab_index || !index ) {
				subitem_.hide();
				return;
			}

			auto const name = mmd_map::get_elements( map_, *sel_index, *tab_index )[*index].first;
			auto const itr = keys_.find( name );
			if( itr == keys_.end() ) {
				keys_.emplace( name, string_to_keys( k ) );
			}
			else {
				itr->second = string_to_keys( k );
			}

			list_view_.set_item_text( *index, 1, k );

			subitem_.hide();
		}

		void on_sizing(int edge, RECT& rc)
		{
		}

		void on_command(int id, int code, HWND hwnd)
		{
			if( id == IDC_SELECTOR ) {
				if( code == CBN_DROPDOWN ) {
					apply_subitem();
				}
				else if( code == CBN_SELCHANGE ) {
					update_tabs();
					update_list_view();
				}
			}
			else if( hwnd == subitem_.handle() && code == EN_KILLFOCUS ) {
				apply_subitem();
			}
			else if( id == IDOK ) {
				apply_subitem();
				save_key_map( u8"key_map.txt", keys_, map_ );
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
			else if( id == IDCANCEL ) {
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
			else if( id == IDAPPLY ) {
				apply_subitem();
			}
		}

		void on_notify( DWORD id, LPARAM lparam )
		{
			NMHDR const& nmhdr = *reinterpret_cast<NMHDR const*>( lparam );
			if( id == IDC_TAB ) {
				if( nmhdr.code == TCN_SELCHANGE ) {
					update_list_view();
				}
				else if( nmhdr.code == TCN_SELCHANGING ) {
					apply_subitem();
				}
			}
			else if( id == IDC_LIST_VIEW ) {
				if( nmhdr.code == NM_DBLCLK ) {
					NMITEMACTIVATE const& nia = *reinterpret_cast<NMITEMACTIVATE const*>( lparam );
					if( nia.iItem >= 0 ) {
						auto const rc = list_view_.get_subitem_rect( nia.iItem, 1 );
						if( rc ) {
							subitem_.show( *rc );
						}
					}
					else if( nmhdr.code == LVN_BEGINSCROLL ) {
						apply_subitem();
					}
				}
				else if( nmhdr.code == LVN_ITEMCHANGING ) {
					NMLISTVIEW const& nlv = *reinterpret_cast<NMLISTVIEW const*>( lparam );
					if( ( nlv.uChanged & LVIF_STATE ) && ( nlv.uOldState & LVIS_SELECTED ) && !( nlv.uNewState & LVIS_SELECTED ) ) {
						apply_subitem();
					}
				}
				else if( nmhdr.code == LVN_BEGINSCROLL ) {
					apply_subitem();
				}
			}
		}

		void on_active()
		{
			update_tabs();
			update_list_view();
		}

		static INT_PTR CALLBACK proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
		{
			if( msg == WM_SIZING ) {
				instance().on_sizing( wparam, *reinterpret_cast<RECT *>( lparam ) );
			}
			else if( msg == WM_NOTIFY ) {
				instance().on_notify( static_cast< DWORD >( wparam ), lparam );
			}
			else if( msg == WM_COMMAND ) {
				instance().on_command( LOWORD( wparam ), HIWORD( wparam ), reinterpret_cast<HWND>( lparam ) );
			}
			else if( msg == WM_ACTIVATE ) {
				if( LOWORD( wparam ) != WA_INACTIVE ) {
					instance().on_active();
				}
			}
			else if( msg == WM_CLOSE ) {
				DestroyWindow( hwnd );
				return TRUE;
			}
			else if( msg == WM_DESTROY ) {
				PostQuitMessage( 0 );
				return TRUE;
			}

			return FALSE;
		}
	};
const 
	inline window_impl& window()
	{
		return window_impl::instance();
	}

} } // namespace mmaccel::key_config