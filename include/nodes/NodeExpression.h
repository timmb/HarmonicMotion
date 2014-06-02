//
// NodeExpression.h
// HarmonicMotion
//
// Created by Tim Murray-Browne on 2014-05-27
//


#pragma once

#include "NodeUnthreaded.h"
// Remove nil and Nil defines as they are incompatible with boost.
#ifdef nil
#define hm__nil_ nil
#endif
#ifdef Nil
#define hm__Nil_ Nil
#endif
#undef nil
#undef Nil

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

//// Restore nil and Nil defines
#ifdef hm__nil_
#define nil hm__nil_
#undef hm__nil_
#endif
#ifdef hm__Nil_
#define Nil hm__Nil_
#undef hm__Nil_
#endif


namespace hm
{
	namespace expression
	{
		template <typename Iterator>
		struct Grammar;
		struct Program;
	}
	
	class NodeExpression : public NodeUnthreaded
	{
	public:
		NodeExpression(Node::Params params, std::string className="NodeExpression");
		
		bool setExpression(std::string const& expression);
		
	protected:
		virtual NodePtr create(Node::Params params) const override;
		virtual void step() override;
		
	private:
		void setLetCounts(int numScalarInlets, int numVectorInlets, int numOutlets);
		void expressionChangedCallback();
		
		std::unique_ptr<expression::Grammar<std::string::const_iterator>> mGrammar;
		std::string mExpression;
		std::unique_ptr<expression::Program> mProgram;
		bool mIsValid;
		double mLastTimestamp;
	};
	
	namespace expression
	{
		struct Empty {};
		struct Signed;
		struct Program;
		
		typedef boost::variant<
		Empty,
//		double,
		Data,
		boost::recursive_wrapper<Signed>,
		boost::recursive_wrapper<Program>
		> Operand;
		
		// A signed element, e.g. +x, -x
		struct Signed
		{
			char sign;
			Operand operand;
		};
		
		// An operation with its right hand operand
		struct Operation
		{
			char operator_;
			Operand operand;
		};
		
		// A program as a left hand operand and a sequence of right hand operations
		struct Program
		{
			Operand first;
			std::list<Operation> rest;
			
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Signed,
						  (char, sign)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Operation,
						  (char, operator_)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Program,
						  (hm::expression::Operand, first)
						  (std::list<hm::expression::Operation>, rest))



namespace hm
{
	namespace expression
	{
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		using std::cout;

		template <typename Iterator>
		struct Grammar : qi::grammar<Iterator, Program(), ascii::space_type>
		{
			qi::rule<Iterator, Operand(), ascii::space_type> factor;
			qi::rule<Iterator, Program(), ascii::space_type> term;
			qi::rule<Iterator, Program(), ascii::space_type> expression;
			//	qi::rule<Iterator, double(), ascii::space_type> paren;
			qi::symbols<std::string, InletPtr> inlets;
			qi::symbols<std::string, InletPtr> outlets;
			
			Grammar()
			: Grammar::base_type(expression)
			{
				using qi::lit;
				using qi::_val;
				using qi::_1;
				using qi::_2;
				using qi::double_;
				using boost::phoenix::construct;
				using boost::phoenix::bind;
				using qi::char_;
				
				factor =
				double_[_val = construct<Data>(_1)]
				/*| inlets[ bind([](InletPtr i) { i->data(); }, _1) ]*/
				| '(' >> expression >> ')'
				| (char_('-') >> factor)
				| (char_('+') >> factor);
				
				term = factor
				>> *((char_('*') >> factor)
					 | (char_('/') >> factor));
				
				expression = term
				>> *((char_('+') >> term)
					 | (char_('-') >> term));
			}
			
			// Based on http://www.boost.org/doc/libs/1_55_0/libs/spirit/example/qi/compiler_tutorial/calc4.cpp
			
		};

	}
}

#include "FactoryNode.h"
hm_register_node(NodeExpression, "Expression", "Write a mathematical expression to define how input is transformed into output.")