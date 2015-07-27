#include "stdafx.h"
#include "CppUnitTest.h"

#include <mmaccel/gui/message_handler.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mmaccel_test
{
	struct test
	{
		using function_type = int( int );
	};

	struct test_void
	{
		using function_type = void( int );
	};

	TEST_CLASS( MessageHandlerTest )
	{
	public:
		TEST_METHOD( Empty )
		{
			mmaccel::message_handler< test > mh;

			Assert::IsTrue( mh.empty( test() ) );
		}

		TEST_METHOD( Invoke )
		{
			mmaccel::message_handler< test, test_void > mh;

			mh.assign( test(), []( int i ) { return i + 1; } );

			Assert::IsTrue( mh.invoke( test(), 2 ) == 3 );

			int n = 2;
			mh.assign( test_void(), [&n]( int i ) { n += i; } );
			mh.invoke( test_void(), 3 );

			Assert::IsTrue( n == 5 );
		}
	};
}