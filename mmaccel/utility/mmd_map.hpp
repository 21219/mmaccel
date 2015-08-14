#pragma once

#include "json.hpp"
#include <boost/utility/string_ref.hpp>
#include <fstream>
#include <stdexcept>

namespace mmaccel { namespace mmd_map
{
	inline json::data_type load(boost::string_ref path)
	{
		std::ifstream ifs( path.data() );
		if( !ifs ) {
			throw std::runtime_error( "mmd_map.jsonを読み込めませんでした" );
		}

		json::grammar< boost::spirit::istream_iterator > g;
		boost::spirit::istream_iterator first( ifs ), last;
		json::data_type dst;

		if( !boost::spirit::qi::phrase_parse( first, last, g, boost::spirit::unicode::space, dst ) ) {
			throw std::runtime_error( "mmd_map.jsonにエラーがありました" );
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

} } // nmaespace mmaccel::mmd_map