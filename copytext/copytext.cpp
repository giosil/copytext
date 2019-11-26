#include "pch.h"

#define OPT_HELP         1
#define OPT_LOWERCASE    2
#define OPT_UPPERCASE    3
#define OPT_CAPITALIZE   4
#define OPT_REVERSE      5
#define OPT_QUOTE        6
#define OPT_NUMBERS      7
#define OPT_ENVIRONMENT  8
#define OPT_READ_FILE    9
#define OPT_SAVE_FILE   10
#define OPT_DATE        11
#define OPT_TIME        12
#define OPT_VALUE       13
#define OPT_WORK_DIR    14
#define OPT_INTERACTIVE 15

// Functions Declarations

char* lowercase(char *text);
char* uppercase(char *text);
char* capitalize(char *text);
char* reverse(char *text);
char* quote(char *text);
char* numbers(char *text);
char* concat(char *text1, char *text2);
char* concatPath(char *text1, char *text2);
char* trim(char *text);
char* substring(char *text, int beginIndex, int endIndex);
char* substring(char *text, int beginIndex);
char* removeQuotes(char *text);
char* value(char *text);

int indexOf(char *text, char c);

char* readTextFile(char *file, int max);
bool writeTextFile(const char *file, char *text);
char* readStdIn(int max);

char* currentDate();
char* currentTime();
char* currentWorkDir();

int getOption(int argc, char* argv[]);
void showHelp();

// Entry point

int main(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: copytext [-opt] text\n");
    printf("Help:  copytext -h\n");
    exit(1);
  }

  char* text = argc > 2 ? argv[2] : argv[1];

  int opt = getOption(argc, argv);
  switch (opt)
  {
    case OPT_HELP:
      showHelp();
      break;
    case OPT_LOWERCASE:
      text = lowercase(text);
      break;
    case OPT_UPPERCASE:
      text = uppercase(text);
      break;
    case OPT_CAPITALIZE:
      text = capitalize(text);
      break;
    case OPT_REVERSE:
      text = reverse(text);
      break;
    case OPT_QUOTE:
      text = quote(text);
      break;
    case OPT_NUMBERS:
      text = numbers(text);
      break;
    case OPT_ENVIRONMENT:
      text = getenv(text);
      break;
    case OPT_READ_FILE:
      text = readTextFile(text, 255);
      break;
    case OPT_SAVE_FILE:
      writeTextFile("copytext.txt", text);
      break;
    case OPT_DATE:
      text = concat(text, currentDate());
      break;
    case OPT_TIME:
      text = concat(text, currentTime());
      break;
    case OPT_VALUE:
      text = value(text);
      break;
    case OPT_WORK_DIR:
      text = concatPath(currentWorkDir(), text);
      break;
    case OPT_INTERACTIVE:
      text = readStdIn(100);
      break;
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

char* trim(char *text)
{
  size_t len = strlen(text);
  if (len == 0) return text;

  int count = 0;
  for (int i = 0; i < len; i++) {
    if (text[i] > 32) {
      count++;
    }
  }

  char *buffer = new char[count + 1];
  char *result = buffer;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
    }
    text++;
  }
  *buffer = '\0';

  return result;
}

char* substring(char *text, int beginIndex, int endIndex)
{
  if (beginIndex == endIndex) {
    return { '\0' };
  }

  char *buffer = new char[endIndex - beginIndex + 1];
  char *result = buffer;
  text += beginIndex;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
    }
    text++;
  }
  *buffer = '\0';

  return result;
}

char* substring(char *text, int beginIndex)
{
  size_t endIndex = strlen(text);

  if (beginIndex == endIndex) {
    return { '\0' };
  }

  char *buffer = new char[endIndex - beginIndex + 1];
  char *result = buffer;
  text += beginIndex;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
    }
    text++;
  }
  *buffer = '\0';

  return result;
}

char* removeQuotes(char *text)
{
  size_t len = strlen(text);
  if (len == 0) return text;

  if (text[0] != '\'' && text[0] != '"') {
    return text;
  }
  if (text[len - 1] != '\'' && text[len - 1] != '"') {
    return text;
  }
  
  int i = 0;
  char *buffer = new char[len - 2 + 1];
  char *result = buffer;
  text++;
  while (*text != '\0') {
    i++;
    if (i == len - 1) break;
    *buffer++ = *text++;
  }
  *buffer = '\0';

  return result;
}

int indexOf(char *text, char c)
{
  size_t len = strlen(text);
  if (len == 0) return -1;

  for (int i = 0; i < len; i++) {
    if (text[i] == c) return i;
  }

  return -1;
}

char* value(char *text)
{
  int idx = indexOf(text, '=');
  if (idx < 0) {
    idx = indexOf(text, ':');
  }
  if (idx < 0) {
    return text;
  }
  
  char *sub = substring(text, idx + 1);

  char *trm = trim(sub);

  char *result = removeQuotes(trm);

  return result;
}

char* readTextFile(char *file, int max)
{
  FILE *fp = fopen(file, "r");

  if (fp == NULL) {
    printf("File %s not found", file);
    return {'\0'};
  }

  if (max == 0) max = 255;

  char *buffer = new char[max + 1];

  int i = 0;
  char c;
  while ((c = fgetc(fp)) != EOF) {
    buffer[i++] = c;
    if (i >= max) break;
  }
  buffer[i] = '\0';

  fclose(fp);

  return buffer;
}

bool writeTextFile(const char *file, char *text)
{
  FILE *fp = fopen(file, "w");

  if (fp == NULL) {
    return false;
  }

  fprintf(fp, "%s", text);

  fclose(fp);

  return true;
}

char* readStdIn(int max)
{
  char *buffer = new char[max];

  fgets(buffer, max, stdin);
  
  // If CTRL+C pressed buffer has not \0.
  int end = 0;
  for (int i = 0; i < max; i++) {
    if (buffer[i] == '\0') {
      end = i;
      break;
    }
  }
  if (end == 0) {
    return { '\0' };
  }

  return trim(buffer);
}

char* currentDate()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iDate = (1900 + tm.tm_year) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;

  char *buffer = new char[9];
  char *result = _itoa(iDate, buffer, 10);
  return result;
}

char* currentTime()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iHHMM = tm.tm_hour * 100 + tm.tm_min;

  char *buffer = new char[9];
  char *result = _itoa(iHHMM, buffer, 10);
  return result;
}

char* currentWorkDir()
{
  char *buffer = new char[FILENAME_MAX];

  _getcwd(buffer, FILENAME_MAX);

  return buffer;
}

int getOption(int argc, char* argv[])
{
  if (argc == 0) return 0;
  
  char* opt = argv[1];

  int cmp = strcmp(opt, "-h");
  if (cmp == 0) return OPT_HELP;

  cmp = strcmp(opt, "-l");
  if (cmp == 0) return OPT_LOWERCASE;

  cmp = strcmp(opt, "-u");
  if (cmp == 0) return OPT_UPPERCASE;

  cmp = strcmp(opt, "-c");
  if (cmp == 0) return OPT_CAPITALIZE;
  
  cmp = strcmp(opt, "-r");
  if (cmp == 0) return OPT_REVERSE;

  cmp = strcmp(opt, "-q");
  if (cmp == 0) return OPT_QUOTE;

  cmp = strcmp(opt, "-n");
  if (cmp == 0) return OPT_NUMBERS;

  cmp = strcmp(opt, "-e");
  if (cmp == 0) return OPT_ENVIRONMENT;

  cmp = strcmp(opt, "-f");
  if (cmp == 0) return OPT_READ_FILE;

  cmp = strcmp(opt, "-s");
  if (cmp == 0) return OPT_SAVE_FILE;

  cmp = strcmp(opt, "-d");
  if (cmp == 0) return OPT_DATE;

  cmp = strcmp(opt, "-t");
  if (cmp == 0) return OPT_TIME;

  cmp = strcmp(opt, "-v");
  if (cmp == 0) return OPT_VALUE;

  cmp = strcmp(opt, "-w");
  if (cmp == 0) return OPT_WORK_DIR;

  cmp = strcmp(opt, "-i");
  if (cmp == 0) return OPT_INTERACTIVE;

  return 0;
}

void showHelp()
{
  const char* PRG_NAME = "copytext";
  const int   PRG_VER_MAJ = 1;
  const int   PRG_VER_MIN = 0;

  printf("%s ver. %d.%d\n\n", PRG_NAME, PRG_VER_MAJ, PRG_VER_MIN);

  printf("Options:\n\n");
  printf("  -h: help;\n");
  printf("  -l: lowercase;\n");
  printf("  -u: uppercase;\n");
  printf("  -c: capitalize;\n");
  printf("  -r: reverse;\n");
  printf("  -q: quote;\n");
  printf("  -n: extract numbers;\n");
  printf("  -e: environment variable;\n");
  printf("  -f: read file;\n");
  printf("  -s: save to file copytext.txt;\n");
  printf("  -d: append date;\n");
  printf("  -t: append time;\n");
  printf("  -v: extract value from key=value;\n");
  printf("  -w: prepend current work directory.\n\n");

  printf("Example:\n\n");
  printf("> copytext -u hello\n");
  printf("  (HELLO copied in clipboard)\n\n");
}