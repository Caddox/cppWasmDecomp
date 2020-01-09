#include "Mediator.h"

Mediator::Mediator(Sectioner* worker)
{
	this->worker = worker; // Save the reference to the worker

	// Using the reader, extract the sections of the functions out of the binary
	extractFunctions();
}

void Mediator::extractFunctions()
{
	// First, extract the signatures and names out of the functions.
	extractSignatures();
	extractNames();

	// Now that we have the basis setup, we need to put the bytecode into each function
	std::vector<byte> byteString = worker->getSection(0x0a);

	// The first byte is the number of functions that will get extracted
	int offset = 0;
	const byte numberOfFuncs = byteString[offset];
	offset++;

	// for each function. . .
	for(int i = 0; i < numberOfFuncs; ++i)
	{
		// The first four bytes are the encoded function length, so extract that.
		char lebLength[4];
		for (char& k : lebLength)
		{
			k = byteString[offset];
			offset++;
		}
		int size = u_decLEB32(lebLength);
		int nextStop = size + offset + 1;

		// The next byte is padding, so we skip it
		offset++;

		// The next set of bytes is the locals vector, which defines how many of
		// each variable type can be defined in each function body.
		// We do not care about this at all, so we have to skip the vector.
		byte localSize = byteString[offset];
		offset++;

		if(localSize != 0x00)
		{
			// We can do some math here to skip forward, if we establish a few things:
			//		1) The vector represents how many different types there are
			//		2) There can be four types max
			//		3) Each different type is represented in two bytes max.
			// Therefore, we can adjust the offset by (localSize) * 2
			// 6 initial: 6 + (1 * 2) = 8
			offset += (localSize * 2);
		}
		// If the size is zero, we do not need to move.

		// Now, copy a section into a new vector and pass it to the function 
		std::vector<byte> byteCode;
		for(int k = 0; offset < nextStop; ++k)
		{
			byteCode.push_back(byteString[offset]);
			offset++;
		}

		// Send the function byte code to the function representation for it to do it's thing.
		funcs[i].decodeFunction(byteCode);
		std::cout << funcs[i] << std::endl;
	}
}

/// <summary>Now that the function definitions have been extracted, we need to figure out which
/// two things:<br>
///		1) How many functions there are, and<br>
///		2) What type definitions each function gets.<br>
///	This information is contained in section 3</summary>
void Mediator::populateFunctions(std::vector< std::pair< std::vector<dataDef>, dataDef> > pairs)
{
	// Extract the byte code for section 3
	std::vector<byte> byteCode = worker->getSection(3);

	// The first bytes are representative of the length of the section, which is the number of defined functions in the module
	int offset = 0;
	int size = 0;
	offset += getNextu32(byteCode, 0, size);

	// For each value from the offset to the end, we need to create a function
	// with the definitions provided in our pairs array.
	for(int i = 0; i < size; ++i)
	{
		byte idx = byteCode[offset];
		offset++;

		// Double check that the imported functions are established correctly.

		Function next = Function(pairs[idx].first, pairs[idx].second);
		funcs.push_back(next);
	}
	
}

/// <summary>Some modules may contain references to other modules and their functions.
/// In this case, the functions used become part of THIS modules function table.
/// Therefore, we have to load the import section and parse through it to add
/// references to functions that we have no definition for, just a name and funcidx.</summary>
std::map<int, Function> Mediator::extractImports() const
{
	// Load the byte data for this section. It it is empty, return.
	std::map<int, Function> importedFuncs;
	std::vector<byte> byteString = worker->getSection(0x02);
	if (byteString.empty())
		return importedFuncs;

	// The first byte represents the number of imported functions to be used in our module
	int numberOfFuncs;
	int offset = getNextu32(byteString, 0, numberOfFuncs);

	// Now we loop
	for(int i = 0; i < numberOfFuncs; ++i)
	{
		std::stringstream ss;
		// The first byte represents the length of the module string
		int strLen;
		offset += getNextu32(byteString, offset, strLen);
		int stop = offset + strLen;

		// Loop over that number of letters.
		for(int k = offset; k < stop; ++k)
		{
			ss << (char)byteString[k];
		}
		offset += strLen;

		// The next byte is the length of the next part of the name
		offset += getNextu32(byteString, offset, strLen);
		stop = offset + strLen;

		// Loop over all the letters
		ss << "::";
		for(int k = offset; k < stop; ++k)
		{
			ss << (char)byteString[k];
		}
		offset += strLen;

		// The next byte tells us if it is a function or something else. For now, we only care about 0x00
		byte importDesc = byteString[offset];
		offset++;
		if (importDesc == 0x00)
		{
			// The next byte is the funcidx that we need
			Function next = Function();
			next.setTitle(ss.str());

			byte funcidx = byteString[offset];
			offset++;
			importedFuncs[funcidx] = next;
		}
		else
			offset++;
	}

	return importedFuncs;
}

/// <summary>
/// Working with section 01, we can extract what inputs 
/// and outputs the function will work with for each funcidx. Also
/// extracts the number of functions present in the module.
/// </summary>
void Mediator::extractSignatures()
{
	// Access section 1 from the reader.
	std::vector<byte> byteString = worker->getSection(0x01);
	std::vector<std::pair<std::vector<dataDef>, dataDef>> types; // This definition is to help match multiple functions with the same definitions.

	// The first byte is the section ID
	// The next 4 bytes denote the length of the section, followed by padding zeros

	// The vector starts at the first byte we can look at, so that's the length.
	numberOfFuncs = byteString[0x00]; // It's also the number of function signature's present.
	//funcs.resize(numberOfFuncs);

	// Now, for each func determine the input and output signatures of each
	int offset = 0x01;
	for(int i = 0; i < numberOfFuncs; ++i)
	{
		std::vector<dataDef> inputs;
		dataDef output;

		// First byte should be 0x60
		if (byteString[offset] != 0x60)
			throw std::exception("Malformed signature section");

		// Next is a vector length
		offset++;
		byte vectorLength = byteString[offset]; // TODO: Modify this so that it works with u32 bit numbers up to 4 bytes long
		offset++;

		// The next vectorLength bytes represent the signatures of the incoming variables.
		// If it is zero, there are no inputs.
		if (vectorLength != 0x00)
		{
			for (int k = 0; k < vectorLength; ++k)
			{
				byte workingByte = byteString[offset];
				offset++;

				// String stream for ease of naming.
				std::stringstream ss;

				// Determine the correct enum for the data type
				if (workingByte == 0x7f)
				{
					ss << "__i32_invar_" << k;
					inputs.push_back(dataDef{ dataTypes::i32, false, ss.str() });
				}
				else if (workingByte == 0x7e)
				{
					ss << "__i64_invar_" << k;
					inputs.push_back(dataDef{ dataTypes::i64, false, ss.str() });
				}
				else if (workingByte == 0x7d)
				{
					ss << "__f32_invar_" << k;
					inputs.push_back(dataDef{ dataTypes::f32, false, ss.str() });
				}
				else if (workingByte == 0x7c)
				{
					ss << "__f64_invar_" << k;
					inputs.push_back(dataDef{ dataTypes::f64, false, ss.str() });
				}


			}

			// The next vector represents the return type
		}
		else
		{
			// If there are no inputs, pass a void type
			inputs.push_back(dataDef{ none, false, "void" });
			//offset++;
		}

		// The next vector represents the return type
		vectorLength = byteString[offset];
		offset++;
		if(vectorLength != 0x00)
		{
			byte workingByte = byteString[offset];
			offset++;

			// Determine what type is being returned.
			if (workingByte == 0x7f)
				output = dataDef{ dataTypes::i32, false, "__i32_out" };
			else if (workingByte == 0x7e)
				output = dataDef{ dataTypes::i64, false, "__i64_out" };
			else if (workingByte == 0x7d)
				output = dataDef{ dataTypes::f32, false, "__f32_out"};
			else if (workingByte == 0x7c)
				output = dataDef{ dataTypes::f64, false, "__f64_out"};
		}
		else
		{
			output = dataDef{ none, false, "void" };
			//offset++;
		}
		// What we have now is a pair assigned to a value
		// eg: [key] : [std::pair(inputs, outputs)]
		// From here, we need to look at another section to determine the which function gets what pair definition
		types.emplace_back(inputs, output);


		// Finally, create a new function and add it to the functions vector.
		//Function next = Function(inputs, output);
		//funcs[i] = next;
	}

	// Parse through the import section if it exists to ensure we have all the function definitions in our vector.
	//extractImports(types);

	// Give each function their typing
	populateFunctions(types);
}

/// <summary>
/// Now that we have the signatures, give each function
/// the name that it has in the module
/// </summary>
void Mediator::extractNames()
{
	// For this, we need to look at the bytes present in section 7
	std::vector<byte> byteString = worker->getSection(7);
	// Figure out how many items are being exported.
	byte numberOfExports = byteString[0];

	// Iterate and extract all the names
	int offset = 1;
	for(int i = 0; i < numberOfExports; ++i)
	{
		// Start by grabbing the name length
		byte nameLength = byteString[offset];
		offset++;
		int initialOffset = offset;

		// Extract the name into a std::string.
		std::stringstream ss;

		while(offset < (initialOffset + nameLength))
		{
			ss << (char)byteString[offset];
			offset++;
		}

		// Grab the idxType byte
		byte idxType = byteString[offset];
		offset++;

		// Grab the idxValue
		byte idxValue = byteString[offset];
		offset++;

		// Attach the names to the corresponding functions.
		// Only do this operation if the idx corresponds to a function
		if(idxType == 0x00)
		{
			funcs[idxValue].setTitle(ss.str());
		}

	}
}
