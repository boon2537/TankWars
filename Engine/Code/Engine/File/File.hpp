#pragma once

#include <string>

void* FileReadToNewBuffer(const char* filename);
bool WriteStringToFile(const char* filename, std::string data);
