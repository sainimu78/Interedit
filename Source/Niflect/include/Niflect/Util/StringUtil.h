#pragma once
#include "Niflect/NiflectBase.h"
#include <stdarg.h>
#include <fstream>
#include <algorithm>

namespace NiflectUtil
{
	static Niflect::CString FormatString(const char* format, ...)
	{
		char szContent[10240] = "";
		va_list va_alist;
		va_start(va_alist, format);
		vsnprintf(szContent, 10240, format, va_alist);
		va_end(va_alist);
		return szContent;
	}
	static std::string FormatStdString(const char* format, ...)
	{
		char szContent[10240] = "";
		va_list va_alist;
		va_start(va_alist, format);
		vsnprintf(szContent, 10240, format, va_alist);
		va_end(va_alist);
		return szContent;
	}
	static float StringToFloat(const Niflect::CString& str)
	{
        return static_cast<float>(atof(str.c_str()));
	}
	static bool StartsWith(const Niflect::CString& fullString, const Niflect::CString& start)
	{
		if (fullString.length() >= start.length())
		{
			return (0 == fullString.compare(0, start.length(), start));
		}
		return false;
	}
	static bool EndsWith(const Niflect::CString& fullString, const char& ending)
	{
		if (fullString.length() >= 1)
			return fullString.back() == ending;
		return false;
	}
	static bool EndsWith(const Niflect::CString& fullString, const Niflect::CString& ending)
	{
		if(fullString.length() >= ending.length())
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		return false;
	}
	static Niflect::CString ReplaceFilePathExt(const Niflect::CString& filePath, const Niflect::CString& a, const Niflect::CString& b)
	{
		auto pos = filePath.find_last_of(a);
		if (pos != std::string::npos)
		{
			auto result = filePath.substr(0, pos - 1);
			result += b;
			return result;
		}
		return filePath;
	}
	static Niflect::CString RemoveFileExt(const Niflect::CString& filePath, const Niflect::CString& ext = ".")
	{
		auto pos = filePath.find_last_of(ext);
		if (pos != std::string::npos)
		{
			auto result = filePath.substr(0, pos);
			return result;
		}
		return filePath;
	}
	static Niflect::CString ConcatPath(const Niflect::CString& a, const Niflect::CString& b)
	{
		constexpr const int JoinABWithSlash = 0;
		constexpr const int ATrailingSlash = 1;
		constexpr const int BLeadingSlash = 2;
		constexpr const int BothHaveSlash = 3;
		constexpr const int BothEmpty = 4;

		auto mode = JoinABWithSlash;
		if (!a.empty() && !b.empty())
		{
			if (a.back() == '/')
				mode = ATrailingSlash;
			if (b[0] == '/')
			{
				if (mode != ATrailingSlash)
					mode = BLeadingSlash;
				else
					mode = BothHaveSlash;
			}
		}
		else if (a.empty() && !b.empty())
		{
			return b;
		}
		else if (!a.empty() && b.empty())
		{
			return a;
		}
		else
		{
			mode = BothEmpty;
		}

		Niflect::CString result;
		switch (mode)
		{
		case JoinABWithSlash:
		{
			result = a + '/' + b;
			break;
		}
		case ATrailingSlash:
		case BLeadingSlash:
		{
			result = a + b;
			break;
		}
		case BothHaveSlash:
		{
			result = b.substr(1, b.length() - 1);
			result = a + result;
			break;
		}
		default:
			break;
		}
		return result;
	}
	static bool GetParentDirPathSafe(const Niflect::CString& fileOrDirPath, Niflect::CString& parentDirPath)
	{
		auto posSlash = fileOrDirPath.find_last_of('/');
		if (posSlash != std::string::npos)
		{
			parentDirPath = fileOrDirPath.substr(0, posSlash);
			return true;
		}
		return false;
	}
	static Niflect::CString GetParentDirPath(const Niflect::CString& fileOrDirPath)
	{
		Niflect::CString parentDirPath;
		if (GetParentDirPathSafe(fileOrDirPath, parentDirPath))
			return parentDirPath;
		return fileOrDirPath;
	}
	static Niflect::CString GetFileName(const Niflect::CString& filePath)
	{
		auto posSlash = filePath.find_last_of('/');
		if (posSlash != std::string::npos)
			return filePath.substr(posSlash + 1, filePath.length() - posSlash);
		return filePath;
	}
	static Niflect::CString GetDirName(const Niflect::CString& dirPath)
	{
		return GetFileName(dirPath);
	}
	static void WriteStringToFile(const Niflect::CString& data, const Niflect::CString& filePath)
	{
		std::ofstream ofs(filePath.c_str(), std::ios::binary);
		ASSERT(ofs.is_open());
		ofs << data;
		ofs.close();
	}
	static Niflect::CString ReadStringFromFile(const Niflect::CString& filePath)
	{
		std::ifstream ifs(filePath.c_str(), std::ios::binary);
		char buffer[1024];
		Niflect::CString data;
		while (ifs.read(buffer, sizeof(buffer)))
			data.append(buffer, ifs.gcount());
		data.append(buffer, ifs.gcount());
		return data;
	}
	static Niflect::CString ConvertToUpperCase(Niflect::CString str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
		return str;
	}
	static bool CompareLessHumanReadable(const Niflect::CString& a, const Niflect::CString& b)
	{
		Niflect::CString lowerA = a;
		Niflect::CString lowerB = b;
		std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
		std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
		return lowerA < lowerB;
	}
	static Niflect::TArrayNif<Niflect::CString> Split(const Niflect::CString& str, char delimiter) {
		Niflect::TArrayNif<Niflect::CString> tokens;  // 用于存储分割后的子字符串
		Niflect::CString token;                // 临时存储每个子字符串
		for (char ch : str) {             // 遍历输入字符串中的每个字符
			if (ch == delimiter) {        // 如果当前字符是分隔符
				if (!token.empty()) {     // 并且临时子字符串不为空
					tokens.push_back(token); // 将临时子字符串添加到结果向量中
					token.clear();        // 清空临时子字符串
				}
			}
			else {
				token += ch;              // 如果当前字符不是分隔符，则将其添加到临时子字符串中
			}
		}
		if (!token.empty()) {             // 如果最后一个临时子字符串不为空
			tokens.push_back(token);      // 将其添加到结果向量中
		}
		return tokens;                    // 返回分割后的子字符串向量
	}
	static Niflect::TArrayNif<Niflect::CString> Split(const Niflect::CString& str, const Niflect::CString& delimiter) {
		Niflect::TArrayNif<Niflect::CString> tokens;  // 用于存储分割后的子字符串
		size_t start = 0;                 // 开始位置
		size_t end = 0;                   // 结束位置

		while ((end = str.find(delimiter, start)) != std::string::npos) {
			// 找到分隔符位置
			tokens.push_back(str.substr(start, end - start)); // 提取子字符串并添加到结果向量中
			start = end + delimiter.length();                 // 更新开始位置
		}
		tokens.push_back(str.substr(start)); // 添加最后一个子字符串

		return tokens;                       // 返回分割后的子字符串向量
	}
}