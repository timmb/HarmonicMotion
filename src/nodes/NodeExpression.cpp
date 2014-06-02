#include "NodeExpression.h"

#include <string>



#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "Data.h"
#include "Inlet.h"
#include "Outlet.h"
//

using namespace std;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

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
			
			Data operator()(Data const& x) const
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
				BOOST_ASSERT(false && ("Invalid binary operator character "+to_string(op.operator_)).c_str());
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
				BOOST_ASSERT(false && ("Invalid unary operator character "+to_string(x.sign)).c_str());
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
		addParameter("Expression", &mExpression)->addNewExternalValueCallback([this](){ this->expressionChangedCallback(); });
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
				//			mGrammar->inlets.add(name, inlet);
			}
			//		for (; i<numScalarInlets + numVectorInlets; i++)
			//		{
			//			string name = "v"+to_string(i+1);
			//			InletPtr inlet = createInlet(VALUE, "3D Input "+name, "3D values (Point3d, Skeleton3d, Scene3d) inputted here here may be used as \""+name+"\" in your expression.");
			//			mGrammar->inlets.add(name, inlet);
			//		}
		}
		//		for (int j=0; j<numOutlets_; j++)
		//		{
		//			string name = "o"+to_string(j+1);
		//			InletPtr inlet = createInlet(VALUE, "Output "+name, "Output here here may be written to as \""+name+"\" in your expression.");
		//			mGrammar->inlets.add(name, inlet);
		//		}
		
	}
	
	void NodeExpression::expressionChangedCallback()
	{
		hm_debug("Attempting to parse "+mExpression);
		auto iter = mExpression.cbegin();
		mIsValid = qi::phrase_parse(iter, mExpression.cend(), *mGrammar, ascii::space, *mProgram);
		if (mIsValid)
		{
			hm_debug("Expression "+mExpression+" parsed successfully.");
		}
		else
		{
			hm_debug("Failed to parse "+mExpression);
		}
	}
	
	void NodeExpression::step()
	{
		if (mIsValid)
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
	
}




