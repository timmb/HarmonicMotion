#include "NodeExpression.h"

#include <string>



#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "Data.h"
#include "Inlet.h"
#include "Outlet.h"
#include "Pipeline.h"
//

using namespace std;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;




namespace hm
{
	namespace expression
	{
		/// Evaluator visits an abstract syntax tree created from a parse
		/// and evaluates its outcome.
		struct Evaluator
		{
			typedef Data result_type;
			double timestamp;
			std::vector<std::string> errors;
			
			Evaluator(double timestamp_)
			: timestamp(timestamp_)
			{}
			
			Data operator()(Empty) const
			{
//				BOOST_ASSERT(false);
				cerr << "ERROR: Empty passed into Evaluator"<<endl;
				return Data();
			}
			
			Data operator()(double const& x) const
			{
				return Data(Value(x, timestamp));
			}
			
			Data operator()(Vec3 const& v)
			{
				Data x = boost::apply_visitor(*this, v.x);
				Data y = boost::apply_visitor(*this, v.y);
				Data z = boost::apply_visitor(*this, v.z);
				if (x.isValue() && y.isValue() && z.isValue())
					return Point3d(x.asValue(), y.asValue(), z.asValue());
				else
				{
					char errorComponent =
					!x.isValue()? 'x' :
					!y.isValue()? 'y' :
					'z';
					BOOST_ASSERT(errorComponent!='z' || !z.isValue());
					errors.push_back(string("Cannot construct vector as ")
									 +errorComponent+
									 " component is not a scalar Value (types are x: "
									 + to_string(x.type()) + ", y: "
									 + to_string(y.type()) + ", z: "
									 + to_string(z.type()) + ").");
					return Data();
				}
			}
			
			Data operator()(Data const& x) const
			{
				return x;
			}
			
			Data operator()(InletPtr const& i) const
			{
				return i->data();
			}
			
			Data operator()(Data const& lhs, Operation const& op)
			{
				Data rhs = boost::apply_visitor(*this, op.operand);
				switch (op.operator_)
				{
					case '+':
						if (lhs.canAdd(rhs))
						{
							return lhs + rhs;
						}
						else
						{
							return failWithOp("add", lhs, rhs);
						}
					case '-':
						return lhs.canSubtract(rhs)? lhs-rhs : failWithOp("subtract", lhs, rhs);
					case '*':
						return lhs.canMultiply(rhs)? lhs*rhs : failWithOp("multiply", lhs, rhs);
					case '/':
						return lhs.canDivide(rhs)? lhs/rhs : failWithOp("divide", lhs, rhs);
				}
				BOOST_ASSERT_MSG(false, ("Invalid binary operator character "+to_string(op.operator_)).c_str());
				return Data();
			}
			
			Data operator()(double lhs_, Operation const& op)
			{
				Data lhs(Value(lhs_, timestamp));
				Data rhs = boost::apply_visitor(*this, op.operand);
				switch (op.operator_)
				{
					case '+':
						if (lhs.canAdd(rhs))
						{
							return lhs + rhs;
						}
						else
						{
							return failWithOp("add", lhs, rhs);
						}
					case '-':
						return lhs.canSubtract(rhs)? lhs-rhs : failWithOp("subtract", lhs, rhs);
					case '*':
						return lhs.canMultiply(rhs)? lhs*rhs : failWithOp("multiply", lhs, rhs);
					case '/':
						return lhs.canDivide(rhs)? lhs/rhs : failWithOp("divide", lhs, rhs);
				}
				BOOST_ASSERT_MSG(false, ("Invalid binary operator character "+to_string(op.operator_)).c_str());
				return Data();
			}
			
			Data operator()(Signed const& x)
			{
				Data rhs = boost::apply_visitor(*this, x.operand);
				switch (x.sign)
				{
					case '-': return -rhs;
					case '+': return +rhs;
				}
				BOOST_ASSERT_MSG(false, ("Invalid unary operator character "+to_string(x.sign)).c_str());
				return Data();
			}
			
			Data operator()(Expression const& x)
			{
				Data state = boost::apply_visitor(*this, x.first);
				for(Operation const& op: x.rest)
				{
					state = (*this)(state, op);
				}
				return state;
			}
			
			Data operator()(Output const& out)
			{
				Data rhs = (*this)(out.expression);
				if (!rhs.isNull())
				{
					out.outlet->outputNewData(rhs);
				}
				return rhs;
			}
			
			Data operator()(Program const& x)
			{
				return boost::apply_visitor(*this, x.statement);
			}
			
		private:
			Data failWithOp(std::string const& opName, Data const& lhs, Data const& rhs)
			{
				errors.push_back("Cannot "+opName+" a "+to_string(lhs.type())+" to a "+to_string(rhs.type()));
				return Data();
			}
		};
		
	} // namespace expression
	
	NodeExpression::NodeExpression(Node::Params params, string className)
	: NodeUnthreaded(params, className)
	, mIsValid(false)
	, mLastTimestamp(-42.)
	, mGrammar(new expression::Grammar<string::const_iterator>)
	, mProgram(new expression::Program)
	, mDebugPrint(false)
	, mRequestedNumInlets(1)
	, mRequestedNumOutlets(1)
	, mIsReplacingThisNode(false)
	{
		auto numInletsParam = addParameter("Number of inlets", &mRequestedNumInlets);
		numInletsParam->addNewExternalValueCallback([this](){
			this->letCountsChangedCallback();
		});
		numInletsParam->hardMin = numInletsParam->softMin = 0;
		numInletsParam->hardMax = numInletsParam->softMax = 100;
		
		auto numOutletsParam = addParameter("Number of outlets", &mRequestedNumOutlets);
		numOutletsParam->addNewExternalValueCallback([this](){
			this->letCountsChangedCallback();
		});
		numOutletsParam->hardMin = numOutletsParam->softMin = 0;
		numOutletsParam->hardMax = numOutletsParam->softMax = 100;

		addParameter("Expression", &mExpression)->addNewExternalValueCallback([this](){
			this->expressionChangedCallback();
		});
//		addParameter("Print final expression (debug)", &mDebugPrint);
		setLetCounts(mRequestedNumInlets, mRequestedNumOutlets);
		expressionChangedCallback();
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
	
	void NodeExpression::setLetCounts(int inletCount, int outletCount)
	{
		assert(mRequestedNumInlets >= 0 && mRequestedNumOutlets >= 0);
		{
			int i = numInlets();
			while (i > mRequestedNumInlets)
			{
				i--;
				InletPtr inlet_ = inlet(i);
				assert(inlet_);
				if (inlet_)
				{
					removeInlet(inlet_);
				}
			}
			while (i < mRequestedNumInlets)
			{
				string name = "i"+to_string(i+1);
				InletPtr inlet = createInlet(ALL_TYPES, "Input "+name, "Scalar values inputted here here may be used as \""+name+"\" in your expression.");
				i++;
			}
		}
		{
			int i = numOutlets();
			while (i > mRequestedNumOutlets)
			{
				i--;
				OutletPtr outlet_ = outlet(i);
				assert(outlet_);
				if (outlet_)
				{
					removeOutlet(outlet_);
				}
			}
			while (i < mRequestedNumOutlets)
			{
				string name = "o"+to_string(i+1);
				OutletPtr outlet = createOutlet(ALL_TYPES, "Output "+name, "Output here here may be written to as \""+name+"\" in your expression.");
				i++;
			}
		}
		
		// Update symbol table in mGrammar
		
		mGrammar->inlets.clear();
		{
			int i=0;
			for (InletPtr inlet: inlets())
			{
				string name = "i"+to_string(i+1);
				mGrammar->inlets.add(name.c_str(), inlet);
				i++;
			}
		}
		{
			int i=0;
			for (OutletPtr outlet: outlets())
			{
				string name = "o"+to_string(i+1);
				mGrammar->outlets.add(name.c_str(), outlet);
				i++;
			}
		}
	}
	
	void NodeExpression::expressionChangedCallback()
	{
		if (mIsReplacingThisNode)
		{
			return;
		}
		auto iter = mExpression.cbegin();
		mIsValid = qi::phrase_parse(iter, mExpression.cend(), *mGrammar, ascii::space, *mProgram);
		mIsValid = mIsValid && iter == mExpression.cend();
		if (mIsValid)
		{
			hm_debug("Expression "+mExpression+" parsed successfully.");
		}
		else
		{
			hm_debug("Failed to parse "+mExpression);
		}
	}
	
	void NodeExpression::letCountsChangedCallback()
	{
		// Note that this callback is triggered by updateParameters
		// which will not happen concurrently with step()
		setLetCounts(mRequestedNumInlets, mRequestedNumOutlets);
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
				expression::Evaluator evaluator(t);
				Data outcome = evaluator(*mProgram);
				if (mDebugPrint)
				{
					cout << t << " :" << outcome << endl;
				}
				if (evaluator.errors.size())
				{
					cout << "Evaluation errors: \n";
					for(string error: evaluator.errors)
					{
						cout << error << ". ";
					}
					cout << endl;
				}
				mLastTimestamp = t;
			}
		}
	}
	
}




