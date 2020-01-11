#ifndef MEDIATOR_H
#define MEDIATOR_H
#include "Sectioner.h"
#include "Function.h"
#include <sstream>
#include <map>


///	<summary>This class will take the sections and turn them into a list of 
///	functions represented in the abstract. Basically, this is data
///	manipulation.
///	</summary>
class Mediator
{
public:
	Mediator(Sectioner* worker);

	int getIdxInputSize(int index);
	std::map< int, std::pair< std::vector<dataDef>, dataDef> > funcSizeTable;

private:
	void extractFunctions();
	void populateFunctions(std::vector<std::pair<std::vector<dataDef>, dataDef>> pairs);
	std::map<int, std::string> extractImports() const;
	void extractSignatures();
	void extractNames();

	int numberOfFuncs = 0;
	Sectioner * worker;
	std::map<int, Function> funcs;
};

#endif // MEDIATOR_H
