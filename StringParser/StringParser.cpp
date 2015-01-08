// StringParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <string>
#include <unordered_map>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Kussba legyé
#pragma warning (disable: 4804)

#define MAX_LANG_TEXT_NAME			40

std::unordered_map<std::string, std::string> lang;

std::string RemoveHexColor(const char *szMsg)
{
	char szNonColorEmbeddedMsg[257];
	int iNonColorEmbeddedMsgLen = 0;

	for (size_t pos = 0; pos < strlen(szMsg); pos++)
	{
		if (szMsg[pos] == '{')
		{
			for (size_t i = pos; i != strlen(szMsg); i++)
			{
				if (szMsg[i] == '}')
				{
					pos += (i - pos) + 1;
					break;
				}
			}
		}

		szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = szMsg[pos];
		iNonColorEmbeddedMsgLen++;
	}
	szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = 0;

	return std::string(szNonColorEmbeddedMsg);
}

/*----------------------------------------------------------*/

void DeleteEkezet(std::string &str)
{
	for (size_t i = 0; i != str.length(); i++)
	{
		switch (str[i])
		{
		case 'á': { str[i] = 'a'; break; } 
		case 'é': { str[i] = 'e'; break; }
		case 'ö': 
		case 'ó':
		case 'õ':
			{ str[i] = 'o'; break; } 
		case 'ü':
		case 'ú':
		case 'û': 
			{ str[i] = 'u'; break; }
		case 'í': { str[i] = 'i';  break; }
		case 'Á': { str[i] = 'A'; break; }
		case 'É': { str[i] = 'E'; break; }
		case 'Ö': 
		case 'Ó':
		case 'Õ': 
			{ str[i] = 'O'; break; }
		case 'Ü': 
		case 'Û': 
		case 'Ú': { str[i] = 'U'; break; }
		case 'Í': { str[i] = 'I'; break; }
		}
	}
}

/*----------------------------------------------------------*/

std::string GetLangTextName(std::string str)
{
	std::string msg = RemoveHexColor(str.c_str());
	DeleteEkezet(msg);

	msg.insert(0, "MSG_");
	std::string ret;
	bool bSkip = false;

	for (size_t i = 0; i < msg.length() && i != MAX_LANG_TEXT_NAME; ++i)
	{
		// Ha a bskip bevan kapcsolva, továbbmegyünk
		if (bSkip)
		{
			bSkip = false;
			continue;
		}

		msg[i] = toupper(msg[i]);
		if (msg[i] == '%')
		{
			bSkip = true;
			continue;
		}

		if (msg[i] == ' ') msg[i] = '_';

		if ((msg[i] >= 'A' && msg[i] <= 'Z') || (msg[i] >= '0' && msg[i] <= '9') || msg[i] == '_')
		{
			ret.push_back(msg[i]);
		}
	}
	
	size_t len = ret.length();
	if (len > 30)
	{
		for (size_t i = 30; i != len; i++)
		{
			if (ret[i] == '_')
			{
				ret.erase(i, len - i);
				break;
			}
		}
	}

	printf("ret: %s\n", ret.c_str());
	return ret;
}

/*----------------------------------------------------------*/

int CheckDuplicate(std::string &msgname, std::string &msg)
{
	for (auto &i : lang)
	{
		// Ha az üzenet egyenlõ
		if (i.first == msgname)
		{
			if (i.second == msg)
			{
				return 1; // Duplán van, továbbmegyünk és kihagyuk a gecibe
			}
			else
			{
				int count = 1;
				char asd[128];
				char output[128];
				sprintf(output, msgname.c_str());
				do
				{
					
					sprintf(asd, "%s_%d", output, count);
					count++;
				}
				while (lang.find(asd) != lang.end());

				msgname = asd;
				return 0;
			}
		}
	}
	return 0;
}

/*----------------------------------------------------------*/

int _tmain(int argc, _TCHAR* argv[])
{
	// printf() magyar karakterek javítása
	setlocale(LC_ALL, "");

	std::ifstream input("yourmodename.pwn");
	std::ofstream output("texts.txt");

	std::string it;
	std::string msg;
	std::string langtextname;
	DWORD count = 0, tick = GetTickCount();

	while (std::getline(input, it))
	{
		size_t pos = it.find("SendClientMessage");
		size_t end;
		if (pos != std::string::npos)
		{
			if ((pos = it.find("\"", pos)) != std::string::npos)
			{
				if ((end = it.find_last_of("\"")) != std::string::npos)
				{
					size_t len = end - pos;
					msg = it.substr(pos + 1, len - 1);
					//printf("%s", msg.c_str());
					
					// MSG_%s lekérése
					langtextname = GetLangTextName(msg);
					
					if(!CheckDuplicate(langtextname, msg))
					{
						lang.insert(std::make_pair(langtextname, msg));

						output << langtextname << " = " << "\"" << msg << "\"" << std::endl;
						count++;
					}
				}
			}
		}
	}

	input.close();
	output.close();

	printf("\nFinish:\n%d messages exported in %d ms\n", count, GetTickCount() - tick);
	while (1) {}

	return 0;
}
