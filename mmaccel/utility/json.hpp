#pragma once

#include <cstddef>
#include "../platform.hpp"
#include <sstream>
#include <boost/any.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace mmaccel { namespace json
{
	namespace qi = boost::spirit::qi;
	namespace code = boost::spirit::unicode;
	namespace phx = boost::phoenix;

	struct array_type;
	struct object_type;

	using data_type = boost::variant< int, double, std::string, boost::recursive_wrapper< array_type >, boost::recursive_wrapper< object_type > >;
	using pair_type = std::pair< std::string, data_type >;
	
	struct array_type
	{
		std::vector< data_type > data;

		using container_type = decltype( data );

		using size_type = container_type::size_type;
		using iterator = container_type::iterator;
		using const_iterator = container_type::const_iterator;

		array_type() = default;

		array_type( decltype( data ) const& d ) :
			data( d )
		{ }

		size_type size() const noexcept
		{
			return data.size();
		}

		iterator begin() noexcept
		{
			return data.begin();
		}

		iterator end() noexcept
		{
			return data.end();
		}

		const_iterator begin() const noexcept
		{
			return data.begin();
		}

		const_iterator end() const noexcept
		{
			return data.end();
		}

		data_type& operator[]( int i )
		{
			return data[i];
		}

		data_type const& operator[]( int i ) const
		{
			return data[i];
		}
	};

	struct object_type
	{
		std::vector< pair_type > data;

		using container_type = decltype( data );

		using size_type = container_type::size_type;
		using iterator = container_type::iterator;
		using const_iterator = container_type::const_iterator;
		
		object_type() = default;

		object_type( decltype( data ) const& d ) :
			data( d )
		{ }

		size_type size() const noexcept
		{
			return data.size();
		}

		iterator begin() noexcept
		{
			return data.begin();
		}

		iterator end() noexcept
		{
			return data.end();
		}

		const_iterator begin() const noexcept
		{
			return data.begin();
		}

		const_iterator end() const noexcept
		{
			return data.end();
		}

		pair_type& operator[]( int i )
		{
			return data[i];
		}

		pair_type const& operator[]( int i ) const
		{
			return data[i];
		}
	};

	template <typename Iterator>
	struct grammar :
		qi::grammar< Iterator, data_type(), code::space_type >
	{
		qi::rule< Iterator, data_type(), code::space_type > root;
		qi::rule< Iterator, data_type(), code::space_type > data_rule;
		qi::rule< Iterator, std::string(), code::space_type > string_rule;
		qi::rule< Iterator, pair_type(), code::space_type > pair_rule;
		qi::rule< Iterator, array_type(), code::space_type > array_rule;
		qi::rule< Iterator, object_type(), code::space_type > object_rule;

		grammar() :
			grammar::base_type( root )
		{
			root %= data_rule.alias();
			data_rule %= object_rule | array_rule | string_rule | qi::int_;
			string_rule %= qi::lexeme['\"' >> *( qi::char_ - '\"' ) >> '\"'];
			pair_rule %= string_rule >> ':' >> data_rule;
			array_rule %= '[' >> ( data_rule % ',' )[qi::_val = phx::construct< array_type >( qi::_1 )] >> ']';
			object_rule %= '{' >> ( pair_rule % ',' )[qi::_val = phx::construct< object_type >( qi::_1 )] >> '}';
		}
	};

} } // namespace mmaccel::json