#include "mystring.h"
#include <string.h>
#include <malloc.h>

String* newString()
{
    String* str = malloc(sizeof(*str));
    if (str == NULL)
        return str;
    str->string = NULL;
    str->length = 0;
    return str;
}

String* newString1(char* data)
{
    String* str = malloc(sizeof (*str));
    if (str == NULL)
        return str;
    uint32_t dataLen = strlen(data);
    str->string = malloc(dataLen * sizeof (char));
    strcpy(str->string, data);
    str->length = dataLen;
    return str;
}

String* strAppend(String* self, String* str)
{
    if (self->string == NULL) {
        self->string = malloc(sizeof (char) * str->length);
        self->length = 0;
    } else {
        self->string = realloc(self->string, (self->length + str->length) * sizeof(char));
    }
    memcpy(self->string + self->length, str->string, sizeof(char) * str->length);
    self->length += str->length;
    return self;
}

String* strAppendc(String* self, char const* str)
{
    uint32_t strLen = strlen(str);
    if (self->string == NULL) {
        self->string = malloc(sizeof (char) * strLen);
        self->length = 0;
    } else {
        self->string = realloc(self->string, (self->length + strLen) * sizeof (char));
    }
    memcpy(self->string + self->length, str, sizeof(char) * strLen);
    self->length += strLen;
    return self;
}

String* strPrepend(String* self, String* str)
{
    if (self->string == NULL) {
        return strAppend(self, str);
    }
    char* buff = malloc(sizeof (char) * (self->length + str->length));
    memcpy(buff, str->string, str->length * sizeof (char));
    memcpy(buff + str->length, self->string, self->length * sizeof (char));
    free(self->string);
    self->string = buff;
    self->length += str->length;
    return self;
}

String* strPrependc(String* self, char const* str)
{
    if (self->string == NULL) {
        return strAppendc(self, str);
    }
    uint16_t strLen = strlen(str);
    char* buff = malloc(sizeof(char) * (self->length + strLen));
    memcpy(buff, str, strLen * sizeof (char));
    memcpy(buff + strLen, self->string, self->length * sizeof (char));
    free(self->string);
    self->string = buff;
    self->length += strLen;
    return self;
}

int strCmp(String* self, char* str)
{
    return strncmp(self->string, str, self->length);
}
