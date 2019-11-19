#include "pch.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Usage: copytext text");
		exit(1);
	}

	char* pText = argv[1];

	GLOBALHANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, 1024);
	int* pGlobal = (int*)GlobalLock(hGlobal);
	strcpy_s((char *)pGlobal, 1024, pText);
	GlobalUnlock(hGlobal);

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

	return 0;
}