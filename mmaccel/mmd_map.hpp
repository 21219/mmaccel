#pragma once

#include "utility/json.hpp"
#include <boost/utility/string_ref.hpp>
#include <boost/container/flat_map.hpp>
#include <fstream>
#include <stdexcept>

namespace mmaccel { namespace mmd_map
{
	inline json::data_type load(boost::string_ref path)
	{
		std::ifstream ifs( winapi::convert_string( path, CP_UTF8, CP_OEMCP ).c_str() );
		if( !ifs ) {
			throw std::runtime_error( u8"mmd_map.jsonを読み込めませんでした" );
		}

		boost::spirit::istream_iterator first( ifs ), last;
		json::data_type dst;

		if( !json::parse( first, last, dst ) ) {
			throw std::runtime_error( u8"mmd_map.jsonにエラーがありました" );
		}

		return dst;
	}

	inline json::object_type const& get_category( json::data_type const& mm )
	{
		return boost::get< json::object_type >( mm );
	}

	inline json::object_type const& get_sub_category(json::data_type const& mm, int c_index)
	{
		return boost::get< json::object_type >( get_category( mm )[c_index].second );
	}

	inline json::object_type const& get_elements( json::data_type const& mm, int c_index, int sc_index)
	{
		return boost::get< json::object_type >( get_sub_category( mm, c_index )[sc_index].second );
	}

	inline std::string const& get_friendly_name(json::pair_type const& elem)
	{
		return boost::get< std::string >( boost::get< json::array_type >( elem.second )[0] );
	}

	inline boost::container::flat_map< std::string, json::array_type > get_elements_map(json::data_type const& mm)
	{
		boost::container::flat_map< std::string, json::array_type > result;

		for( auto const& i : boost::get< json::object_type >( mm ) ) {
			for( auto const& j : boost::get< json::object_type >( i.second ) ) {
				for( auto const& k : boost::get< json::object_type >( j.second ) ) {
					result.emplace( k.first, boost::get< json::array_type >( k.second ) );
				}
			}
		}

		return result;
	}

} } // nmaespace mmaccel::mmd_map