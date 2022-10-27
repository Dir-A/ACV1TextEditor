#include "ACV1TextEditor.h"
using namespace std;

bool DumpText(wstring& strFileNmae, unsigned int uCodePage, bool isDoubleLine)
{
	unsigned int lineCount = 1;
	auto cvtUTF8 = locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode(consume_header | generate_header | little_endian)>);

	ifstream scriptFile(strFileNmae);
	wofstream transFile(strFileNmae + L".txt");
	transFile.imbue(cvtUTF8);
	if (transFile.is_open() && scriptFile.is_open())
	{
		for (std::string mLine; getline(scriptFile, mLine); lineCount++)
		{
			if (mLine.size() <= 0)
			{
				continue;
			}

			if ((unsigned char)mLine[0] >= (unsigned char)0x7B)
			{
				wstring wLine = StrToWStr(mLine, uCodePage);
				transFile << L"Count:" << setw(0x8) << setfill(L'0') << lineCount << endl;
				transFile << L"Raw:" << wLine << endl;
				if (isDoubleLine)
				{
					transFile << L"Tra:" << wLine << endl << endl;
				}
				else
				{
					transFile << L"Tra:" << endl << endl;
				}
			}
		}
		scriptFile.close();
		transFile.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool InsetText(wstring& strFileName, unsigned int uCodePage)
{
	unsigned int position = 0;
	vector<string> scriptLineList;
	vector<wstring> transLineList;
	vector<unsigned int> transPositionList;
	auto cvtUTF8 = locale(locale::empty(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode(consume_header | generate_header | little_endian)>);

	//Get Trans Text
	wifstream transFile(strFileName + L".txt");
	transFile.imbue(cvtUTF8);
	if (transFile.is_open())
	{
		for (std::wstring wLine; getline(transFile, wLine);)
		{
			if (wLine.find(L"Count:") != string::npos)
			{
				swscanf_s(wLine.c_str(), L"Count:%d", &position);
				transPositionList.push_back(position);
				continue;
			}

			if (wLine.find(L"Tra:") != string::npos)
			{
				wLine = wLine.substr(0x4);
				if (!wLine.empty())
				{
					transLineList.push_back(wLine);
				}
			}

		}
		transFile.close();
	}
	else
	{
		return false;
	}


	if (transPositionList.size() != transLineList.size())
	{
		return false;
	}


	//Read Script By PerLine
	ifstream scriptFile(strFileName);
	if (scriptFile.is_open())
	{
		for (std::string line; getline(scriptFile, line);)
		{
			scriptLineList.push_back(line);
		}
		scriptFile.close();
	}
	else
	{
		return false;
	}


	//Out Script As New File
	ofstream newScriptFile(strFileName + L".new", ios::binary);
	if (newScriptFile.is_open())
	{
		//Replace Trans Text
		for (size_t i = 0; i < transPositionList.size(); i++)
		{
			scriptLineList[transPositionList[i] - 1] = WStrToStr(transLineList[i], uCodePage);
		}
		//Write Back All Line
		for (auto& line : scriptLineList)
		{
			newScriptFile << line << '\n';
		}
		newScriptFile.close();
	}
	else
	{
		return false;
	}

	return true;
}