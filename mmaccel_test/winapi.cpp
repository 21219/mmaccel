#include "stdafx.h"
#include "CppUnitTest.h"
#include <mmaccel/winapi/point.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mmaccel_test
{		
	TEST_CLASS(WinAPI)
	{
	public:
		TEST_METHOD(Point_Client_To_Screen)
		{
			HWND hwnd = CreateWindowW( L"BUTTON", L"Test", WS_POPUP, 10, 15, 50, 30, nullptr, nullptr, nullptr, nullptr );
			if( !hwnd ) {
				Assert::Fail( L"cannot create window" );
				return;
			}

			winapi::point_t< winapi::origin_type::client > client( hwnd, 0, 0 );
			winapi::point_t< winapi::origin_type::screen > screen( client );

			Assert::IsTrue( screen.x() == 10 );
			Assert::IsTrue( screen.y() == 15 );
		}

		TEST_METHOD( Point_Screen_To_Client )
		{
			HWND hwnd = CreateWindowW( L"BUTTON", L"Test", WS_POPUP, 10, 15, 50, 30, nullptr, nullptr, nullptr, nullptr );
			if( !hwnd ) {
				Assert::Fail( L"cannot create window" );
				return;
			}

			winapi::point_t< winapi::origin_type::screen > screen( 10, 15 );
			winapi::point_t< winapi::origin_type::client > client( hwnd, screen );

			Assert::IsTrue( client.x() == 0 );
			Assert::IsTrue( client.y() == 0 );
		}
	};
}