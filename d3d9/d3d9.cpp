#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/module.hpp>
#include <mmaccel/winapi/directory.hpp>
#include <mmaccel/winapi/message_box.hpp>
#include <mmaccel/mmaccel.hpp>
#include <cassert>
#include <mmaccel/path.hpp>

HMODULE mmaccel_mod_;

IDirect3D9* create_d3d( UINT version ) noexcept
{
	HMODULE mod = winapi::load_library( winapi::get_system_directory() + "\\d3d9.dll" );
	if( !mod ) {
		return nullptr;
	}

	auto const f = winapi::get_proc_address< decltype( &Direct3DCreate9 ) >( mod, "Direct3DCreate9" );
	if( !f ) {
		return nullptr;
	}

	return f( version );
}

HRESULT create_d3d_ex( UINT version, IDirect3D9Ex** obj )
{
	HMODULE mod = winapi::load_library( winapi::get_system_directory() + "\\d3d9.dll" );
	if( !mod ) {
		return E_FAIL;
	}

	auto const f = winapi::get_proc_address< decltype( &Direct3DCreate9Ex ) >( mod, "Direct3DCreate9Ex" );
	if( !f ) {
		return E_FAIL;
	}

	return f( version, obj );
}

void load_mme() 
{
	winapi::load_library( winapi::get_module_path() + "\\MMHack.dll" );
}

void load_mmaccel_hook()
{
	if( !mmaccel_mod_ ) {
		return;
	}

	auto const hooks = winapi::get_proc_address< decltype( &mmaccel_register_hooks ) >( mmaccel_mod_, "mmaccel_register_hooks" );
	if( !hooks ) {
		return;
	}

	hooks( ( winapi::get_module_path() + "\\mmaccel" ).c_str() );
}

void load_mmaccel()
{
	if( !mmaccel_mod_ ) {
		winapi::message_box( u8"MMAccel", u8"mmaccel.dllをロードできませんでした", MB_OK | MB_ICONERROR );
		return;
	}

	auto const start = winapi::get_proc_address< decltype( &mmaccel_start) >( mmaccel_mod_, "mmaccel_start" );
	if( !start ) {
		winapi::last_error_message_box( u8"MMAccel", u8"MMAccelを開始できませんでした" );
		return;
	}

	start( ( winapi::get_module_path() + "\\mmaccel" ).c_str() );
}

extern "C"
{
	IDirect3D9* WINAPI proxy_Direct3DCreate9( UINT version )
	{
		load_mmaccel();
		return create_d3d( version );
	}

	HRESULT WINAPI proxy_Direct3DCreate9Ex( UINT version, IDirect3D9Ex** obj )
	{
		return create_d3d_ex( version, obj );
	}

	BOOL APIENTRY DllMain( HINSTANCE, DWORD reason, LPVOID )
	{
		if( reason == DLL_PROCESS_ATTACH ) {
			mmaccel_mod_ = winapi::load_library( mmaccel::dll_path() );
			load_mmaccel_hook();
			load_mme();
		}

		return TRUE;
	}
}