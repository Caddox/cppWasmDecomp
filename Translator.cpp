#include "Translator.h"

Translator::Translator(Mediator& m, const std::string& filename)
{
	setMediator(m);
	setTranslationScheme(filename);
}

void Translator::setMediator(Mediator& m)
{
	intermediateRepr = &m;
}

void Translator::setTranslationScheme(const std::string& filename)
{
	translationFilename = filename;
	loadTranslationScheme();
	validateTranslationScheme();
}

/// <summary>This function will take in the toml file and ensure that it
/// has all of the items present that it needs to translate the </summary>
bool Translator::validateTranslationScheme()
{
	// Start by asserting the items that must be present in the meta section:
	//	Required: type, boiler
	//	Available: implicit_return, args
	auto metaTable = translationScheme->get_table("meta");
	const auto type = metaTable->get_as<std::string>("type").value_or("ERR");
	const auto boiler = metaTable->get_as<std::string>("boiler").value_or("ERR");
	
	assert(type != "ERR", "Boiler section not correctly formatted.");
	assert(boiler != "ERR", "Boiler section not correctly formatted.");

	symbolMap["type"] = type;
	symbolMap["boiler"] = boiler;
	symbolMap["implicit_return"] = metaTable->get_as<bool>("implicit_return").value_or(false);
	symbolMap["args"] = metaTable->get_as<std::string>("args").value_or("");

	// Next, validate the symbols
	auto symbolTable = translationScheme->get_table("symbols");
	auto add = symbolTable->get_as<std::string>("add").value_or("");
	auto sub = symbolTable->get_as<std::string>("sub").value_or("");
	auto multiply = symbolTable->get_as<std::string>("multiply").value_or("");
	auto divide = symbolTable->get_as<std::string>("divide").value_or("");
	auto bang_not = symbolTable->get_as<std::string>("not").value_or("");
	auto set = symbolTable->get_as<std::string>("set").value_or("");
	auto index_open = symbolTable->get_as<std::string>("index_open").value_or("");
	auto index_close = symbolTable->get_as<std::string>("index_close").value_or("");
	auto paren_open = symbolTable->get_as<std::string>("paren_open").value_or("");
	auto paren_close = symbolTable->get_as<std::string>("paren_close").value_or("");

    assert(add != "", "add is not correctly formatted in the translation toml.");
    assert(sub != "", "sub is not correctly formatted in the translation toml.");
    assert(multiply != "", "multiply is not correctly formatted in the translation toml.");
    assert(divide != "", "divide is not correctly formatted in the translation toml.");
    assert(bang_not != "", "bang_not is not correctly formatted in the translation toml.");
    assert(set != "", "set is not correctly formatted in the translation toml.");
    assert(index_open != "", "index_open is not correctly formatted in the translation toml.");
    assert(index_close != "", "index_close is not correctly formatted in the translation toml.");
    assert(paren_open != "", "paren_open is not correctly formatted in the translation toml.");
    assert(paren_close != "", "paren_close is not correctly formatted in the translation toml.");

    symbolMap["add"] = add;
    symbolMap["sub"] = sub;
    symbolMap["multiply"] = multiply;
    symbolMap["divide"] = divide;
    symbolMap["bang_not"] = bang_not;
    symbolMap["set"] = set;
    symbolMap["index_open"] = index_open;
    symbolMap["index_close"] = index_close;
    symbolMap["paren_open"] = paren_open;
    symbolMap["paren_close"] = paren_close;

	return true;
}

void Translator::loadTranslationScheme()
{
	translationScheme = cpptoml::parse_file(translationFilename);
}
