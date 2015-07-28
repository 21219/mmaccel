#include <mmaccel/platform.hpp>
#include <mmaccel/winapi/module.hpp>
#include <mmaccel/winapi/directory.hpp>
#include <mmaccel/mmaccel.hpp>

IDirect3D9* create_d3d( UINT version ) noexcept
{
	HMODULE mod = winapi::load_library( winapi::get_system_directory() + L"\\d3d9.dll" );
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
	HMODULE mod = winapi::load_library( winapi::get_system_directory() + L"\\d3d9.dll" );
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
	winapi::load_library( winapi::get_module_path() + L"\\MMHack.dll" );
}

void load_mmaccel()
{
	HMODULE mod = winapi::load_library( winapi::get_module_path() + L"\\mmaccel\\mmaccel.dll" );
	if( !mod ) {
		return;
	}

	auto const hooks = winapi::get_proc_address< decltype( &mmaccel_register_hooks ) >( mod, "mmaccel_register_hooks" );
	if( !hooks ) {
		return;
	}

	auto const start = winapi::get_proc_address< decltype( &mmaccel_start) >( mod, "mmaccel_start" );
	if( !start ) {
		return;
	}

	hooks();
	start();
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
			load_mme();
		}

		return TRUE;
	}
}