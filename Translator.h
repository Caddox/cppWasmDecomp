#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include "Mediator.h"
#include "cpptoml.h"

/************************************************************************************
 *  This file uses cpptoml from https://github.com/skystrife/cpptoml
 *  The license for the use of this file has been reproduced below.
 *  Thank you!
 *
 *  Copyright (c) 2014 Chase Geigle
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 *  the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***********************************************************************************/

class Translator
{
public:
	Translator(Mediator& m, const std::string& filename);
	void setMediator(Mediator& m);
	void setTranslationScheme(const std::string& filename);

private:
	Mediator* intermediateRepr = nullptr;
	bool validateTranslationScheme();
	void loadTranslationScheme();

	std::string translationFilename;
	std::shared_ptr<cpptoml::table> translationScheme;
	std::map<std::string, std::string> symbolMap;
};

#endif // TRANSLATOR_H
