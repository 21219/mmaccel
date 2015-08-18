#pragma once

#include <cstddef>
#include "../platform.hpp"
#include <sstream>
#include <utility>
#include <vector>
#include <boost/variant.hpp>
#include <boost/spirit/include/qi.hpp>

namespace mmaccel { namespace json
{
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

	bool parse(boost::spirit::istream_iterator first, boost::spirit::istream_iterator end, json::data_type& dst);

} } // namespace mmaccel::json