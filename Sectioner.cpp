#include "Sectioner.h"

///	<summary>Returns the size of a file in bytes.</summary>
int getFileSize(const std::string& cs)
{
	std::ifstream infile(cs.c_str(), std::ios::binary);
	if(!infile)
	{
		std::string ex = "File " + cs + " failed to open";
		throw std::exception(ex.c_str());
	}
	infile.seekg(0, std::ios::end);
	int temp = infile.tellg();
	infile.close();
	return temp;
}

Sectioner::Sectioner(std::string& filename)
{
	this->filename = filename;
	sectionListings.resize(12);

	// Grab the file size while we're here
	fileSize = getFileSize(filename);

	//readFromFile(filename);
	sectionBytes();
}

/// <Summary> Read the entire file into a vector detailing the byte values </Summary>
void Sectioner::readFromFile(std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	//rawByteString = std::vector<byte>((std::istreambuf_iterator<char>(file)),
		//std::istreambuf_iterator<char>());
}

///	<summary>Return a vector with the byte string in it from the section listings. Valid ID's range from 0 -> 11</summary>
std::vector<byte> Sectioner::getSection(int id)
{
	if (id >= 0 && id < 12)
		return sectionListings[id];

	throw std::exception("Bad parameter for getSection.");
}

/// <Summary> Function that reads n bytes from the offset specified from the target file. </Summary>
void Sectioner::readNBytesFromFile(int n_bytes, int fileOffset, char* out) const
{
	std::ifstream is(filename, std::ios::binary);

	is.seekg(fileOffset);

	is.read(out, n_bytes);

	if(!is) // File fails somehow.
	{
		std::cout << "Error in reading from file: " << filename << std::endl;
		std::cout << "The program will now close." << std::endl;
		is.close();
		exit(1);
	}

	is.close();
}

/// <summary>Wrapper for the overloaded function that returns a vector of bytes
/// rather than a char array.
/// </summary>
std::vector<byte> Sectioner::readNBytesFromFile(int n_bytes, int fileOffset) const
{
	std::vector<byte> temp;
	temp.resize(n_bytes);
	char* readTo = new char[n_bytes];

	// Read the values from the file into a char*
	readNBytesFromFile(n_bytes, fileOffset, readTo);

	// Convert those into a vector form
	for (int i = 0; i < n_bytes; ++i)
		temp[i] = readTo[i];

	// Clean up and return.
	delete[] readTo;
	return temp;
}

///	<summary>A helper function to print out some formatted info about
///	the sections that the sectioner has determined. 
///	</summary>
void Sectioner::sectionReport()
{
	// Iterate through each section in the section listings
	for(int i = 0; i < sectionListings.size(); ++i)
	{
		if(!sectionListings[i].empty())
		{
			std::cout << "Section " << std::setw(2) << i << " present.";
			std::cout << std::setw(20) << sectionListings[i].size() << " Bytes" << std::endl;
		}
		else
		{
			std::cout << "Section " << std::setw(2) << i << " is not present." << std::endl;
		}
	}
}

/// <summary>Breaks the list of bytes into each section. </summary>
void Sectioner::sectionBytes()
{
	// Start iterating through the bytes

	// Grab each section and jump by the size of the section 
	// to the next one.

	int fileOff = 0;
	int sectionID;
	int sectionSize;
	char* size = new char[4]; // Char pointer for the sizes.

	// Read the first 8 bytes to verify the magic signature.
	std::vector<byte> out = readNBytesFromFile(8, 0);
	byte magic[] = { 0, 0x61, 0x73, 0x6d }; // \0asm
	for (int i = 0; i < 4; ++i)
	{
		if (out[i] != magic[i])
			// For now, just throw
			throw std::exception("The magic string was malformed.");
	}

	// Write the version of the wasm to the console
	std::cout << "WASM Version: ";
	for (int i = 4; i < 8; ++i)
	{
		if (out[i] != '\0')
			std::cout << (int)out[i];
	}
	std::cout << std::endl;

	// Move the offset to the start of the actual data.
	fileOff += 8;

	// Now, loop over the other sections to extract their contents.
	while (fileOff < fileSize)
	{
		// Grab the section id
		out = readNBytesFromFile(1, fileOff);
		fileOff += 1;
		sectionID = out.front();

		// Next 4 bytes are the size of the section
		readNBytesFromFile(4, fileOff, size);
		fileOff += 4;
		sectionSize = u_decLEB32(size);

		// Get the contents of the section
		out = readNBytesFromFile(sectionSize, fileOff + 1); // +1 because SpiderMonkey is dumb.
		fileOff += sectionSize + 1;

		// Put the bytes into the section vector
		sectionListings[sectionID] = out;	
	}

	delete[] size;

	std::cout << "Done sectioning file." << std::endl << std::endl;
	sectionReport();
	std::cout << std::endl;
}
