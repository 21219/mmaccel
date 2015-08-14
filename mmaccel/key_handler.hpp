#pragma once

#include "platform.hpp"
#include <boost/utility/string_ref.hpp>
#include "keyboard.hpp"

namespace mmaccel
{
	struct key_handler
	{
		using functor_type = void( *)( keyboard_state& state );

		boost::string_ref name;
		functor_type functor;

		key_handler(boost::string_ref name, functor_type functor) :
			name( name ),
			functor( functor )
		{ }
	};

	inline bool operator==(key_handler const& lhs, key_handler const& rhs) noexcept
	{
		return lhs.name == rhs.name;
	}

	inline bool operator!=( key_handler const& lhs, key_handler const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

} // namespace mmaccel