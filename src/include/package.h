#ifndef FJB_PACKAGE_H
#define FJB_PACKAGE_H
#define PACKAGE_NAME "package.json"
#include <json/json_parser.h>

char* package_get(const char* path, const char* key);
#endif
