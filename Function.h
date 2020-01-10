#ifndef FUNCTION_H
#define FUNCTION_H
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include "Sectioner.h"
#include "opcodes.h"

///	<summary>Defines the data types that WASM works with.
///	'none' is used in place of void, but they are equivalent
///	</summary>
enum dataTypes
{
	i32, // The iN values are interpreted as u32 until any operation occurs to convert them.
	i64,
	f32,
	f64,

	none // Placeholder for void
};

///	<summary>Simple struct to define what a full data definition looks like.</summary>
struct dataDef
{
	dataTypes type{};
	bool pointer = false;
	std::string name{};
};

struct oper
{
	opcodes::ins op{ 0xef, "error" };
	std::vector<byte> bytes{};

	oper() = default;

	oper(opcodes::ins& operation)
	{
		op = operation;
	}

	oper(opcodes::ins& operation, std::vector<byte>& in)
	{
		op = operation;
		bytes = in;
	}
};

struct statement
{
	oper operation{};
	statement* leftOperand  = nullptr;
	statement* rightOperand = nullptr;
};

std::string printStatement(statement* top);
std::string tabMaker(int size);

///	<summary>
///	Class that provides an abstract representation of a function. 
///	Theoretically, it will be able to transform into any language
///	that is compilable to WASM
///	</summary>
class Function
{
public:
	Function();
	Function(std::vector<dataDef> funcIns, dataDef& funcOuts);
	void setInputs(std::vector<dataDef> ins); // TODO: Watch this to make sure it stays in line.
	void setOutput(dataDef& out);
	void setTitle(std::string name);
	void decodeFunction(std::vector<byte>& in);

	friend std::ostream& operator<<(std::ostream& out, const Function& c); // Print function to see the current results.
	std::string getTitle();
	int getInputSize() const;
	int getOtherInputSize(int funcIdx);

	std::stack<statement*> statements;
	bool isImported = false;

private:
	byte nextByte();
	void getValueBytes(const opcodes::ins& ins, oper& oper);

	std::string funcName{};
	std::vector<dataDef> funcInputs{};
	dataDef funcOutputs{};
	std::stack<oper> ops;

	std::vector<byte> byteCode{};
	int loc = 0;
};



#endif // FUNCTION_H
