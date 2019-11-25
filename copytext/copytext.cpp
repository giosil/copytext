#include "pch.h"

char* toLowerCase(char *text);
char* toUpperCase(char *text);
char* capitalize(char *text);
char* quote(char *text);

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Usage: copytext [-opt] text");
		exit(1);
	}

	char* opt0 = argc > 2 ? argv[1] : 0;
	char* text = argc > 2 ? argv[2] : argv[1];

	if (opt0) {
		int cmp = strcmp(opt0, "-l");
		if (cmp == 0) {
			text = toLowerCase(text);
		}
		cmp = strcmp(opt0, "-u");
		if (cmp == 0) {
			text = toUpperCase(text);
		}
		cmp = strcmp(opt0, "-c");
		if (cmp == 0) {
			text = capitalize(text);
		}
		cmp = strcmp(opt0, "-r");
		if (cmp == 0) {
			text = capitalize(text);
		}
		cmp = strcmp(opt0, "-q");
		if (cmp == 0) {
			text = quote(text);
		}
	}

	GLOBALHANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, 1024);
	int* pGlobal = (int*)GlobalLock(hGlobal);
	strcpy_s((char *)pGlobal, 1024, text);
	GlobalUnlock(hGlobal);

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();

	return 0;
}

char* toLowerCase(char *text)
{
	size_t len = strlen(text);
	if (len == 0) return text;

	char *buffer = new char[len + 1];
	char *result = buffer;
	while (*text != '\0') {
		*buffer++ = tolower(*text++);
	}
	*buffer = '\0';
	
	return result;
}

char* toUpperCase(char *text)
{
	size_t len = strlen(text);
	if (len == 0) return text;

	char *buffer = new char[len + 1];
	char *result = buffer;
	while (*text != '\0') {
		*buffer++ = toupper(*text++);
	}
	*buffer = '\0';

	return result;
}

char* capitalize(char *text)
{
	size_t len = strlen(text);
	if (len == 0) return text;

	int i = 0;
	char *buffer = new char[len + 1];
	char *result = buffer;
	while (*text != '\0') {
		if (i == 0) {
			*buffer++ = toupper(*text++);
		}
		else {
			*buffer++ = tolower(*text++);
		}
		i++;
	}
	*buffer = '\0';

	return result;
}

char* quote(char *text)
{
	size_t len = strlen(text);
	if (len == 0) return text;

	int count = 0;
	for (int i = 0; i < len; i++) {
		if (*text == '\'') {
			count++;
		}
	}

	char *buffer = new char[len + 1 + 2 + count];
	char *result = buffer;
	*buffer++ = '\'';
	while (*text != '\0') {
		*buffer++ = *text;
		if (*text == '\'') *buffer++ = '\'';
		text++;
	}
	*buffer++ = '\'';
	*buffer = '\0';

	return result;
}