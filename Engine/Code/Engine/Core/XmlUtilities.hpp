#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <string>

int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue );
Rgba ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba& defaultValue );
Vector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector2& defaultValue );
Vector3 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector3& defaultValue );
Vector4 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector4& defaultValue );
IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultValue );
IntVector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue );
AABB2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const AABB2& defaultValue );
Ints ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Ints& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const char* defaultValue=nullptr );