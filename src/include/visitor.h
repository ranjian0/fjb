#ifndef FJB_VISITOR_H
#define FJB_VISITOR_H
#include "AST.h"
#include "parser.h"
#include "gc.h"
#include "list.h"

#define NONE -1
#define LEFT 0
#define RIGHT 1

typedef struct FJB_VISITOR_STRUCT {
  parser_T* parser;
  const char* filepath;
  list_T* imports;
  list_T* es_exports;
  AST_T* module;
  AST_T* exports;
  list_T* pre_loaded_symbols;
} visitor_T;

visitor_T* init_visitor(parser_T* parser, const char* filepath, list_T* imports, AST_T* module, AST_T* exports);

AST_T* visitor_visit(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack);

AST_T* visitor_visit_function(visitor_T* visitor, AST_T* ast, list_T* args, list_T* stack);

void visitor_free(visitor_T* visitor);

AST_T* get_pointer(AST_T* ptr, unsigned int body);

AST_T* resolve_pointer(AST_T* ptrin, char* name, list_T* stack);

void loads(AST_T* ast, visitor_T* visitor, list_T* imported_symbols, list_T* refs);
#endif
