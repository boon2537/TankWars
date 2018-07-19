#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/StringUtils.hpp"

int ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, int defaultValue)
{
	int output = defaultValue;
	if (element.FindAttribute(attributeName) != 0) //TODO: push a warning if we cannot find a value!!
		output = atoi(element.Attribute(attributeName));
	return output;
}

char ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, char defaultValue)
{
	char output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output = (char) atoi(element.Attribute(attributeName));
	return output;
}

bool ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, bool defaultValue)
{
	bool output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
	{	
		const char* value = element.Attribute(attributeName);
		if (strcmp(value, "true") == 0)
			output = true;
		else if (strcmp(value, "false") == 0)
			output = false;
	}
	return output;
}

float ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, float defaultValue)
{
	float output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output = (float) atof(element.Attribute(attributeName));
	return output;
}

Rgba ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Rgba & defaultValue)
{
	Rgba output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

Vector2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Vector2 & defaultValue)
{
	Vector2 output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

Vector3 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Vector3 & defaultValue)
{
	Vector3 output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

Vector4 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Vector4 & defaultValue)
{
	Vector4 output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

IntRange ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const IntRange & defaultValue)
{
	IntRange output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

FloatRange ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const FloatRange & defaultValue)
{
	FloatRange output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

IntVector2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const IntVector2 & defaultValue)
{
	IntVector2 output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

AABB2 ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const AABB2 & defaultValue)
{
	AABB2 output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output.SetFromText(element.Attribute(attributeName));
	return output;
}

Ints ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const Ints & defaultValue)
{
	Ints output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
	{
		Strings splitted = Split(element.Attribute(attributeName), ',');
		for (unsigned int i = 0; i < splitted.size(); i++)
			output.push_back(atoi(splitted[i].c_str()));
	}
	return output;
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const std::string & defaultValue)
{
	std::string output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output = element.Attribute(attributeName);
	return output;
}

std::string ParseXmlAttribute(const tinyxml2::XMLElement & element, const char * attributeName, const char * defaultValue)
{
	if (defaultValue == nullptr)
		defaultValue = "";

	std::string output = defaultValue;
	if (element.FindAttribute(attributeName) != 0)
		output = element.Attribute(attributeName);
	return output;
}
