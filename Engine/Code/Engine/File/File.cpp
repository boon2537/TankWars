#include "Engine/File/File.hpp"
#include <stdio.h>
#include <stdlib.h>

// writing on a text file
#include <iostream>
#include <fstream>

void* FileReadToNewBuffer(const char* filename)
{
	FILE *fp = nullptr;
	fopen_s( &fp, filename, "r" );

	if (fp == nullptr) {
		return nullptr;
	}

	size_t size = 0U;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	unsigned char *buffer = (unsigned char*) malloc(size + 1U); // space for NULL

	size_t read = fread( buffer, 1, size, fp );
	fclose(fp);

	buffer[read] = NULL;

	return buffer; 
}

bool WriteStringToFile(const char* filename, std::string data)
{
	std::string fullFileName = std::string(filename) + ".txt";
	std::ofstream myfile(fullFileName);
	if (myfile.is_open())
	{
		myfile << data;
		myfile.close();

		return true;
	}
	else
	{
		return false;
	}
}