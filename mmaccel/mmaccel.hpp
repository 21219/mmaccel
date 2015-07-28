#pragma once

extern "C"
{
	__declspec( dllimport ) void mmaccel_register_hooks();
	__declspec( dllimport ) void mmaccel_start();

} // extern "C"