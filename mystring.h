#ifndef MYSTRING_H
#define MYSTRING_H
#include <inttypes.h>

typedef struct _String {
    char* string;
    uint16_t length;
} String;

String* newString();
String* newString1(char* data);
String* strAppend(String* self, String* str);
String* strAppendc(String* self, char const* str);
String* strPrepend(String* self, String* str);
String* strPrependc(String* self, char const* str);
int strCmp(String* self, char* data);

#endif // MYSTRING_H
