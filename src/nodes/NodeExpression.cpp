#include "NodeExpression.h"

#include <string>


// Remove nil and Nil defines as they are incompatible with boost.
//#ifdef nil
//#define hm__nil_ nil
//#endif
//#ifdef Nil
//#define hm__Nil_ Nil
//#endif

#undef nil
#undef Nil

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
//
//// Restore nil and Nil defines
//#ifdef hm__nil_
//#define nil hm__nil_
//#undef hm__nil_
//#endif
//#ifdef hm__Nil_
//#define Nil hm__Nil_
//#undef hm__Nil_
//#endif

using namespace std;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace hm
{
	template <typename Iterator>
	struct NodeExpressionGrammar : qi::grammar<Iterator, double(), ascii::space_type>
	{
		qi::rule<Iterator, Data(), ascii::space_type> factor;
		qi::rule<Iterator, Data(), ascii::space_type> term;
		qi::rule<Iterator, Data(), ascii::space_type> expression;
		//	qi::rule<Iterator, double(), ascii::space_type> paren;
		qi::symbols<char, Data> vars;
		
		NodeExpressionGrammar()
		: NodeExpressionGrammar::base_type(expression)
		{
			using qi::lit;
			using qi::_val;
			using qi::_1;
			using qi::_2;
			using qi::double_;
			using boost::phoenix::construct;
			
			factor = double_[_val = construct<Data>(construct<Value>(_1), 0.)] | vars | '(' >> expression >> ')';
			
			term = factor[_val = _1]
			>> *(('*' >> factor)[_val = _val * _1]
				 | ('/' >> factor)[_val = _val / _1]);
			
			expression = term[_val = _1]
			>> *(('+' >> term)[_val = _val + _1]
				 | ('-' >> term)[_val = _val - _1]);
		}
	};



	NodeExpression::NodeExpression(Node::Params params, string className)
	: NodeUnthreaded(params, className)
	{
		int numInlets = 3;
		int numOutlets = 2;
		for (int i=0; i<numInlets; i++)
		{
			createInlet(VALUE | VECTOR3D_TYPES, "Input i"+std::to_string(i+1), "Input here may be used as \"i"+to_string(i+1)+"\" in your expression.");
		}
		for (int i=0; i<numOutlets; i++)
		{
			createOutlet(VALUE | VECTOR3D_TYPES, "Output i"+to_string(i+1), "Output here may be written to as \"o"+to_string(i+1)+"\" in your expression.");
		}
		mGrammar = unique_ptr<NodeExpressionGrammar<string::iterator>>(new NodeExpressionGrammar<string::iterator>);
	}
	
	NodePtr NodeExpression::create(Node::Params params) const
	{
		return NodePtr(new NodeExpression(params));
	}

}




