#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)

#include <direct.h>
#include <windows.h>

#define EMPTY_STRING  _strdup("")
#define STRDUP(s)     _strdup(s)
#define CURR_WORK_DIR _getcwd((char*) NULL, 0)

#else

#include <unistd.h>

#define EMPTY_STRING  strdup("")
#define STRDUP(s)     strdup(s)
#define CURR_WORK_DIR getcwd((char*) NULL, 0)

#endif

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
#define OPT_TEST        16

// Structures

typedef struct _MAP_ENTRY
{
  char *key;
  char *value;
  int  hashCode;
  struct _MAP_ENTRY *next;
} MAP_ENTRY;

// Functions

// itoa and _itoa are not defined in ANSI-C, but are supported by some compilers.
char* __itoa(int value, char* buffer, int base);

char* lowerCase(const char *text);
char* upperCase(const char *text);
char* capitalize(const char *text);
char* reverse(const char *text);
char* quote(const char *text);
char* numbers(const char *text);
char* concat(const char *text1, const char *text2);
char* concatPath(const char *text1, const char *text2);
char* ltrim(const char *text);
char* rtrim(const char *text);
char* trim(const char *text);
char* lpad(const char *text, char c, int length);
char* rpad(const char *text, char c, int length);
char* substring(const char *text, int beginIndex, int endIndex);
short startsWith(const char *text, const char *prefix, short ignoreCase);
short endsWith(const char *text, const char *suffix, short ignoreCase);
short equalsIgnoreCase(const char *text1, const char *text2);
char* removeQuotes(const char *text);

int indexOf(const char *text, char c);
int lastIndexOf(const char *text, char c);
int hashCode(const char *text);

char* readTextFile(const char *file, int max);
short writeTextFile(const char *file, char *text);
char* readStdIn(int max);

char* getKey(const char *text);
char* getValue(const char *text);
char* findValue(const char *text);

MAP_ENTRY* parseConfig(const char *text);
MAP_ENTRY* loadConfigFile(const char *file);
MAP_ENTRY* cloneEntry(MAP_ENTRY* entry);
MAP_ENTRY* findEntry(const char *key, MAP_ENTRY* entries);
MAP_ENTRY* searchEntry(const char *text, MAP_ENTRY* entries, short(*match_fn) (const char*, const char*, const char*));
void printEntries(MAP_ENTRY* entries);
short match(const char *text, const char *key, const char *value);

char* currentDate();
char* currentTime();

int getOption(int argc, char* argv[]);
void showHelp();
void test();

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
    text = lowerCase(text);
    break;
  case OPT_UPPERCASE:
    text = upperCase(text);
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
    text = findValue(text);
    break;
  case OPT_WORK_DIR:
    text = concatPath(CURR_WORK_DIR, text);
    break;
  case OPT_INTERACTIVE:
    text = readStdIn(100);
    break;
  case OPT_TEST:
    test();
    break;
  }

#if defined(_WINDOWS_) || defined(_WINDOWS_H)

  GLOBALHANDLE hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, 1024);
  char *pGlobal = (char*)GlobalLock(hGlobal);
  strcpy(pGlobal, text);
  GlobalUnlock(hGlobal);

  OpenClipboard(0);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hGlobal);
  CloseClipboard();

#else

  printf("%s\n", text);

#endif

  return 0;
}

// Functions Implementation

char* __itoa(int value, char* buffer, int base)
{
  // invalid input
  if (base < 2 || base > 32)
    return buffer;

  // consider absolute value of number
  int n = abs(value);
  int i = 0;
  while (n) {
    int r = n % base;
    if (r >= 10)
      buffer[i++] = 65 + (r - 10);
    else
      buffer[i++] = 48 + r;
    n = n / base;
  }

  // if number is 0
  if (i == 0)
    buffer[i++] = '0';

  // If base is 10 and value is negative, the resulting string 
  // is preceded with a minus sign (-)
  // With any other base, value is always considered unsigned
  if (value < 0 && base == 10)
    buffer[i++] = '-';

  buffer[i] = '\0'; // null terminate string

  // reverse the string
  int c = 0;
  while (c < i) {
    char t = buffer[--i];
    buffer[i] = buffer[c];
    buffer[c++] = t;
  }
  return buffer;
}

char* lowerCase(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;
  while (*text != '\0') {
    *buffer++ = tolower(*text++);
  }
  *buffer = '\0';

  return result;
}

char* upperCase(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;
  while (*text != '\0') {
    *buffer++ = toupper(*text++);
  }
  *buffer = '\0';

  return result;
}

char* capitalize(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // bool first = true; // Not supported by ISO C90
  short first = 1;
  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;
  while (*text != '\0') {
    if (*text < 64) {
      *buffer++ = *text++;
    }
    else if (first) {
      *buffer++ = toupper(*text++);
      first = 0;
    }
    else {
      *buffer++ = tolower(*text++);
    }
  }
  *buffer = '\0';

  return result;
}

char* reverse(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;
  for (int i = 0; i < len; i++) {
    *buffer++ = text[len - 1 - i];
  }
  *buffer = '\0';

  return result;
}

char* quote(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  int count = 0;
  for (int i = 0; i < len; i++) {
    if (text[i] == '\'') {
      count++;
    }
  }

  // char *buffer = new char[len + 1 + 2 + count];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1 + 2 + count));
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

char* numbers(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  int count = 0;
  for (int i = 0; i < len; i++) {
    if (text[i] >= 48 && text[i] <= 57) {
      count++;
    }
  }

  // char *buffer = new char[count + 1];
  char *buffer = (char*)malloc(sizeof(char) * (count + 1));
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

char* concat(const char *text1, const char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  // char *buffer = new char[len1 + len2 + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len1 + len2 + 1));
  strcpy(buffer, text1);
  strcat(buffer, text2);

  return buffer;
}

char* concatPath(const char *text1, const char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  char c0 = len1 > 0 ? text1[0] : '\0';

  // char *buffer = new char[len1 + len2 + 2];
  char *buffer = (char*)malloc(sizeof(char) * (len1 + len2 + 2));
  strcpy(buffer, text1);
  strcat(buffer, c0 == '/' ? "/" : "\\");
  strcat(buffer, text2);

  return buffer;
}

char* ltrim(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;

  // bool copy = false; // Not supported by ISO C90
  short copy = 0;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
      copy = 1;
    }
    else if (copy) {
      *buffer++ = *text;
    }
    text++;
  }
  *buffer = '\0';

  return result;
}

char* rtrim(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;

  buffer[len] = '\0';
  // bool copy = false; // Not supported by ISO C90
  short copy = 0;
  int j = (int)len;
  for (int i = 0; i < len; i++) {
    j--;
    if (text[j] > 32) {
      buffer[j] = text[j];
      copy = 1;
    }
    else if (copy) {
      buffer[j] = text[j];
    }
    else {
      buffer[j] = '\0';
    }
  }

  return result;
}

char* trim(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;

  int i = 0;
  int lastPrintable = -1;

  // bool copy = false; // Not supported by ISO C90
  short copy = 0;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
      copy = 1;
      lastPrintable = i++;
    }
    else if (copy) {
      *buffer++ = *text;
      i++;
    }
    text++;
  }
  if (lastPrintable < 0) {
    *buffer = '\0';
  }
  else {
    // buffer e' incrementato, si usa result
    result[lastPrintable + 1] = '\0';
  }

  return result;
}

char* lpad(const char *text, char c, int length)
{
  size_t len = strlen(text);
  if (len >= length) return STRDUP(text);

  // char *buffer = new char[length + 1];
  char *buffer = (char*)malloc(sizeof(char) * (length + 1));
  char *result = buffer;

  int d = length - (int)len;
  for (int i = 0; i < d; i++) {
    *buffer++ = c;
  }
  strcpy(buffer, text);

  return result;
}

char* rpad(const char *text, char c, int length)
{
  size_t len = strlen(text);
  if (len >= length) return STRDUP(text);

  // char *buffer = new char[length + 1];
  char *buffer = (char*)malloc(sizeof(char) * (length + 1));
  char *result = buffer;

  while (*text != '\0') {
    *buffer++ = *text++;
  }
  int d = length - (int)len;
  for (int i = 0; i < d; i++) {
    *buffer++ = c;
  }
  *buffer = '\0';

  return result;
}

char* substring(const char *text, int beginIndex, int endIndex)
{
  // Overloading not supported in C (but supported in C++)
  if (endIndex == -1) {
    endIndex = (int)strlen(text);
  }
  if (beginIndex >= endIndex) {
    return EMPTY_STRING;
  }

  int len = endIndex - beginIndex;
  int count = 0;

  // char *buffer = new char[len + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len + 1));
  char *result = buffer;
  text += beginIndex;
  while (*text != '\0') {
    *buffer++ = *text++;
    count++;
    if (count >= len) break;
  }
  *buffer = '\0';

  return result;
}

short startsWith(const char *text, const char *prefix, short ignoreCase)
{
  size_t lentxt = strlen(text);
  size_t lenprf = strlen(prefix);
  if (lentxt < lenprf) return 0;
  if (ignoreCase) {
    while (*prefix != '\0') {
      if (tolower(*text++) != tolower(*prefix++)) return 0;
    }
  }
  else {
    while (*prefix != '\0') {
      if (*text++ != *prefix++) return 0;
    }
  }
  return 1;
}

short endsWith(const char *text, const char *suffix, short ignoreCase)
{
  size_t lentxt = strlen(text);
  size_t lensfx = strlen(suffix);
  if (lentxt < lensfx) return 0;
  if (ignoreCase) {
    for (int i = 0; i < lensfx; i++) {
      if (tolower(suffix[lensfx - 1 - i]) != tolower(text[lentxt - 1 - i])) return 0;
    }
  }
  else {
    for (int i = 0; i < lensfx; i++) {
      if (suffix[lensfx - 1 - i] != text[lentxt - 1 - i]) return 0;
    }
  }
  return 1;
}

short equalsIgnoreCase(const char *text1, const char *text2)
{
  size_t lentxt1 = strlen(text1);
  size_t lentxt2 = strlen(text2);
  if (lentxt1 != lentxt2) return 0;
  while (*text1 != '\0') {
    if (tolower(*text1++) != tolower(*text2++)) return 0;
  }
  return 1;
}

char* removeQuotes(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return EMPTY_STRING;

  if (text[0] != '\'' && text[0] != '"') {
    return STRDUP(text);
  }
  if (text[len - 1] != '\'' && text[len - 1] != '"') {
    return STRDUP(text);
  }

  int i = 0;
  // char *buffer = new char[len - 2 + 1];
  char *buffer = (char*)malloc(sizeof(char) * (len - 2 + 1));
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

int indexOf(const char *text, char c)
{
  size_t len = strlen(text);
  if (len == 0) return -1;

  for (int i = 0; i < len; i++) {
    if (text[i] == c) return i;
  }

  return -1;
}

int lastIndexOf(const char *text, char c)
{
  size_t len = strlen(text);
  if (len == 0) return -1;

  int j = (int)len;
  for (int i = 0; i < len; i++) {
    if (text[--j] == c) return j;
  }

  return -1;
}

int hashCode(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return 0;

  int i = 0, h = 0;
  while (i < len) {
    h = (h << 5) - h + text[i++] | 0;
  }

  return h;
}

char* readTextFile(const char *file, int max)
{
  FILE *fp = fopen(file, "r");

  if (fp == NULL) {
    printf("File %s not found", file);
    return EMPTY_STRING;
  }

  if (max == 0) max = 255;

  // char *buffer = new char[max + 1];
  char *buffer = (char*)malloc(sizeof(char) * (max + 1));

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

short writeTextFile(const char *file, char *text)
{
  FILE *fp = fopen(file, "w");

  if (fp == NULL) {
    return 0;
  }

  fprintf(fp, "%s", text);

  fclose(fp);

  return 1;
}

char* readStdIn(int max)
{
  // char *buffer = new char[max];
  char *buffer = (char*)malloc(sizeof(char) * (max + 1));

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
    return EMPTY_STRING;
  }

  return trim(buffer);
}

char* getKey(const char *text)
{
  int idx = lastIndexOf(text, '=');
  if (idx < 0) {
    idx = lastIndexOf(text, ':');
  }
  if (idx < 0) {
    return EMPTY_STRING;
  }

  char *sub = substring(text, 0, idx);

  char *result = trim(sub);

  return result;
}

char* getValue(const char *text)
{
  int idx = lastIndexOf(text, '=');
  if (idx < 0) {
    idx = lastIndexOf(text, ':');
  }
  if (idx < 0) {
    return STRDUP(text);
  }

  char *sub = substring(text, idx + 1, -1);

  char *trm = trim(sub);

  char *result = removeQuotes(trm);

  return result;
}

MAP_ENTRY* parseConfig(const char *text)
{
  // MAP_ENTRY *entries = new MAP_ENTRY[101]; // "new" is not C
  MAP_ENTRY *currEntry = NULL; // ((void *)0)
  MAP_ENTRY *lastEntry = NULL; // ((void *)0)
  MAP_ENTRY *result = NULL; // ((void *)0)

  size_t len = strlen(text);
  int c = 0;
  // char *row = new char[121];
  char *row = (char*)malloc(sizeof(char) * 121);
  for (int i = 0; i < len + 1; i++) {
    if (text[i] == 10 || text[i] == 0) { // [LF],[\0]
      // End row
      row[c] = '\0';
      if (row[0] != '\0' && row[0] != '#') {
        char *key = getKey(row);
        size_t lenKey = strlen(key);
        if (lenKey > 0) {
          char *val = getValue(row);
          int hash = hashCode(key);

          free(row);

          currEntry = (MAP_ENTRY*)malloc(sizeof(MAP_ENTRY));
          currEntry->key = key;
          currEntry->value = val;
          currEntry->hashCode = hash;
          currEntry->next = NULL; // ((void *)0)
          if (lastEntry) {
            lastEntry->next = currEntry;
          }
          else {
            result = currEntry;
          }
          lastEntry = currEntry;
        }
      }
      // row = new char[121];
      row = (char*)malloc(sizeof(char) * 121);
      c = 0;
    }
    else if (text[i] > 31) {
      row[c++] = text[i];
    }
  }

  return result;
}

MAP_ENTRY* loadConfigFile(const char *file)
{
  char *text = readTextFile(file, 4096);

  return parseConfig(text);
}

MAP_ENTRY* cloneEntry(MAP_ENTRY* entry)
{
  if (!entry) return NULL; // ((void *)0)

  // MAP_ENTRY* result = new MAP_ENTRY; // C++ 
  MAP_ENTRY* result = (MAP_ENTRY*)malloc(sizeof(MAP_ENTRY));

  result->key = entry->key;
  result->value = entry->value;
  result->hashCode = entry->hashCode;
  result->next = entry->next;

  return result;
}

MAP_ENTRY* findEntry(const char *key, MAP_ENTRY* entries)
{
  if (!entries) return NULL; // ((void *)0)

  int keyHashCode = hashCode(key);

  MAP_ENTRY* entry = entries;
  while (1) {
    if (entry->hashCode == keyHashCode) {
      return entry;
    }
    if (!entry->next) break;
    entry = entry->next;
  }
  return NULL; // ((void *)0)
}

/* Example with pointer to function (match_fn) */
MAP_ENTRY* searchEntry(const char *text, MAP_ENTRY* entries, short(*match_fn) (const char*, const char*, const char*))
{
  if (!entries) return NULL; // ((void *)0)

  MAP_ENTRY* entry = entries;
  while (1) {
    if ((*match_fn)(text, entry->key, entry->value)) {
      return entry;
    }
    if (!entry->next) break;
    entry = entry->next;
  }
  return NULL; // ((void *)0)
}

void printEntries(MAP_ENTRY* entries)
{
  if (!entries) return;

  const int WIDTH = 25;
  MAP_ENTRY* entry = entries;
  printf("%s%s%s\n", rpad("Key", ' ', WIDTH), rpad("Value", ' ', WIDTH), "Hashcode");
  printf("%s%s%s\n", rpad("", '-', WIDTH), rpad("", '-', WIDTH), "-----------");
  while (1) {
    printf("%s%s%d\n", rpad(entry->key, ' ', WIDTH), rpad(entry->value, ' ', WIDTH), entry->hashCode);
    if (!entry->next) break;
    entry = entry->next;
  }
}

short match(const char *text, const char *key, const char *value)
{
  short result = startsWith(value, text, 1);
  if (!result) result = endsWith(value, text, 1);
  return result;
}

char* findValue(const char *text)
{
  int kvs = indexOf(text, '=');
  if (kvs >= 0) {
    return getValue(text);
  }
  int sep = indexOf(text, '@');
  if (sep <= 0) {
    return getValue(text);
  }

  char* key = substring(text, 0, sep);
  char* cfg = substring(text, sep + 1, -1);

  MAP_ENTRY *entries = loadConfigFile(cfg);
  MAP_ENTRY *entry = findEntry(key, entries);

  if (entry) {
    return entry->value;
  }
  return EMPTY_STRING;
}

char* currentDate()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iDate = (1900 + tm.tm_year) * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;

  // char *buffer = new char[9];
  char *buffer = (char*)malloc(sizeof(char) * 9);
  char *result = __itoa(iDate, buffer, 10);
  return result;
}

char* currentTime()
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int iHHMM = tm.tm_hour * 100 + tm.tm_min;

  // char *buffer = new char[5];
  char *buffer = (char*)malloc(sizeof(char) * 5);
  char *result = __itoa(iHHMM, buffer, 10);
  return result;
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

  cmp = strcmp(opt, "-z");
  if (cmp == 0) return OPT_TEST;

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
  printf("  -p: numbers with padding;\n");
  printf("  -e: environment variable;\n");
  printf("  -f: read file;\n");
  printf("  -s: save to file copytext.txt;\n");
  printf("  -d: append date;\n");
  printf("  -t: append time;\n");
  printf("  -v: extract value from key=value;\n");
  printf("  -w: prepend current work directory;\n");
  printf("  -i: interactive (stdin);\n");
  printf("  -z: test.\n\n");

  printf("Example:\n\n");
  printf("> copytext -u hello\n");
  printf("  (HELLO copied in clipboard)\n\n");
}

void test()
{
  const char* text = " heLL0 ";
  printf("lowerCase(\"%s\") -> \"%s\"\n", text, lowerCase(text));
  printf("upperCase(\"%s\") -> \"%s\"\n", text, upperCase(text));
  printf("capitalize(\"%s\") -> \"%s\"\n", text, capitalize(text));
  printf("reverse(\"%s\") -> \"%s\"\n", text, reverse(text));
  printf("quote(\"%s\") -> \"%s\"\n", text, quote(text));
  printf("numbers(\"%s\") -> \"%s\"\n", text, numbers(text));
  printf("concat(\"%s\",\"%s\") -> \"%s\"\n", text, text, concat(text, text));
  printf("concatPath(\"%s\",\"%s\") -> \"%s\"\n", text, text, concatPath(text, text));
  printf("ltrim(\"%s\") -> \"%s\"\n", text, ltrim(text));
  printf("rtrim(\"%s\") -> \"%s\"\n", text, rtrim(text));
  printf("trim(\"%s\") -> \"%s\"\n", text, trim(text));
  printf("lpad(\"%s\", '-', 10) -> \"%s\"\n", text, lpad(text, '-', 10));
  printf("rpad(\"%s\", '-', 10) -> \"%s\"\n", text, rpad(text, '-', 10));
  printf("substring(\"%s\", 1, 3) -> \"%s\"\n", text, substring(text, 1, 3));
  printf("substring(\"%s\", 1, -1) -> \"%s\"\n", text, substring(text, 1, -1));
  printf("indexOf(\"%s\",'L') -> %d\n", text, indexOf(text, 'L'));
  printf("lastIndexOf(\"%s\",'L') -> %d\n", text, lastIndexOf(text, 'L'));
  printf("startsWith(\"%s\",\" h\", 0) -> %d\n", text, startsWith(text, " h", 0));
  printf("startsWith(\"%s\",\"xx\", 0) -> %d\n", text, startsWith(text, "xx", 0));
  printf("endsWith(\"%s\",\"0 \", 0) -> %d\n", text, endsWith(text, "0 ", 0));
  printf("endsWith(\"%s\",\"xx\", 0) -> %d\n", text, endsWith(text, "xx", 0));
  printf("equalsIgnoreCase(\"hello\",\"HeLLo\") -> %d\n", equalsIgnoreCase("hello", "HeLLo"));
  printf("hashCode(\"%s\") -> %d\n", text, hashCode(text));

  const char* config = "# Comment\nname=Clark\nsurname='Kent'\ngender=M\ncity=Metropolis\nnickname=Superman";
  printf("\nparseConfig...\n\n%s\n\n", config);

  MAP_ENTRY *entries = parseConfig(config);
  printEntries(entries);

  printf("\n");

  MAP_ENTRY *entry = findEntry("city", entries);
  if (entry) {
    printf("findEntry(\"city\", entries) -> entry.value=\"%s\"\n", entry->value);
  }
  else {
    printf("findEntry(\"city\", entries) -> NULL");
  }

  entry = findEntry("name", entries);
  if (entry) {
    printf("findEntry(\"name\", entries) -> entry.value=\"%s\"\n", entry->value);
  }
  else {
    printf("findEntry(\"name\", entries) -> NULL");
  }

  entry = searchEntry("metro", entries, &match);
  if (entry) {
    printf("searchEntry(\"metro\", entries, &match) -> entry.key=\"%s\", entry.value=\"%s\"\n", entry->key, entry->value);
  }
  else {
    printf("findEntry(\"metro\", entries, &match) -> NULL");
  }
}