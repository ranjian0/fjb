#ifndef FJB_EMIT_TYPESCRIPT_H
#define FJB_EMIT_TYPESCRIPT_H
#include "AST.h"
#include "env.h"
char* emit_ts(AST_T* ast);
char* emit_interface(AST_T* ast);
char* emit_data_type(AST_T* ast);
#endif
