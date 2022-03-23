#pragma once

#include <fstream>

class StreamHelper
{
public:
	static char ReadByte(std::ifstream& file)
	{
		char byte;
		file.read(&byte, 1);
		return byte;
	}

	static int ReadInt(std::ifstream& file)
	{
		char buf[4];
		file.read(buf, 4);
		return *(reinterpret_cast<int*>(buf));
	}

	static float ReadFloat(std::ifstream& file)
	{
		char buf[4];
		file.read(buf, 4);
		return *(reinterpret_cast<float*>(buf));
	}

	static void ReadString(std::ifstream& file, char* str)
	{
		int count = ReadInt(file);
		file.read(str, count);
		str[count] = 0;
	}

	//static std::string ReadString(std::ifstream& file)
	//{
	//	int count = ReadInt(file);
	//	char* str = "\0";
	//	file.read(str, count);
	//	std::string retString = str;
	//	return retString;
	//}
};