#include <iostream>
#include <string.h>
#include <fstream>
#include <codecvt>
#include "../glb/globalVars.h"
#include "../export/saveSpecs.h"
#include "../core/SystemInfo.h"
#include "../util/utility.h"

using namespace std;
const TCHAR *saveSpecs::xmlDTD = _T("<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n");
const TCHAR *saveSpecs::uniformComment = _T("Generated by -SystemInfo- on ");
const TCHAR *saveSpecs::htmlCommentStart = _T("<!-- ");
const TCHAR *saveSpecs::htmlCommentEnd = _T("-->\n");
const TCHAR *saveSpecs::CSSCommentStart = _T("/*");
const TCHAR *saveSpecs::CSSCommentEnd = _T("*/\n");
saveSpecs::saveSpecs() {}

//to do add file write exceptions
bool saveSpecs::save(WORD command,
					 RESULT_STRUCT *res,
					 HWND hwnd,
					 SystemInfo *localMachine) {

	switch (command) {
		case ID_EXPORT_XML: {
			res->result = saveAsXML(hwnd, localMachine, res);
			break;
		}
		case ID_EXPORT_TXT: {
			res->result = saveAsText(hwnd, localMachine, res);
			break;
		}
		case ID_EXPORT_HTML: {
			res->result = saveAsHTML(hwnd, localMachine, res);
			break;
		}
	}
	return res->result;
}

bool saveSpecs::saveAsHTML(HWND hwnd, SystemInfo *info, RESULT_STRUCT *resultStruct) {
	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));

	if (openFileDiag(hwnd, FILE_EXTENSION::HTML, fullSavePath, 1) != ACTION::CANCELED_OUT) {
		std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>); //this line is necessary to output non-ascii text
		wofstream htmlOutFile;
		htmlOutFile.open(fullSavePath, wofstream::out);
		htmlOutFile.imbue(loc);
		TCHAR commentBuff[256] = { 0 };
		getCurrentDateTimeVerbose(commentBuff);
		htmlOutFile << saveSpecs::htmlCommentStart;
		htmlOutFile << saveSpecs::uniformComment;
		htmlOutFile << commentBuff;
		htmlOutFile << saveSpecs::htmlCommentEnd;
		htmlOutFile << saveSpecs::htmlStartPoint;
		htmlOutFile << L"<div id=\"hardwareinfo\">\n";
		for (int x = 0; x < totalItemsCount - 1; x++) { //excluding snapshot
			htmlOutFile << _T("\t<div class=\"item\">\n");
			htmlOutFile << _T("\t\t<div class=\"header\">");
			htmlOutFile << itemStrings[x].c_str();
			htmlOutFile << _T("\t\t</div>\n");
			htmlOutFile << _T("\t\t<div class=\"info\">");
			writeToFile(htmlOutFile, info, x, WRITE_OUT_TYPE::FILE_HTML);
			htmlOutFile << L"\t</div>\n</div>\n";
		}
		htmlOutFile << L"</div>\n</body>\n</html>\n";

		htmlOutFile.close();
		resultStruct->src.assign(fullSavePath);

		return fileIOCheck(htmlOutFile);
	} else {
		return false;
	}
}

bool saveSpecs::saveAsXML(HWND hwnd, SystemInfo *info, RESULT_STRUCT *resultStruct) {
	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));

	if (openFileDiag(hwnd, FILE_EXTENSION::XML, fullSavePath, 1) != ACTION::CANCELED_OUT) {

		std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>); //this line is necessary to output non-ascii text
		wofstream xmlOutFile;
		xmlOutFile.open(fullSavePath, wofstream::out);
		xmlOutFile.imbue(loc);
		TCHAR commentBuff[256] = { 0 };
		getCurrentDateTimeVerbose(commentBuff);
		xmlOutFile << saveSpecs::xmlDTD;
		xmlOutFile << saveSpecs::htmlCommentStart;
		xmlOutFile << saveSpecs::uniformComment;
		xmlOutFile << commentBuff;
		xmlOutFile << saveSpecs::htmlCommentEnd;

		xmlOutFile << L"<hardwareinfo>\n";
		for (int x = 0; x < totalItemsCount; x++) {
			xmlOutFile << _T("\t<item type=\"");
			xmlOutFile << itemStrings[x].c_str();
			xmlOutFile << _T("\">\n\t\t");

			writeToFile(xmlOutFile, info, x, WRITE_OUT_TYPE::FILE_XML);

			xmlOutFile << L"\t</item>\n";
		}
		xmlOutFile << L"</hardwareinfo>\n";
		xmlOutFile.close();
		resultStruct->src.assign(fullSavePath);
		return fileIOCheck(xmlOutFile);
	} else {
		return false;
	}
}

void importAsXML(HWND hwnd) {

	TCHAR fullOpenPath[256];
	ZeroMemory(&fullOpenPath, sizeof(fullOpenPath));

	if (openFileDiag(hwnd, FILE_EXTENSION::XML, fullOpenPath, 0) != ACTION::CANCELED_OUT) {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));


		// Start the child process. 
		TCHAR fullExecLine[256] = { 0 };
		_tcscpy(fullExecLine, execName);
		_tcscat(fullExecLine, fullOpenPath);


		DWORD err;
		DWORD res = 1;
		LPTSTR szcmdLine = _tcsdup(fullExecLine);
		if (!CreateProcess(NULL,   // No module name (use command line)
						   szcmdLine,        // Command line
						   NULL,           // Process handle not inheritable
						   NULL,           // Thread handle not inheritable
						   FALSE,          // Set handle inheritance to FALSE
						   0,              // No creation flags
						   NULL,           // Use parent's environment block
						   NULL,           // Use parent's starting directory 
						   &si,            // Pointer to STARTUPINFO structure
						   &pi)) {
			MessageBox(NULL, L"Unable to create process", L"Something happened", MB_OK | MB_ICONERROR);
		}
	}
}

bool saveSpecs::saveAsText(HWND hwnd, SystemInfo *info, RESULT_STRUCT *resultStruct) {
	TCHAR fullSavePath[256];

	ZeroMemory(&fullSavePath, sizeof(fullSavePath));

	if (openFileDiag(hwnd, FILE_EXTENSION::TXT, fullSavePath, 1) != ACTION::CANCELED_OUT) {
		std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>); //this line is necessary to output non-ascii text
		wofstream txtOutFile;
		txtOutFile.open(fullSavePath, wofstream::out);
		txtOutFile.imbue(loc);
		TCHAR commentBuff[256] = { 0 };
		getCurrentDateTimeVerbose(commentBuff);
		txtOutFile << saveSpecs::uniformComment;
		txtOutFile << commentBuff;
		txtOutFile << endl << endl;
		for (int x = 0; x < totalItemsCount - 1; x++) { //excluding snapshot
			txtOutFile << itemStrings[x].c_str();
			txtOutFile << _T(":\n");
			txtOutFile << _T("\t");
			writeToFile(txtOutFile, info, x, WRITE_OUT_TYPE::FILE_TXT);
			txtOutFile << endl;
		}
		txtOutFile.close();
		resultStruct->src.assign(fullSavePath);
		return fileIOCheck(txtOutFile);
	} else {
		return false;
	}

}

saveSpecs::~saveSpecs() {}
const TCHAR *saveSpecs::htmlStartPoint = _T("\
	<!DOCTYPE html>\n\
	<html>\n\
	<head>\
	<meta charset=\"utf-8\"/>\n\
	<style>\n\
	*\n\
\t{\n\
	\tfont-family: Helvetica;\n\
\t}\n\
\tdiv#hardwareinfo\n\
\t{\n\
\t\tmargin: auto;\n\
\t\twidth: 50%;\n\
\t}\n\
\t.item\n\
\t{\
\t\tborder: 1px solid #ccc;\n\
\t\tpadding: 0px;\n\
\t\tmargin-top: 10px;\n\
\t}\n\
\t.item.author{\n\
\t\ttext - align: center;\
\t\tpadding: 10px;\
\t}\
\t.header{\n\
\t\tbackground-color: purple;\n\
\t\tcolor: #fff;\n\
\t\tborder-bottom: 1px solid #ccc;\n\
\t\tpadding-left: 10px;\n\
\t}\n\
\t.info{\n\
\t\tpadding-left: 10px;\n\
\t}\n\
</style>\n\
<title>Hardware Information</title>\n\
</head>\n\
<body>\n");