#pragma once

#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/window.hpp>
#include <mmaccel/winapi/menu.hpp>
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
	class control_mover
	{
		HWND ctrl_;
		POINT offset_;

	public:
		control_mover( HWND ctrl ) :
			ctrl_( ctrl )
		{
			RECT const parent_rc = winapi::get_window_rect( GetParent( ctrl ) );
			RECT const ctrl_rc = winapi::get_window_rect( ctrl );

			offset_.x = parent_rc.right - ctrl_rc.left + GetSystemMetrics( SM_CXFRAME ) * 2;
			offset_.y = parent_rc.bottom - ctrl_rc.top + GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYFRAME ) * 2;
		}

		void on_sizing( int, RECT const& rc )
		{
			RECT const ctrl_rc = winapi::get_window_rect( ctrl_ );

			SetWindowPos(
				ctrl_, nullptr,
				( rc.right - rc.left ) - offset_.x,
				( rc.bottom - rc.top ) - offset_.y,
				0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW
			);
		}
	};

	class control_resizer
	{
		HWND ctrl_;
		SIZE offset_;

	public:
		control_resizer( HWND ctrl ) :
			ctrl_( ctrl )
		{
			RECT const parent_rc = winapi::get_window_rect( GetParent( ctrl ) );
			RECT const ctrl_rc = winapi::get_window_rect( ctrl );

			offset_.cx = parent_rc.right - ctrl_rc.right;
			offset_.cy = parent_rc.bottom - ctrl_rc.bottom;
		}

		void on_sizing( int, RECT const& rc ) 
		{
			RECT const ctrl_rc = winapi::get_window_rect( ctrl_ );

			SetWindowPos( 
				ctrl_, nullptr, 0, 0, 
				( rc.right - rc.left ) - ( ctrl_rc.left - rc.left ) - offset_.cx,
				( rc.bottom - rc.top ) - ( ctrl_rc.top - rc.top ) - offset_.cy,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW
			);
		}
	};

	class window_impl
	{
		HWND wnd_;
		dropdown_box< IDC_SELECTOR > selector_;
		tabs< IDC_TAB > tabs_;
		list_view< IDC_LIST_VIEW > list_view_;
		subitem_edit< IDC_SUBITEM_EDIT > subitem_;
		winapi::loaded_menu_handle popup_root_;
		winapi::got_menu_handle popup_;

		boost::container::flat_map< std::string, keys_combination > keys_;
		json::data_type map_;

		std::array< control_mover, 3 > movers;
		std::array< control_resizer, 2 > resizers;

		window_impl() :
			wnd_( winapi::modeless_dialog_box( IDD_KEY_CONFIG, nullptr, &proc ) ),
			selector_( wnd_ ),
			tabs_( wnd_ ),
			list_view_( wnd_ ),
			subitem_( wnd_, list_view_.handle() ),
			popup_root_( winapi::load_menu( IDR_MENU_CONTEXT ) ),
			popup_( winapi::get_sub_menu( popup_root_, 0 ) ),
			keys_( load_key_map( u8"key_map.txt" ) ),
			movers{ { GetDlgItem( wnd_, IDOK ), GetDlgItem( wnd_, IDCANCEL ), GetDlgItem( wnd_, IDAPPLY ) } },
			resizers{ { tabs_.handle(), list_view_.handle() } }
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

		void clear_key()
		{
			auto const sel_index = selector_.current_index();
			auto const tab_index = tabs_.current_index();
			auto const index = list_view_.current_index();

			auto const name = mmd_map::get_elements( map_, *sel_index, *tab_index )[*index].first;
			auto const itr = keys_.find( name );
			if( itr == keys_.end() ) {
				return;
			}

			keys_.erase( itr );

			list_view_.set_item_text( *index, 1, u8"" );
		}

		void on_sizing(HWND hwnd, int frame, RECT& rc)
		{
			if( hwnd != wnd_ ) {
				return;
			}

			for( auto& i : movers ) {
				i.on_sizing( frame, rc );
			}
			for( auto& i : resizers ) {
				i.on_sizing( frame, rc );
			}

			InvalidateRect( wnd_, nullptr, TRUE );
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
			else if( id == ID_MENU_CLEAR ) {
				clear_key();
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
				else if( nmhdr.code == NM_RCLICK ) {
					NMITEMACTIVATE const& nia = *reinterpret_cast<NMITEMACTIVATE const*>( lparam );
					if( nia.iItem >= 0 ) {
						POINT pt;
						GetCursorPos( &pt );
						TrackPopupMenu( popup_.get(), TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, wnd_, nullptr );
					}
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
				instance().on_sizing( hwnd, wparam, *reinterpret_cast<RECT *>( lparam ) );
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

	inline window_impl& window()
	{
		return window_impl::instance();
	}

} } // namespace mmaccel::key_config