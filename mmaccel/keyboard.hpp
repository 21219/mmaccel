#pragma once

#include "platform.hpp"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <memory>
#include <array>
#include <unordered_map>
#include <boost/preprocessor.hpp>
#include <boost/optional.hpp>
#include <boost/utility/string_ref.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/functional/hash.hpp>
#include "utility/stringize.hpp"

#define MMACCEL_LOWER_ALPHABETS \
	a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
#define MMACCEL_ALPHABET_CHARS \
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'

#define MMACCEL_ALPHABET_TABLE_FUNC(z, n, txt) \
	{ MMACCEL_U8_STRINGIZE( BOOST_PP_VARIADIC_ELEM( n, MMACCEL_LOWER_ALPHABETS ) ), BOOST_PP_VARIADIC_ELEM( n, MMACCEL_ALPHABET_CHARS ) },

#define MMACCEL_KEY_N(z, n, txt) { u8#n, '0' + n },
#define MMACCEL_NUMPAD_N(z, n, txt) { MMACCEL_U8_STRINGIZE( num##n ), VK_NUMPAD##n },
#define MMACCEL_FKEY(z, n, txt) { MMACCEL_U8_STRINGIZE( f##n ), VK_F##n },

namespace mmaccel
{
	class keys_combination
	{
		std::vector< std::uint16_t > keys_;

	public:
		using value_type = decltype( keys_ )::value_type;
		using size_type = decltype( keys_ )::size_type;
		using difference_type = decltype( keys_ )::difference_type;
		using const_pointer = decltype( keys_ )::const_pointer;
		using const_reference = decltype( keys_ )::const_reference;
		using const_iterator = decltype( keys_ )::const_iterator;

		keys_combination() = default;

		keys_combination( std::initializer_list< value_type > l ) :
			keys_( l )
		{
			std::stable_sort( keys_.begin(), keys_.end() );
		}

		keys_combination( keys_combination const& ) = default;
		keys_combination( keys_combination&& ) = default;
		keys_combination& operator=( keys_combination const& ) = default;
		keys_combination& operator=( keys_combination&& ) = default;

		bool empty() const noexcept
		{
			return keys_.empty();
		}

		size_type size() const noexcept
		{
			return keys_.size();
		}

		const_iterator begin() const noexcept
		{
			return keys_.begin();
		}
	
		const_iterator end() const noexcept
		{
			return keys_.end();
		}

		value_type const& operator[]( size_type index ) const noexcept
		{
			return keys_[index];
		}

		void push_back( decltype( keys_ )::value_type v )
		{
			keys_.push_back( v );
		}

		friend bool operator==( keys_combination const&, keys_combination const& ) noexcept;
		friend bool operator<( keys_combination const&, keys_combination const& ) noexcept;
	};

	inline bool operator==( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return lhs.size() == rhs.size() && std::equal( lhs.begin(), lhs.end(), rhs.begin() );
	}

	inline bool operator!=( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return !( lhs == rhs );
	}

	inline bool operator<( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return lhs.keys_ < rhs.keys_;
	}

	inline bool operator>=( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return !( lhs < rhs );
	}

	inline bool operator>( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return rhs < lhs;
	}

	inline bool operator<=( keys_combination const& lhs, keys_combination const& rhs ) noexcept
	{
		return !( lhs > rhs );
	}

	namespace detail
	{
		class keyboard_pairs_impl
		{
			using pair_type = std::pair< std::string, keys_combination::value_type>;
			using table_type = std::vector< pair_type >;

			table_type table_;

			keyboard_pairs_impl() :
				table_{
					{ u8"backspace", VK_BACK },
					{ u8"tab", VK_TAB },
					{ u8"enter", VK_RETURN },
					{ u8"shift", VK_SHIFT },
					{ u8"ctrl", VK_CONTROL },
					{ u8"alt", VK_MENU },
					{ u8"pause", VK_PAUSE },
					{ u8"capslock", VK_CAPITAL },
					{ u8"esc", VK_ESCAPE },
					{ u8"space", VK_SPACE },
					{ u8"pageup", VK_PRIOR },
					{ u8"pagedown", VK_NEXT },
					{ u8"end", VK_END },
					{ u8"home", VK_HOME },
					{ u8"left", VK_LEFT },
					{ u8"up", VK_UP },
					{ u8"right", VK_RIGHT },
					{ u8"down", VK_DOWN },
					{ u8"printscreen", VK_SNAPSHOT },
					{ u8"insert", VK_INSERT },
					{ u8"delete", VK_DELETE },
					BOOST_PP_REPEAT( 10, MMACCEL_KEY_N, BOOST_PP_EMPTY() )
					BOOST_PP_REPEAT( BOOST_PP_VARIADIC_SIZE( MMACCEL_LOWER_ALPHABETS ), MMACCEL_ALPHABET_TABLE_FUNC, BOOST_PP_EMPTY() )
					BOOST_PP_REPEAT( 10, MMACCEL_NUMPAD_N, BOOST_PP_EMPTY() )
					{ u8"num*", VK_MULTIPLY },
					{ u8"num+", VK_ADD },
					{ u8"num-", VK_SUBTRACT },
					{ u8"num/", VK_DIVIDE },
					{ u8"num.", VK_DECIMAL },
					BOOST_PP_REPEAT_FROM_TO( 1, 25, MMACCEL_FKEY, BOOST_PP_EMPTY() )
					{ u8"-", VK_OEM_MINUS },
					{ u8";", VK_OEM_PLUS },
					{ u8",", VK_OEM_COMMA },
					{ u8".", VK_OEM_PERIOD },
					{ u8":", VK_OEM_1 },
					{ u8"/", VK_OEM_2 },
					{ u8"@", VK_OEM_3 },
					{ u8"[", VK_OEM_4 },
					{ u8"\\", VK_OEM_5 },
					{ u8"]", VK_OEM_6 },
					{ u8"^", VK_OEM_7 },
					{ u8"_", VK_OEM_102 },
				}
			{ }
			
			keyboard_pairs_impl( keyboard_pairs_impl const& ) = delete;
			keyboard_pairs_impl( keyboard_pairs_impl&& ) = delete;
			keyboard_pairs_impl& operator=( keyboard_pairs_impl const& ) = delete;
			keyboard_pairs_impl& operator=( keyboard_pairs_impl&& ) = delete;

		public:
			using value_type = keys_combination::value_type;
			using const_iterator = table_type::const_iterator;

			boost::optional< value_type > value( boost::string_ref name ) const
			{
				auto const itr = boost::find_if( table_, [&name]( pair_type const& p ) { return name == p.first; } );
				return itr != table_.end() ? itr->second : boost::optional< value_type >();
			}

			boost::optional< boost::string_ref > name( value_type v ) const
			{
				auto const itr = boost::find_if( table_, [v]( pair_type const& p ) { return v == p.second; } );
				return itr != table_.end() ? boost::string_ref( itr->first ) : boost::optional< boost::string_ref >();
			}

			static keyboard_pairs_impl& instance()
			{
				static std::unique_ptr< keyboard_pairs_impl > obj( new keyboard_pairs_impl );
				return *obj;
			}
		};

	} // namespace detail

	inline detail::keyboard_pairs_impl& keyboard_pairs()
	{
		return detail::keyboard_pairs_impl::instance();
	}

	inline std::string keys_to_string( keys_combination const& src )
	{
		if( src.empty() ) {
			return{};
		}

		std::ostringstream oss;
		auto v = keyboard_pairs().name( src[0] );
		if( !v ) {
			return{};
		}
		oss << *v;

		for( std::size_t i = 1; i < src.size(); ++i ) {
			v = keyboard_pairs().name( src[i] );
			if( !v ) {
				continue;
			}
			oss << u8" + " << *v;
		}

		return oss.str();
	}

	inline keys_combination string_to_keys( boost::string_ref str )
	{
		namespace qi = boost::spirit::qi;

		auto const rule = boost::proto::deep_copy(
			( qi::string( "num+" ) | qi::string( "num-" ) | qi::string( "-" ) | +qi::char_( "A-Za-z0-9^\\\\@[;:],./_" ) ) % '+'
		);

		std::vector< std::string > strs;
		if( !qi::phrase_parse( str.begin(), str.end(), rule, qi::unicode::space, strs ) ) {
			return{};
		}

		keys_combination keys;
		for( auto const& s : strs ) {
			auto const v = keyboard_pairs().value( s );
			if( v ) {
				keys.push_back( *v );
			}
		}

		return keys;
	}

	class keyboard_state
	{
		using table_type = std::array< BYTE, 256 >;

		table_type states_;

		keyboard_state() = default;
		keyboard_state( keyboard_state const& ) = delete;
		keyboard_state& operator=( keyboard_state const& ) = delete;

		keyboard_state(table_type&& states) :
			states_( std::move( states ) )
		{ }

	public:
		using size_type = table_type::size_type;
		using const_iterator = table_type::const_iterator;

		keyboard_state( keyboard_state&& ) = default;
		keyboard_state& operator=( keyboard_state&& ) = default;

		template <typename T>
		class key_type
		{
			friend class keyboard_state;

			T& k_;

			key_type() = delete;
			key_type( key_type const& ) = delete;
			key_type( key_type&& ) = default;
			key_type& operator=( key_type const& ) = delete;
			key_type& operator=( key_type&& ) = default;

			key_type(T& k) :
				k_( k )
			{ }

		public:
			key_type& operator=( bool state ) noexcept
			{
				k_ = state ? 0x80 : 0x00;
				return *this;
			}

			operator bool() const noexcept
			{
				return ( k_ & 0x80 ) != 0;
			}
		};

		size_type size() const noexcept
		{
			return states_.size();
		}

		void clear() noexcept
		{
			boost::fill( states_, 0x00 );
		}
		
		key_type< BYTE > operator[]( size_type key ) noexcept
		{
			return { states_[key] };
		}

		key_type< BYTE const > operator[]( size_type key ) const noexcept
		{
			return { states_[key] };
		}

		const_iterator begin() const noexcept
		{
			return states_.begin();
		}

		const_iterator end() const noexcept
		{
			return states_.end();
		}

		friend keyboard_state get_keyboard_state();
		friend void set_keyboard_state( keyboard_state const& );
	};

	inline keyboard_state get_keyboard_state()
	{
		keyboard_state::table_type states;
		GetKeyboardState( &states[0] );
		return{ std::move( states ) };
	}
	
	inline void set_keyboard_state( keyboard_state const& ks ) 
	{
		SetKeyboardState( const_cast<LPBYTE>( &ks.states_[0] ) );
	}

	inline keys_combination state_to_combination( keyboard_state const& state )
	{
		keys_combination keys;
		for( keyboard_state::size_type i = 0; i < state.size(); ++i ) {
			if( i == 0xf0 || i == 0xf3 || i == 0xf6 || i == 0xfb || i >= 0xa0 && i <= 0xa5 ) {
				continue;
			}
			if( state[i] ) {
				keys.push_back( static_cast< keys_combination::value_type >( i ) );
			}
		}

		return keys;
	}

	inline std::string keys_to_string( keyboard_state const& state )
	{
		return keys_to_string( state_to_combination( state ) );
	}

} // namespace mmaccel

namespace std
{
	template <>
	struct hash< mmaccel::keys_combination >
	{
		using argument_type = mmaccel::keys_combination;
		using result_type = std::size_t;

		result_type operator()( mmaccel::keys_combination const& kc ) const
		{
			hash< typename mmaccel::keys_combination::value_type > f;

			result_type result = 0;
			for( auto i : kc ) {
				boost::hash_combine( result, i );
			}

			return result;
		}
	};

} // namespace std