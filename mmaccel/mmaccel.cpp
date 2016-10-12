#include "platform.hpp"
#include "winapi/window.hpp"
#include "winapi/hook.hpp"
#include "winapi/thread.hpp"
#include "winapi/message_box.hpp"
#include "winapi/string.hpp"
#include "winapi/pipe.hpp"
#include "winapi/path.hpp"
#include "path.hpp"
#include "file_monitor.hpp"
#include "resource.h"
#include "key_map.hpp"
#include "gui/menu.hpp"
#include "gui/version.hpp"
#include "v120_to_v150.hpp"
#include <atomic>
#include <sstream>
#include <memory>
#include <locale>
#include <clocale>

namespace mmaccel
{
	template <HOOKPROC Func>
	class hook_procedure
	{
	public:
		inline static HOOKPROC address() noexcept
		{
			return &impl;
		}

	private:
		static LRESULT CALLBACK impl( int code, WPARAM wparam, LPARAM lparam ) noexcept
		{
			if( code < 0 ) {
				return CallNextHookEx( nullptr, code, wparam, lparam );
			}

			try {
				return Func( code, wparam, lparam );
			}
			catch( std::exception const& e ) {
				winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
			}
			catch( ... ) {
				winapi::message_box( u8"MMaccel", u8"unknown error", MB_OK | MB_ICONERROR );
			}

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}
	};

	void output_debug_string(std::string const& str)
	{
#ifdef _DEBUG
		OutputDebugStringW( winapi::multibyte_to_widechar( str, CP_UTF8 ).c_str() );
#endif
	}

	class module_impl
	{
		HWND mmd_;
		HWND sep_;
		menu_t< IDR_MMACCEL_MENU, ID_MMACCEL_SETTING, ID_MMACCEL_VERSION > menu_;

		bool dialog_;
		std::atomic< bool > update_;

		file_monitor fm_;
		key_handler_map_t khm_;

		winapi::hook_handle hook_cwp_;
		winapi::hook_handle hook_gm_;

		std::vector< std::pair< keys_combination, keys_combination > > down_keys_;

		HWND key_config_;

		module_impl():
			dialog_( false ),
			update_( false )
		{}

	public:	
		void register_hooks()
		{
			HINSTANCE hinst = winapi::get_module_handle();
			hook_cwp_ = winapi::set_windows_hook_ex( 
				winapi::hook_type::call_wnd_proc, hook_procedure< call_wnd_proc_forward>::address(), hinst, winapi::get_current_thread_id() 
			);
			hook_gm_ = winapi::set_windows_hook_ex(
				winapi::hook_type::get_message, hook_procedure< get_msg_proc_forward >::address(), hinst, winapi::get_current_thread_id()
			);
		}

		void start()
		{
			try {
				mmaccel_txt_to_key_map_txt(
					winapi::get_module_path() + u8"\\mmaccel.txt",
					winapi::get_module_path() + u8"\\mmaccel\\key_map.txt",
					mmd_map::load( winapi::get_module_path() + u8"\\mmaccel\\mmd_map.json" )
				);

				auto const wnds = winapi::get_window_from_process_id( winapi::get_current_process_id() );
				for( HWND w : wnds ) {
					if( winapi::get_class_name( w ) == u8"Polygon Movie Maker" ) {
						mmd_ = w;
						break;
					}
				}

				menu_ = decltype( menu_ )( mmd_, dll_path(), u8"MMAccel" );
				menu_.assign_handler( menu_command< ID_MMACCEL_SETTING >(), [this] { this->run_key_config(); } );
				menu_.assign_handler( menu_command< ID_MMACCEL_VERSION >(), [this] { version_dialog::show( this->mmd_ ); } );

				fm_.start( winapi::get_module_path(), [this]( boost::string_ref name ) {
					if( name == u8"mmaccel\\key_map.txt" ) {
						update_ = false;
						PostMessageW( mmd_, WM_NULL, 0, 0 );

						return true;
					}

					return false;
				} );
				
				PostMessageW( mmd_, WM_NULL, 0, 0 );
			}
			catch( std::exception const& e ) {
				winapi::message_box( u8"MMAccel", e.what(), MB_OK | MB_ICONERROR );
			}
		}

		static module_impl& instance()
		{
			static std::unique_ptr< module_impl > obj( new module_impl );
			return *obj;
		}

	private:
		void call_window_proc( CWPSTRUCT const& cwp )
		{
			if( cwp.message == WM_ACTIVATE && !( LOWORD( cwp.wParam ) == WA_INACTIVE ) ) {
				if( winapi::get_class_name( cwp.hwnd ) == u8"MicWindow" ) {
					sep_ = cwp.hwnd;
				}
				else if( winapi::get_class_name( cwp.hwnd ) == u8"#32770" ) {
					dialog_ = true;
				}
			}
			else if( cwp.message == WM_DESTROY ) {
				if( winapi::get_class_name( cwp.hwnd ) == u8"MicWindow" ) {
					sep_ = nullptr;
				}
				else if( winapi::get_class_name( cwp.hwnd ) == u8"#32770" ) {
					dialog_ = false;
				}
				else if( cwp.hwnd == mmd_ ) {
					if( key_config_ && IsWindow( key_config_ ) ) {
						PostMessageW( key_config_, WM_CLOSE, 0, 0 );
					}
				}
			}
		}

		bool kill_focus(keyboard_state& ks, keys_combination const& kc)
		{
			std::string upper_class_name;
			boost::transform( winapi::get_class_name( GetFocus() ), std::back_inserter( upper_class_name ), []( char c ) { return std::toupper( c ); } );
			if( upper_class_name == u8"EDIT" ) {
				auto const rng = khm_.equal_range( kc );
				for( auto const& handler : boost::make_iterator_range( rng ) ) {
					if( handler.second.name == u8"kill_focus" ) {
						handler.second.func( ks );
						break;
					}
				}
				return true;
			}

			return false;
		}

		void get_message_proc( MSG& msg )
		{
			if( msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN ) {
				if( dialog_ ) {
					return;					
				}
				
				auto ks = get_keyboard_state();
				auto kc = state_to_combination( ks );
				
				if( GetFocus() != nullptr ) {
					if( kill_focus( ks, kc ) ) {
						return;
					}
				}

				output_debug_string( keys_to_string( kc ) + u8" : " );

				for( auto const& i : down_keys_ ) {
					for( auto const& c : i.second ) {
						ks[c] = false;
					}
					for( auto const& c : i.first ) {
						ks[c] = true;
					}
				}

				kc = state_to_combination( ks );

				output_debug_string( keys_to_string( kc ) + u8"\n" );

				auto const rng = khm_.equal_range( kc );
				if( rng.first == khm_.end() ) {
					return;
				}

				for( auto const i : kc ) {
					ks[i] = false;
					if( msg.wParam == i ) {
						msg.wParam = 0;
					}
				}

				for( auto const& handler : boost::make_iterator_range( rng ) ) {
					keyboard_state tmp_ks;

					handler.second.func( tmp_ks );

					auto const tmp_kc = state_to_combination( tmp_ks );

					if( boost::find_if( down_keys_, [&tmp_kc](auto const& p) { return p.second == tmp_kc; } ) == down_keys_.end() ) {
						down_keys_.push_back( std::make_pair( kc, tmp_kc ) );
						
						output_debug_string( u8"push down -> (" + keys_to_string( kc ) + ", " + keys_to_string( tmp_kc ) + u8")\n" );
					}

					for( auto const i : tmp_kc ) {
						ks[i] = true;
					}
				}

				set_keyboard_state( ks );
			}
			else if( msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP ) {
				if( dialog_ ) {
					return;
				}

				auto ks = get_keyboard_state();

				for( auto i = down_keys_.begin(); i != down_keys_.end(); ) {
					auto itr =  boost::find( i->first, msg.wParam );
					if( itr != i->first.end() ) {
						for( auto c : i->second ) {
							ks[c] = false;
						}
						for( auto c : i->first ) {
							ks[c] = true;
						}
						ks[msg.wParam] = false;

						output_debug_string( u8"still down -> " + keys_to_string( state_to_combination( ks ) ) + u8"\n" );

						i = down_keys_.erase( i );
					}
					else {
						++i;
					}
				}

				set_keyboard_state( ks );
			}
			else if( msg.message == WM_COMMAND ) {
				menu_.on_command( msg.wParam );
			}
			else if( msg.message == WM_NULL ) {
				update_key_map();
			}
		}

		static LRESULT CALLBACK call_wnd_proc_forward(int code, WPARAM wparam, LPARAM lparam)
		{
			auto& cwp = *reinterpret_cast<CWPSTRUCT*>( lparam );
			instance().call_window_proc( cwp );

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}

		static LRESULT CALLBACK get_msg_proc_forward( int code, WPARAM wparam, LPARAM lparam )
		{
			if( code == HC_NOREMOVE ) {
				return CallNextHookEx( nullptr, code, wparam, lparam );
			}

			auto& msg = *reinterpret_cast<MSG*>( lparam );
			instance().get_message_proc( msg );

			return CallNextHookEx( nullptr, code, wparam, lparam );
		}

		void update_key_map()
		{
			if( update_ ) {
				return;
			}
			
			auto const mm = mmd_map::load( winapi::get_module_path() + u8"\\mmaccel\\mmd_map.json" );
			auto const key_map_path = winapi::get_module_path() + u8"\\mmaccel\\key_map.txt";
			if( !winapi::path_file_exists( key_map_path ) ) {
				save_key_map( key_map_path, {}, mm );
			}
		
			khm_ = load_key_handler_map( winapi::get_module_path() + u8"\\mmaccel\\key_map.txt", mm, mmd_, sep_ );

			update_ = true;
		}

		HWND get_key_config_window(winapi::unique_handle const& hread)
		{
			std::uintptr_t p = 0;
			DWORD byte;

			if( !ReadFile( hread.get(), &p, sizeof( std::uintptr_t ), &byte, nullptr ) ) {
				winapi::last_error_message_box( u8"MMAccel", u8"ReadFile error" );
				return nullptr;
			}

			return reinterpret_cast< HWND >( p );
		}

		void run_key_config()
		{
			SECURITY_ATTRIBUTES sa = { sizeof( SECURITY_ATTRIBUTES ), nullptr, TRUE };
			
			auto pipe = winapi::create_pipe( sa );
			if( !pipe ) {
				winapi::last_error_message_box( u8"MMAccel", u8"run_key_config CreatePipe error" );
				return;
			}

			auto read_tmp = winapi::duplicate_handle( GetCurrentProcess(), pipe->read_handle, GetCurrentProcess(), 0, FALSE, DUPLICATE_SAME_ACCESS );
			if( !read_tmp ) {
				winapi::last_error_message_box( u8"MMAccel", u8"run_key_config DuplicateHandle error" );
				return;
			}
			std::swap( pipe->read_handle, read_tmp );
			read_tmp.release();

			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			RECT const rc = winapi::get_window_rect( mmd_ );

			ZeroMemory( &si, sizeof( si ) );
			si.cb = sizeof( STARTUPINFOW );
			si.dwFlags = STARTF_USEPOSITION | STARTF_USESTDHANDLES;
			si.dwX = rc.left;
			si.dwY = rc.top;
			si.hStdOutput = pipe->write_handle.get();
			si.hStdInput = GetStdHandle( STD_INPUT_HANDLE );
			si.hStdError = GetStdHandle( STD_ERROR_HANDLE );

			auto const target_exe = winapi::multibyte_to_widechar( winapi::get_module_path() + u8"\\mmaccel\\key_config.exe", CP_UTF8 );
			auto const current_dir = winapi::multibyte_to_widechar( winapi::get_module_path() + u8"\\mmaccel", CP_UTF8 );
			std::wstring args( L"--mmd" );
			if( !CreateProcessW( target_exe.c_str(), &args[0], nullptr, nullptr, TRUE, NORMAL_PRIORITY_CLASS, nullptr, current_dir.c_str(), &si, &pi ) ) {
				winapi::last_error_message_box( u8"MMAccel", u8"run_key_config CreateProcess error" );
				return;
			}
			key_config_ = get_key_config_window( pipe->read_handle );

			CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );
		}
	};

	inline module_impl& module()
	{
		return module_impl::instance();
	}

} // namespace mmaccel

extern "C"
{
	__declspec( dllexport ) void mmaccel_register_hooks()
	{
		mmaccel::module().register_hooks();
	}

	__declspec( dllexport ) void mmaccel_start()
	{
		mmaccel::module().start();
	}

} // extern "C"