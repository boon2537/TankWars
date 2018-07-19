#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

void Blackboard::PopulateFromXmlElementAttributes(const tinyxml2::XMLElement & element)
{
	const tinyxml2::XMLAttribute* attributeToUse = element.FirstAttribute();
	while (attributeToUse != nullptr)
	{
		m_keyValuePairs.insert(std::pair<std::string, std::string>(attributeToUse->Name(), attributeToUse->Value()));
		attributeToUse = attributeToUse->Next();
	}	
}

void Blackboard::SetValue(const std::string & keyName, const std::string & newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

bool Blackboard::GetValue(const std::string & keyName, bool defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		if (search->second == "true")
			return true;
		else if (search->second == "false")
			return false;
		else
		{
			ERROR_RECOVERABLE("Bool value wrongly formatted");
			return defaultValue;
		}
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

int Blackboard::GetValue(const std::string & keyName, int defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		return atoi(search->second.c_str());
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

float Blackboard::GetValue(const std::string & keyName, float defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		return static_cast<float>(atof(search->second.c_str()));
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

std::string Blackboard::GetValue(const std::string & keyName, std::string defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		return search->second;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

std::string Blackboard::GetValue(const std::string & keyName, const char * defaultValue) const
{
	return GetValue(keyName, std::string(defaultValue));
}

Rgba Blackboard::GetValue(const std::string & keyName, const Rgba & defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		Rgba output = Rgba::white;
		output.SetFromText(search->second.c_str());
		return output;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

Vector2 Blackboard::GetValue(const std::string & keyName, const Vector2 & defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		Vector2 output = Vector2(0, 0);
		output.SetFromText(search->second.c_str());
		return output;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

IntVector2 Blackboard::GetValue(const std::string & keyName, const IntVector2 & defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		IntVector2 output = IntVector2(0, 0);
		output.SetFromText(search->second.c_str());
		return output;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

FloatRange Blackboard::GetValue(const std::string & keyName, const FloatRange & defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		FloatRange output = FloatRange(0);
		output.SetFromText(search->second.c_str());
		return output;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}

IntRange Blackboard::GetValue(const std::string & keyName, const IntRange & defaultValue) const
{
	std::map<std::string, std::string>::const_iterator search = m_keyValuePairs.find(keyName);
	if (search != m_keyValuePairs.end())
	{
		IntRange output = IntRange(0);
		output.SetFromText(search->second.c_str());
		return output;
	}

	ERROR_RECOVERABLE("Key: " + keyName + " not found");
	return defaultValue;
}
