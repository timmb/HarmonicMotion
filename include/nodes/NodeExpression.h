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
		void setLetCounts(int numInlets, int numOutlets);
		void expressionChangedCallback();
		
		std::unique_ptr<expression::Grammar<std::string::const_iterator>> mGrammar;
		std::string mExpression;
		std::unique_ptr<expression::Program> mProgram;
		bool mIsValid;
		double mLastTimestamp;
		bool mDebugPrint;
	};
	
	
	
	namespace expression
	{
		struct Empty {};
		struct Signed;
		struct Expression;
		struct Output;
		struct Vec3
		{
			float x, y, z;
		};
		
		typedef boost::variant<
		double,
		Vec3
		> Constant;
		
		typedef boost::variant<
		Empty,
		Constant,
		InletPtr,
		boost::recursive_wrapper<Output>,
		boost::recursive_wrapper<Signed>,
		boost::recursive_wrapper<Expression>
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
		struct Expression
		{
			Operand first;
			std::list<Operation> rest;
			
		};

		// An output assignment that evaluates to what was outputted
		// e.g. o2 = 3+5
		struct Output
		{
			OutletPtr outlet;
			Expression expression;
			
			Output(OutletPtr const& outlet_, Expression const& expression_)
			: outlet(outlet_)
			, expression(expression_)
			{}
			
			Output() {}
		};
		
		typedef boost::variant<
		Expression,
		Output
		> Statement;
		
		struct Program
		{
			Statement statement;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Vec3,
						  (float, x)
						  (float, y)
						  (float, z))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Signed,
						  (char, sign)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Operation,
						  (char, operator_)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Expression,
						  (hm::expression::Operand, first)
						  (std::list<hm::expression::Operation>, rest))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Output,
						  (hm::OutletPtr, outlet)
						  (hm::expression::Expression, expression))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Program,
						  (hm::expression::Statement, statement))


namespace hm
{
	namespace expression
	{
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		using std::cout;
		using ascii::space_type;
		
		template <typename Iterator>
		struct Grammar : qi::grammar<Iterator, Program(), ascii::space_type>
		{
			qi::rule<Iterator, Vec3(), space_type> Vec3_;
			
			qi::rule<Iterator, Operand(), space_type> factor;
			qi::rule<Iterator, Expression(), space_type> term;
			qi::rule<Iterator, Expression(), space_type> expression;
			qi::rule<Iterator, Output(), space_type> output;
			qi::rule<Iterator, Statement(), space_type> statement;
			qi::rule<Iterator, Program(), space_type> program;
			
			qi::symbols<char, InletPtr> inlets;
			qi::symbols<char, OutletPtr> outlets;
			
#pragma clang diagnostic push
			// Ignore bogus clang warning about multiple unsequenced _val assignents
#pragma clang diagnostic ignored "-Wunsequenced"

			Grammar()
			: Grammar::base_type(program)
			{
				using qi::lit;
				using qi::_val;
				using qi::_1;
				using qi::_2;
				using qi::double_;
				using qi::float_;
				using boost::phoenix::construct;
				using boost::phoenix::bind;
				using qi::char_;
				
				Vec3_ %= ('(' >> float_ >> ',' >> float_ >> ',' >> float_ >> ')');//[_val = construct<Vec3>(_1, _2, _3)];
				
				factor %=
				Vec3_[_val = _1]
				| double_
				| inlets[_val = construct<InletPtr>(_1)]
				| '(' >> expression[_val=_1] >> ')'
				| (char_('-') >> factor)
				| (char_('+') >> factor);
				
				term %= factor
				>> *((char_('*') >> factor)
					 | (char_('/') >> factor));
				
				expression %= term >> *((char_('+') >> term) | (char_('-') >> term));
				
				output %= (outlets
						   >> '=' >> expression);//[_val = construct<Output>(_1, _2)];
				
				statement %= output | expression;
				
				program %= statement;
				
			}

#pragma clang diagnostic pop
			
			// Based on http://www.boost.org/doc/libs/1_55_0/libs/spirit/example/qi/compiler_tutorial/calc4.cpp
			
		};
		
	}
}

#include "FactoryNode.h"
hm_register_node(NodeExpression, "Expression", "Write a mathematical expression to define how input is transformed into output.")