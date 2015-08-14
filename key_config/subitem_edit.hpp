#pragma once

#include <mmaccel/platform.hpp>
#include <mmaccel/keyboard.hpp>
#include <mmaccel/winapi/string.hpp>
#include <string>
#include <sstream>

namespace mmaccel { namespace key_config
{
	template <DWORD ID>
	class subitem_edit
	{
		HWND ctrl_;

	public:
		subitem_edit() = default;

		subitem_edit( HWND dialog, HWND parent ):
			ctrl_( GetDlgItem( dialog, ID ) )
		{ 
			SetWindowSubclass( ctrl_, &proc, 0, 0 );
			SetParent( ctrl_, parent );
		}

		bool is_shown() const noexcept
		{
			return IsWindow( ctrl_ ) && IsWindowVisible( ctrl_ );
		}

		void show( RECT const& rc )
		{
			if( is_shown() ) {
				return;
			}

			SetWindowTextW( ctrl_, L"" );
			SetWindowPos( ctrl_, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0 );
			ShowWindow( ctrl_, SW_SHOW );
			SetFocus( ctrl_ );
		}

		void hide()
		{
			if( is_shown() ) {
				ShowWindow( ctrl_, SW_HIDE );
			}
		}

		std::string get_text() const
		{
			if( !IsWindow( ctrl_ ) ) {
				return{};
			}

			auto const sz = SendMessageW( ctrl_, WM_GETTEXTLENGTH, 0, 0 );
			if( sz == 0 ) {
				return{};
			}

			std::wstring buf;
			buf.resize( sz + 1 );
			SendMessageW( ctrl_, WM_GETTEXT, buf.size(), reinterpret_cast<LPARAM>( &buf[0] ) );

			return winapi::widechar_to_multibyte( buf, CP_UTF8 );
		}

		HWND handle() noexcept
		{
			return ctrl_;
		}

		HWND handle() const noexcept
		{
			return ctrl_;
		}

	private:
		static LRESULT CALLBACK proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR, DWORD_PTR )
		{
			if( msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN ) {
				auto const keys = get_keyboard_state();
				auto const str = keys_to_string( keys );
				SetWindowTextW( hwnd, winapi::multibyte_to_widechar( str, CP_UTF8 ).c_str() );
				return TRUE;
			}
			else if( msg == WM_CHAR ) {
				return TRUE;
			}

			return DefSubclassProc( hwnd, msg, wparam, lparam );
		}
	};

} } // namespace mmaccel::key_config