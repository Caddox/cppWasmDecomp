#ifndef SECTIONER_H
#define SECTIONER_H
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "maths.h"

typedef unsigned char byte;

class Sectioner
{
public:
	Sectioner(std::string& filename);
	void readFromFile(std::string& filename);
	std::vector<byte> getSection(int id);

private:
	std::vector<std::vector<byte>> sectionListings;
	//std::vector<byte> rawByteString;
	std::string filename;
	int fileSize;

	bool verifyFile();
	void readNBytesFromFile(int n_bytes, int fileOffset, char* out) const;
	std::vector<byte> readNBytesFromFile(int n_bytes, int fileOffset) const;
	void sectionReport();
	void sectionBytes();
};

#endif // SECTIONER_H
