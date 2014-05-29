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
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "Data.h"
#include "Inlet.h"
#include "Outlet.h"
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
	namespace expression
	{
		struct Empty {};
		struct Signed;
		struct Program;
		
		typedef boost::variant<
		Empty,
		double,
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
				
				factor =
				double_[ _val = construct<Data>(construct<Value>(_1)) ]
				| inlets[ bind([](InletPtr i) { i->data(); }, _1) ]
				| '(' >> expression >> ')';
				
				term = factor[_val = _1]
				>> *(('*' >> factor)[_val = _val * _1]
					 | ('/' >> factor)[_val = _val / _1]);
				
				expression = term[_val = _1]
				>> *(('+' >> term)[_val = _val + _1]
					 | ('-' >> term)[_val = _val - _1]);
			}
			
			// Based on http://www.boost.org/doc/libs/1_55_0/libs/spirit/example/qi/compiler_tutorial/calc4.cpp
			
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Signed,
						  (char, sign)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Operation,
						  (std::string, operator_)
						  (hm::expression::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(hm::expression::Program,
						  (hm::expression::Operand, first)
						  (std::list<hm::expression::Operation>, rest))
						  



namespace hm
{
	namespace expression
	{
		/// Evaluator visits an abstract syntax tree created from a parse
		/// and evaluates its outcome.
		struct Evaluator
		{
			typedef Data result_type;
			
			Data operator()(Empty) const
			{
				BOOST_ASSERT(false); return Data();
			}
			
			Data operator()(double x) const
			{
				return x;
			}
			
			Data operator()(Operation const& op, Data const& lhs) const
			{
				Data rhs = boost::apply_visitor(*this, op.operand);
				switch (op.operator_)
				{
					case '+': return lhs + rhs;
					case '-': return lhs - rhs;
					case '*': return lhs * rhs;
					case '/': return lhs / rhs;
				}
				BOOST_ASSERT(false);
				return Data();
			}
			
			Data operator()(Signed const& x) const
			{
				Data rhs = boost::apply_visitor(*this, x.operand);
				switch (x.sign)
				{
					case '-': return -rhs;
					case '+': return +rhs;
				}
				BOOST_ASSERT(false);
				return Data();
			}
			
			Data operator()(Program const& x) const
			{
				Data state = boost::apply_visitor(*this, x.first);
				for(Operation const& op: x.rest)
				{
					state = (*this)(op, state);
				}
				return state;
			}
		};
		
	} // namespace expression
	
	NodeExpression::NodeExpression(Node::Params params, string className)
	: NodeUnthreaded(params, className)
	, mIsValid(false)
	, mLastTimestamp(-42.)
	{
		int numScalarInlets = 2;
		int num3dInlets = 2;
		int numOutlets = 2;
		for (int i=0; i<numScalarInlets; i++)
		{
			createInlet(VALUE, "Scalar Input s"+std::to_string(i+1), "Scalar values inputted here here may be used as \"s"+to_string(i+1)+"\" in your expression.");
		}
		for (int i=0; i<num3dInlets; i++)
		{
			createInlet(VALUE, "3D Input v"+std::to_string(i+1), "3D values (Point3d, Skeleton3d, Scene3d) inputted here here may be used as \"v"+to_string(i+1)+"\" in your expression.");
		}
		for (int i=0; i<numOutlets; i++)
		{
			createOutlet(VALUE | VECTOR3D_TYPES, "Output o"+to_string(i+1), "Output here may be written to as \"o"+to_string(i+1)+"\" in your expression.");
		}
		mGrammar = unique_ptr<expression::Grammar<string::const_iterator>>(new expression::Grammar<string::const_iterator>);
	}
	
	NodePtr NodeExpression::create(Node::Params params) const
	{
		return NodePtr(new NodeExpression(params));
	}
	
	bool NodeExpression::setExpression(std::string const& expression)
	{
		mExpression = expression;
		expressionChangedCallback();
		return mIsValid;
	}
	
	void NodeExpression::setLetCounts(int numScalarInlets, int numVectorInlets, int numOutlets_)
	{
		// At the moment this function may only be called once.
		assert(numInlets()==0 && numOutlets()==0);
		mGrammar->inlets.clear();
		
		{
		int i=0;
		for (; i<numScalarInlets; i++)
		{
			string name = "s"+to_string(i+1);
			InletPtr inlet = createInlet(VALUE, "Scalar Input "+name, "Scalar values inputted here here may be used as \""+name+"\" in your expression.");
			mGrammar->inlets.add(name, inlet);
		}
		for (; i<numScalarInlets + numVectorInlets; i++)
		{
			string name = "v"+to_string(i+1);
			InletPtr inlet = createInlet(VALUE, "3D Input "+name, "3D values (Point3d, Skeleton3d, Scene3d) inputted here here may be used as \""+name+"\" in your expression.");
			mGrammar->inlets.add(name, inlet);
		}
		}
		for (int j=0; j<numOutlets_; j++)
		{
			string name = "o"+to_string(j+1);
			InletPtr inlet = createInlet(VALUE, "Output "+name, "Output here here may be written to as \""+name+"\" in your expression.");
			mGrammar->inlets.add(name, inlet);
		}
	}
	
	void NodeExpression::expressionChangedCallback()
	{
		auto iter = mExpression.cbegin();
		mIsValid = qi::phrase_parse(iter, mExpression.cend(), mGrammar, ascii::space, mProgram);
	}
	
	void NodeExpression::step()
	{
		// check if any new data has arrived
		double t = 0.;
		for (int i=0; i<numInlets(); i++)
		{
			t = max(t, inlet(i)->dataTimestamp());
		}
		if (t > mLastTimestamp)
		{
			expression::Evaluator evaluator;
			evaluator(*mProgram);
		}
	}

}




