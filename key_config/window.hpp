#pragma once

#include <mmaccel/winapi/dialog.hpp>
#include <mmaccel/winapi/window.hpp>
#include <mmaccel/gui/dropdown_box.hpp>
#include <mmaccel/gui/list_view.hpp>
#include <mmaccel/gui/tab.hpp>
#include <fstream>
#include <stdexcept>
#include <mmaccel/utility/json.hpp>
#include "resource.h"

namespace mmaccel { namespace key_config
{
	class window_impl
	{
		HWND wnd_;
		dropdown_box< IDC_SELECTOR > selector_;
		tabs< IDC_TAB > tabs_;
		list_view< IDC_LIST_VIEW > list_view_;

		json::data_type map_;

		window_impl():
			wnd_( winapi::modeless_dialog_box( IDD_KEY_CONFIG, nullptr, &proc ) ),
			selector_( wnd_ ),
			tabs_( wnd_ ),
			list_view_( wnd_ )
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

			list_view_.insert_column( 0, L"動作",  cx - GetSystemMetrics( SM_CXVSCROLL ) );
			list_view_.insert_column( 1, L"キー", cx );

			selector_.set_current_index( 0 );
		}

		void load_mmd_map()
		{
			std::ifstream ifs( "mmd_map.json" );
			if( !ifs ) {
				return;
			}

			json::grammar< boost::spirit::istream_iterator > g;

			boost::spirit::istream_iterator first( ifs );
			boost::spirit::istream_iterator last;
			
			if( !boost::spirit::qi::phrase_parse( first, last, g, boost::spirit::unicode::space, map_ ) ) {
				DestroyWindow( wnd_ );
				throw std::runtime_error( "mmd_map.jsonでエラーがありました" );
			}

			if( map_.type() == typeid( json::object_type ) ) {
				auto const root = boost::get< json::object_type >( map_ );
				for( auto const& i : root ) {
					selector_.push_back( winapi::multibyte_to_widechar( i.first, CP_UTF8 ) );
				}
			}
		}

		void update_tabs()
		{
			tabs_.clear();

			auto const l = boost::get< json::object_type >( map_ )[*selector_.current_index()].second;
			for( auto const& i : boost::get< json::object_type >( l ) ) {
				tabs_.push_back( winapi::multibyte_to_widechar( i.first, CP_UTF8 ) );
			}

			tabs_.set_current_index( 0 );
		}

		void update_list_view()
		{
			list_view_.clear();

			auto const l = boost::get< json::object_type >( boost::get< json::object_type >( map_ )[*selector_.current_index()].second )[*tabs_.current_index()].second;

			int cnt = 0;
			for( auto const i : boost::get< json::object_type >( l ) ) {
				list_view_.insert( cnt, 0, winapi::multibyte_to_widechar( boost::get< std::string >( boost::get< json::array_type >( i.second )[0] ), CP_UTF8 ) );
				++cnt;
			}
		}

		void on_command(int id, int code, HWND)
		{
			if( id == IDC_SELECTOR ) {
				if( code == CBN_DROPDOWN ) {

				}
				else if( code == CBN_SELCHANGE ) {
					update_tabs();
					update_list_view();
				}
			}
			else if( id == IDOK ) {
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
			else if( id == IDCANCEL ) {
				PostMessageW( wnd_, WM_CLOSE, 0, 0 );
			}
		}

		void on_notify( DWORD id, NMHDR const& nmhdr )
		{
			if( id == IDC_TAB ) {
				if( nmhdr.code == TCN_SELCHANGE ) {
					update_list_view();
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
			if( msg == WM_NOTIFY ) {
				instance().on_notify( wparam, *reinterpret_cast<NMHDR const*>( lparam ) );
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