#include "json.hpp"



namespace mmaccel { namespace json
{
	namespace qi = boost::spirit::qi;
	namespace code = boost::spirit::unicode;
	namespace phx = boost::phoenix;

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

	bool parse( boost::spirit::istream_iterator first, boost::spirit::istream_iterator last, json::data_type& dst )
	{
		json::grammar< boost::spirit::istream_iterator > g;
		return qi::phrase_parse( first, last, g, code::space, dst );
	}

} } // namespace mmaccel::json