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
#define OPT_TEST        16

// Structures declarations

typedef struct _MAP_ENTRY
{
  char *key;
  char *value;
  int  hashCode;
  _MAP_ENTRY *next;

} MAP_ENTRY;

// Functions declarations

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
char* substring(const char *text, int beginIndex);
char* removeQuotes(const char *text);

int indexOf(const char *text, char c);
int lastIndexOf(const char *text, char c);
int hashCode(const char *text);

char* readTextFile(const char *file, int max);
bool writeTextFile(const char *file, char *text);
char* readStdIn(int max);

char* getKey(const char *text);
char* getValue(const char *text);
char* findValue(const char *text);

MAP_ENTRY* parseConfig(const char *text);
MAP_ENTRY* loadConfigFile(const char *file);
MAP_ENTRY* cloneEntry(MAP_ENTRY* entry);
MAP_ENTRY* findEntry(const char *key, MAP_ENTRY* entries);
void printEntries(MAP_ENTRY* entries);

char* currentDate();
char* currentTime();
char* currentWorkDir();

int getOption(int argc, char* argv[]);
void showHelp();
char* test();

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
      text = concatPath(currentWorkDir(), text);
      break;
    case OPT_INTERACTIVE:
      text = readStdIn(100);
      break;
  case OPT_TEST:
      text = test();
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

char* lowerCase(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return _strdup("");

  char *buffer = new char[len + 1];
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
  if (len == 0) return _strdup("");

  char *buffer = new char[len + 1];
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
  if (len == 0) return _strdup("");

  bool first = true;
  char *buffer = new char[len + 1];
  char *result = buffer;
  while (*text != '\0') {
    if (*text < 64) {
      *buffer++ = *text++;
    }
    else if (first) {
      *buffer++ = toupper(*text++);
      first = false;
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
  if (len == 0) return _strdup("");

  char *buffer = new char[len + 1];
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
  if (len == 0) return _strdup("");

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

char* numbers(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return _strdup("");

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

char* concat(const char *text1, const char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  char *buffer = new char[len1 + len2 + 1];
  strcpy(buffer, text1);
  strcat(buffer, text2);

  return buffer;
}

char* concatPath(const char *text1, const char *text2)
{
  size_t len1 = strlen(text1);
  size_t len2 = strlen(text2);

  char *buffer = new char[len1 + len2 + 2];
  strcpy(buffer, text1);
  strcat(buffer, "\\");
  strcat(buffer, text2);

  return buffer;
}

char* ltrim(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return _strdup("");

  char *buffer = new char[len + 1];
  char *result = buffer;

  bool copy = false;
  while (*text != '\0') {
    if (*text > 32) {
      *buffer++ = *text;
      copy = true;
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
  if (len == 0) return _strdup("");

  char *buffer = new char[len + 1];
  char *result = buffer;

  buffer[len] = '\0';
  bool copy = false;
  int j = (int)len;
  for (int i = 0; i < len; i++) {
    j--;
    if (text[j] > 32) {
      buffer[j] = text[j];
      copy = true;
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
  char *buffer = ltrim(text);

  return rtrim(buffer);
}

char* lpad(const char *text, char c, int length)
{
  size_t len = strlen(text);
  if (len >= length) return _strdup(text);

  char *buffer = new char[length + 1];
  char *result = buffer;

  int d = length - (int) len;
  for (int i = 0; i < d; i++) {
    *buffer++ = c;
  }
  strcpy(buffer, text);

  return result;
}

char* rpad(const char *text, char c, int length)
{
  size_t len = strlen(text);
  if (len >= length) return _strdup(text);

  char *buffer = new char[length + 1];
  char *result = buffer;

  while (*text != '\0') {
    *buffer++ = *text++;
  }
  int d = length - (int) len;
  for (int i = 0; i < d; i++) {
    *buffer++ = c;
  }
  *buffer = '\0';

  return result;
}

char* substring(const char *text, int beginIndex, int endIndex)
{
  if (beginIndex >= endIndex) {
    return _strdup("");
  }
  
  int len = endIndex - beginIndex;
  int count = 0;

  char *buffer = new char[len + 1];
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

char* substring(const char *text, int beginIndex)
{
  size_t endIndex = strlen(text);

  if (beginIndex >= endIndex) {
    return _strdup("");
  }

  char *buffer = new char[endIndex - beginIndex + 1];
  char *result = buffer;
  text += beginIndex;
  while (*text != '\0') {
    *buffer++ = *text++;
  }
  *buffer = '\0';

  return result;
}

char* removeQuotes(const char *text)
{
  size_t len = strlen(text);
  if (len == 0) return _strdup("");

  if (text[0] != '\'' && text[0] != '"') {
    return _strdup(text);
  }
  if (text[len - 1] != '\'' && text[len - 1] != '"') {
    return _strdup(text);
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

  int j = (int) len;
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
    return _strdup("");
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
    return _strdup("");
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
    return _strdup("");
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
    return _strdup(text);
  }

  char *sub = substring(text, idx + 1);

  char *trm = trim(sub);

  char *result = removeQuotes(trm);

  return result;
}

MAP_ENTRY* parseConfig(const char *text)
{
  MAP_ENTRY *entries = new MAP_ENTRY[101];
  MAP_ENTRY *result = entries;

  size_t len = strlen(text);
  int c = 0;
  int r = 0;
  char *row = new char[120];
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

          MAP_ENTRY *entry = new MAP_ENTRY;
          entry->key = key;
          entry->value = val;
          entry->hashCode = hash;
          entry->next = NULL;

          entries[r] = *entry;
          if (r > 0) {
            entries[r - 1].next = &entries[r];
          }
          r++;
        }
      }
      if (r >= 100) break;
      row = new char[120];
      c = 0;
    }
    else if (text[i] > 31) {
      row[c++] = text[i];
    }
  }

  MAP_ENTRY *empty = new MAP_ENTRY;
  empty->key = _strdup("");
  empty->value = _strdup("");
  empty->hashCode = 0;
  empty->next = NULL;

  entries[r] = *empty;
  if (r > 0) {
    entries[r - 1].next = &entries[r];
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
  // MAP_ENTRY* result = new MAP_ENTRY; // it's the same 
  MAP_ENTRY* result = (MAP_ENTRY*)malloc(sizeof(MAP_ENTRY));
  
  result->key = entry->key;
  result->value = entry->value;
  result->hashCode = entry->hashCode;
  result->next = entry->next;

  return result;
}

MAP_ENTRY* findEntry(const char *key, MAP_ENTRY* entries)
{
  int keyHashCode = hashCode(key);

  // Clone to not change the pointer
  MAP_ENTRY* entry = cloneEntry(entries);
  while (true) {
    int entryHashCode = entry->hashCode;
    if (entryHashCode == 0) {
      return entry;
    }
    if (entryHashCode == keyHashCode) {
      return entry;
    }
    if (entry->next) {
      *entry = *entry->next;
    }
    else {
      break;
    }
  }
  // Unlikely, but you can't return NULL
  return entry;
}

void printEntries(MAP_ENTRY* entries)
{
  // Clone to not change the pointer
  MAP_ENTRY* entry = cloneEntry(entries);
  
  while (true) {
    if (entry->hashCode == 0) break;
    printf("key=\"%s\",\tvalue=\"%s\",\thashCode=%d\n", entry->key, entry->value, entry->hashCode);
    if (entry->next) {
      *entry = *entry->next;
    }
    else {
      break;
    }
  }

  free(entry);
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
  char* cfg = substring(text, sep + 1);

  MAP_ENTRY *entries = loadConfigFile(cfg);
  MAP_ENTRY *entry = findEntry(key, entries);
  
  if (entry->hashCode) {
    return entry->value;
  }
  return _strdup("");
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

char* test()
{
  char *text = _strdup(" heLL0 ");
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
  printf("substring(\"%s\", 1, 3) -> \"%s\"\n", text, substring(text,1,3));
  printf("substring(\"%s\", 1) -> \"%s\"\n", text, substring(text,1));
  printf("indexOf(\"%s\",'L') -> %d\n", text, indexOf(text, 'L'));
  printf("lastIndexOf(\"%s\",'L') -> %d\n", text, lastIndexOf(text, 'L'));
  printf("hashCode(\"%s\") -> %d\n", text, hashCode(text));

  text = _strdup("# Comment\nname=Clark\nsurname=Kent\ngender=M");
  printf("\nparseConfig...\n");

  MAP_ENTRY *entries = parseConfig(text);
  printEntries(entries);

  MAP_ENTRY *entry = findEntry("surname", entries);
  printf("findEntry(\"surname\", entries) -> entry.value=\"%s\"\n", entry->value);
  free(entry);
  
  entry = findEntry("name", entries);
  printf("findEntry(\"name\", entries) -> entry.value=\"%s\"\n", entry->value);
  free(entry);

  return _strdup("test");
}