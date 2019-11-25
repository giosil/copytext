#include "pch.h"

// Functions Declarations

char* lowercase(char *text);

char* uppercase(char *text);

char* capitalize(char *text);

char* reverse(char *text);

char* quote(char *text);

char* numbers(char *text);

char* currentDate();

char* currentTime();

char* concat(char *text1, char *text2);

char* concatPath(char *text1, char *text2);

char* currentWorkDir();

bool showHelp(char *text);

// Entry point

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: copytext [-opt] text\n");
    printf("Help:  copytext -h\n");
    exit(1);
  }

  char* opt0 = argc > 2 ? argv[1] : 0;
  char* text = argc > 2 ? argv[2] : argv[1];

  if (showHelp(text)) {
    exit(1);
  }

  if (opt0) {
    showHelp(opt0);

    int cmp = strcmp(opt0, "-l");
    if (cmp == 0) {
      text = lowercase(text);
    }
    cmp = strcmp(opt0, "-u");
    if (cmp == 0) {
      text = uppercase(text);
    }
    cmp = strcmp(opt0, "-c");
    if (cmp == 0) {
      text = capitalize(text);
    }
    cmp = strcmp(opt0, "-r");
    if (cmp == 0) {
      text = reverse(text);
    }
    cmp = strcmp(opt0, "-q");
    if (cmp == 0) {
      text = quote(text);
    }
    cmp = strcmp(opt0, "-n");
    if (cmp == 0) {
      text = numbers(text);
    }
    cmp = strcmp(opt0, "-e");
    if (cmp == 0) {
      text = getenv(text);
    }
    cmp = strcmp(opt0, "-d");
    if (cmp == 0) {
      text = concat(text, currentDate());
    }
    cmp = strcmp(opt0, "-t");
    if (cmp == 0) {
      text = concat(text, currentTime());
    }
    cmp = strcmp(opt0, "-w");
    if (cmp == 0) {
      text = concatPath(currentWorkDir(), text);
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

// Functions Implementation

char* lowercase(char *text)
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

char* uppercase(char *text)
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

char* reverse(char *text)
{
  size_t len = strlen(text);
  if (len == 0) return text;

  char *buffer = new char[len + 1];
  char *result = buffer;
  for (int i = 0; i < len; i++) {
    *buffer++ = text[len - 1 - i];
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
    if (text[i] == '\'') {
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

char* numbers(char *text)
{
  size_t len = strlen(text);
  if (len == 0) return text;

  int count = 0;
  for (int i = 0; i < len; i++) {
    if (text[i] >= 48 && text[i] <= 57) {
      count++;
    }
  }

  char *buffer = new char[count + 1];
  char *result = buffer;
  while (*text != '\0') {
    if (*text >= 48 && *text <= 57) {
      *buffer++ = *text;
    }
    text++;
  }
  *buffer = '\0';

  return result;
}

char* currentDate()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iDate = (1900 + tm.tm_year) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;

  char *buffer = new char[9];
  char* result = _itoa(iDate, buffer, 10);
  return result;
}

char* currentTime()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iHHMM = tm.tm_hour * 100 + tm.tm_min;

  char* buffer = new char[9];
  char* result = _itoa(iHHMM, buffer, 10);
  return result;
}

char* concat(char *text1, char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  char *buffer = new char[len1 + len2 + 1];
  strcpy(buffer, text1);
  strcat(buffer, text2);

  return buffer;
}

char* concatPath(char *text1, char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  char *buffer = new char[len1 + len2 + 2];
  strcpy(buffer, text1);
  strcat(buffer, "\\");
  strcat(buffer, text2);

  return buffer;
}

char* currentWorkDir()
{
  char *buffer = new char[FILENAME_MAX];

  _getcwd(buffer, FILENAME_MAX);

  return buffer;
}

bool showHelp(char *text)
{
  int cmp = strcmp(text, "-h");
  if (cmp != 0) return false;

  const char* PRG_NAME = "copytext";
  const int   PRG_VER_MAJ = 1;
  const int   PRG_VER_MIN = 0;

  printf("%s ver. %d.%d\n\n", PRG_NAME, PRG_VER_MAJ, PRG_VER_MIN);

  printf("Options:\n\n");
  printf("  -h: this guide;\n");
  printf("  -l: lowercase;\n");
  printf("  -u: uppercase;\n");
  printf("  -r: reverse;\n");
  printf("  -q: quote;\n");
  printf("  -n: extract numbers;\n");
  printf("  -e: environmen variable;\n");
  printf("  -d: append date;\n");
  printf("  -t: append time;\n");
  printf("  -w: prepend current work directory.\n\n");

  printf("Example:\n\n");
  printf("> copytext -u hello\n");
  printf("  (HELLO copied in clipboard)\n\n");

  return true;
}