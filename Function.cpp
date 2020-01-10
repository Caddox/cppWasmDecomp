#include "Function.h"
#include "opcodes.h"


///	<summary>Default constructor. Nothing is set up.</summary>
Function::Function()
= default;

///	<summary>Constructor that sets up the inputs and outputs.</summary>
Function::Function(std::vector<dataDef> funcIns, dataDef& funcOuts)
{
	setInputs(funcIns);
	setOutput(funcOuts);
}

///	<summary>Setter for the inputs of the function</summary>
void Function::setInputs(std::vector<dataDef> ins)
{
	funcInputs = ins;
}

/// <summary>Setter that sets outputs for the function.</summary>
void Function::setOutput(dataDef& out)
{
	funcOutputs = out;
}

/// <summary>Setter that sets the internal title of the function.</summary>
void Function::setTitle(std::string name)
{
	funcName = name;
}

// Not even sure how to approach this lmaoi
void Function::decodeFunction(std::vector<byte>& in)
{
	//using namespace opcodes;
	// Go byte by byte over the byte code.
	// We are going to hold the function body as a linked list
	// To emulate imperative code, we are going to 'create' a stack
	// based decoder, wherein the operations that do not pop the stack
	// do not build a command.
	// THIS is the hard part.
	byteCode = in;

	std::stack<oper> operStack;
	std::stack<statement*> statementStack;
	int endsNeeded = 1;

	// Iterate over each byte, determining if it is an operator or an operand
	while(endsNeeded != 0)
	{
		// We are basically changing equations from Reverse Polish Notation into Infix notation for readability.
		opcodes::ins next = opcodes::find(nextByte());
		oper temp{ next };// Void for later

		// First, check if the next instruction requires any following bytes
		if(next.associatedBytes != 0)
		{
			// Associate the bytes
			temp.bytes.resize(next.associatedBytes);
			getValueBytes(next, temp);
			//for (int i = 0; i < next.associatedBytes; ++i)
				//temp.bytes[i] = nextByte();
		}

		// Then, check if the operation is a blocking operation that would require more ends
		if(next.op >= 0x02 && next.op <= 0x03)
		{
			endsNeeded++;

			// Push a start statement onto the statement stack
			auto start = new statement;
			start->operation = next;
			statementStack.push(start);

			// Eat the block type before continuing
			nextByte();
			continue;
		}

		// To handle external calls, check the function itself to see how many other items need to come off the stack
		// Do this by setting the temp's bytesComsumed equal to the size of the function inputs
		//if(next.op == 0x10 || next.op == 0x11)
		//{
		//	temp.op.bytesConsumed += funcInputs.size();
		//}
		// We can't actually do this right now, so just ignore it.


		// Now, check if the instruction consumes any previous bytes
		if(next.bytesConsumed != 0)
		{
			// Consume them
			// Create a statement structure to hold the information
			statement* newStatement = new statement;
			statement* left = new statement;
			statement* right = nullptr;

			// Populate the branches
			// If the operation stack is empty, pull off of the statement stack
			if(operStack.empty())
			{
				left = statementStack.top();
				statementStack.pop();
			}
			else // Not empty, pull off the operation Stack
			{
				left->operation = operStack.top();
				operStack.pop();
			}

			if (next.bytesConsumed > 1)
			{
				right = new statement;
				if(operStack.empty())
				{
					right = statementStack.top();
					statementStack.pop();
				}
				else
				{
					right->operation = operStack.top();
					operStack.pop();
				}
			}

			// If there is only one operand, only the left side is populated.
			newStatement->operation = temp;
			newStatement->leftOperand = left;
			newStatement->rightOperand = right;
			statementStack.push(newStatement);
			continue;
		}


		// If we encounter an end:
		if(next.op == 0x0b)
		{
			if((endsNeeded - 1) != 0)
			{
				auto end = new statement;
				end->operation = next;
				statementStack.push(end);
			}
			endsNeeded--;
			continue;
		}

		// If the instruction gets here, push it onto the stack
		operStack.push(temp);

	}

	// Now, we have to unwind the stack(s) to establish what our functions actually are.
	ops = operStack;
	statements = statementStack;
}

/// <summary>Getter function that returns the title</summary>
std::string Function::getTitle()
{
	return funcName;
}

int Function::getInputSize() const
{
	return funcInputs.size();
}

/// <summary>This function emits the next byte in the byte stream sequence
/// and keeps track of where you are. It starts at 0 and goes to the end.</summary>
byte Function::nextByte()
{
	// Sanity check
	if (loc >= byteCode.size())
		throw std::exception("Function byteCode length exceeded");

	const byte toRet = byteCode[loc];
	loc++;
	return toRet;
}

/// <summary>This function will determine from hints given by the instruction 
/// and the bytes how many bytes to associate with the operation. This is 
/// required to to cover the bases of signed/unsigned encoded values
/// and floating points.</summary>
void Function::getValueBytes(const opcodes::ins& ins, oper& oper)
{
	std::vector<byte> temp;
	int maxLength = 1;

	if (ins.op == 0x41 || ins.op == 0x43)
		maxLength = 4;
	else if (ins.op == 0x42 || ins.op == 0x44)
		maxLength = 8;

	if (ins.op == 0x41 || ins.op == 0x42) // Integers can be any length up to the max
	{
		for (int k = 0; k < ins.associatedBytes; ++k)
		{
			byte next = nextByte();

			for (int i = 0; i < maxLength; ++i)
			{
				if (next <= 0x7f) // && (ins.op != 0x42 && ins.op != 0x44)))
				{
					// break out
					temp.push_back(next);
					break;
				}
				temp.push_back(next);
				next = nextByte();
			}
		}
	}
	else // Floats are always (?) the byte size
	{
		for(int k = 0; k < ins.associatedBytes; ++k)
		{
			//byte next = nextByte();

			for(int i = 0; i < maxLength; ++i)
			{
				byte next = nextByte();
				temp.push_back(next);
			}
		}
	}
	oper.bytes = temp;
}

/// <summary>Given a statement, print out the commands that take place.</summary>
std::string printStatement(statement* top)
{
	std::string output;
	std::string leftOut;
	std::string rightOut;
	statement* left = top->leftOperand;
	statement* right = top->rightOperand;

	// Drill down
	if(left != nullptr)
	{
		leftOut += printStatement(left);
		leftOut = '(' + leftOut + ')';
	}

	if(right != nullptr)
	{
		rightOut += printStatement(right);
		rightOut = '(' + rightOut + ')';
	}

	// If at the bottom
	output = top->operation.op.symbol;
	if(top->operation.op.op >= 0x41 && top->operation.op.op <= 0x44)
	{
		// Decode the constant into it's value
		auto op = top->operation.op.op;
		std::stringstream ss;
		if(op == 0x41 || op == 0x43) // 4 bytes max each
		{
			char list[4] = { 0x80, 0x80, 0x80, 0x80 };
			int i = 0;
			for (auto o : top->operation.bytes)
			{
				list[i] = o;
				++i;
			}
			if (op == 0x41) //integer
				ss << ' ' << u_decLEB32(list);
			else
				ss << ' ' << decodeFloat(list);

			output += ss.str();
		}
		else // 8 bytes max each
		{
			char list[8] = { 0x80, 0x80, 0x80, 0x80,
							 0x80, 0x80, 0x80, 0x80};
			int i = 0;
			for (auto o : top->operation.bytes)
			{
				list[i] = o;
				++i;
			}
			if (op == 0x42) //integer
				ss << ' ' << u_decLEB32(list);
			else
				ss << ' ' << decodeDouble(list);
			output += ss.str();
		}
	}
	else
	{
		for (auto byte : top->operation.bytes)
		{
			std::stringstream ss;
			ss << (int)byte;
			output += ' ' + ss.str();
		}
	}

	if (left == nullptr && right == nullptr)
		return output;
	if (left != nullptr && right == nullptr)
		return output + ' ' + leftOut;
	return rightOut + ' ' + output + ' ' + leftOut; // Backwards because Reverse Polish Notation (aka I'm dumb)
}

std::ostream& operator<<(std::ostream& out, const Function& c)
{	
	// The statement stack is actually backwards from what we want to print
	// to the ostream. Start by reversing it
	std::stack<statement*> toReverse = c.statements; // NOTE: Make sure this copies rather than references.
	std::stack<statement*> output;
	while(!toReverse.empty())
	{
		output.push(toReverse.top());
		toReverse.pop();
	}

	// Now that the stack has been reversed, we can do some formatting!
	// Start with the function signature
	if (c.funcOutputs.type == i32)
		out << "i32 ";
	else if (c.funcOutputs.type == i64)
		out << "i64 ";
	else if (c.funcOutputs.type == f32)
		out << "f32 ";
	else if (c.funcOutputs.type == f64)
		out << "f64 ";
	else if (c.funcOutputs.type == none)
		out << "void ";

	// Add the function name
	out << c.funcName << "(";

	std::string inputs;
	// Add the input definitions
	for (const auto& func_input : c.funcInputs)
	{
		if (func_input.type == i32)
			inputs += "i32 " + func_input.name + ", ";
		else if (func_input.type == i64)
			inputs += "i64 " + func_input.name + ", ";
		else if (func_input.type == f32)
			inputs += "f32 " + func_input.name + ", ";
		else if (func_input.type == f64)
			inputs += "f64 " + func_input.name + ", ";
		else if (func_input.type == none)
			inputs += "void " + func_input.name + ", ";
	}
	// Remove the trailing comma and space
	inputs = inputs.substr(0, inputs.size() - 2);
	out << inputs;
	out << ')' << std::endl;


	// Now we have to print from our reversed stack thing for each statement.
	int tabs = 1;
	while(!output.empty())
	{
		std::string written = printStatement(output.top());
		output.pop();

		if (written == "end")
			tabs--;

		out << tabMaker(tabs) << written << std::endl;

		// Figure out the number of tabs to place before the item
		if (written == "block"
			|| written == "loop"
			|| written.find("if") != std::string::npos)
		{
			// Check for a br_if, which does not constitute a tab
			if (written.substr(0, 5) != "br_if")
				tabs++;
		}

	}

	// If there is anything on the operation stack, print it now
	statement temp;
	if (!c.ops.empty())
	{
		temp.operation = c.ops.top();
		out << tabMaker(1) << printStatement(&temp);
	}
	//for(int i = 0; i < c.ops.size(); ++i)
	//{
	//	out << tabMaker(1) << c.ops.top().op.symbol;
	//	for (byte elm: c.ops.top().bytes)
	//	{
	//		std::stringstream ss;
	//		ss << (int)elm;
	//		out << ' ' + ss.str();
	//	}
	//}

	// Put the final end in the function
	out << std::endl << tabMaker(0) << "end" << std::endl;


	return out;
}

std::string tabMaker(int size)
{
	std::stringstream ss;
	for (int i = 0; i < size; ++i)
		ss << '\t';

	return ss.str();
}