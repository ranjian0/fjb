#include "include/compound.h"
#include "include/resolve.h"
#include "include/AST.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  AST_T* lookup;
  AST_T* compound;
  AST_T* last_pushed;
  AST_T* parent;
  list_T* saved;
  list_T* skip;
} options_T;


static unsigned int resolve_basic_query(AST_T* ast, query_T data)
{
  if (!ast) return 0;
  if (data.name == 0) return 0;

  unsigned int type_match = (ast->type == data.type) || data.type == -1;
  unsigned int name_match = ast->name && (strcmp(ast->name, data.name) == 0);

  return type_match && name_match;
}

static unsigned int resolve_deps_query(AST_T* ast, query_T data)
{
  if (!ast) return 0;
  if (!ast->name) return 0;
  if (data.type == AST_ASSIGNMENT && !ast->flags) return 0;
  if (data.type == AST_FUNCTION && !ast->name) return 0;
  if (data.parent && ast->parent && (ast->parent != data.parent)) return 0;
  if (ast->from_obj) return 0;
  //if (data.ignore && ptr_in_list(data.ignore, ast)) return 0;
  return resolve_basic_query(ast, data);
}

list_T* get_imported_symbols(AST_T* lookup, list_T* imports)
{
  list_T* list = NEW_STACK;

  AST_T* resolved = 0;

  int types[] = { AST_ASSIGNMENT, AST_FUNCTION };
  size_t nr_types = 2;
  query_T data;
  

  LOOP_NODES(imports, i, child,
    {
      for (unsigned int k = 0; k < nr_types; k++)
      {
        data.type = types[k];
        data.name = child->name;
        resolved = resolve(lookup, resolve_basic_query, data);
        if (resolved)
        {
          list_push_safe(list, resolved);
          break;
        }
      }
    } 
  );

  return list;
}

void get_deps(AST_T* ast, options_T args)
{
  if (!ast) return;
  if (!ast->parent && ast->type != AST_COMPOUND) return; 

  if (ast->left && ast->type != AST_CALL) get_deps(ast->left, args);

  if (ast->type == AST_BINOP)
    return;
  
  if (ast->right && !ast->right->from_obj) get_deps(ast->right, args);

  if (ast->expr) get_deps(ast->expr, args);
  if (ast->body) get_deps(ast->body, args);
  if (ast->next) get_deps(ast->next, args);
  if (ast->value) get_deps(ast->value, args);

  if (ast->list_value && ast->type != AST_FUNCTION)
  {
    LOOP_NODES(ast->list_value, i, child,
        get_deps(child, args);
    );
  }
  
  list_T* pointers = NEW_STACK;

  if (ast->name && !ast->from_obj && ast->parent && (ast->type == AST_NAME || ast->type == AST_CALL))
  {
    query_T query;
    query.name = ast->name;
    query.parent = args.parent;
    //query.ignore = NEW_STACK;
    //list_push_safe(query.ignore, ast);
  

    int types[] = { AST_ASSIGNMENT, AST_FUNCTION };
    size_t nr_types = 2;

    for (unsigned int i = 0; i < nr_types; i++)
    {
      query.type = types[i];
      AST_T* ptr = resolve(args.lookup, resolve_deps_query, query);

      if (ptr) list_push(pointers, ptr);
    }
  }

  LOOP_NODES(pointers, i, ptr,
  {
    if (!ptr->name || ptr_in_list(args.saved, ptr) || ptr->from_obj)
      continue;

    /*if (ast->parent && ast->parent->type == AST_FUNCTION && ast->parent->name)
    {
      skip += ptr_in_list(ast->parent->list_value, ptr);
    }*/

    query_T query;
    query.type = ptr->type;
    query.name = ptr->name;
      
    if (!resolve(args.compound, resolve_deps_query, query))
    {
      list_push_at(args.saved, ptr, args.last_pushed ? args.last_pushed : ast);
      args.last_pushed = ptr;
      //args.parent = ptr->parent;
      get_deps(ptr, args);
    }
  });
}

AST_T* new_compound(AST_T* lookup, list_T* imports, list_T* es_exports)
{
  if (!imports->size) return lookup;

  AST_T* compound = init_ast(AST_COMPOUND);
  compound->list_value = get_imported_symbols(lookup, imports);
  compound->es_exports = imports;

  AST_T* parent = 0;

  if (compound->list_value && compound->list_value->size) parent = ((AST_T*) compound->list_value->items[0])->parent;

  
  options_T args;
  args.lookup = lookup;
  args.saved = NEW_STACK;
  args.skip = NEW_STACK;
  args.compound = compound;
  args.parent = parent;
  get_deps(compound, args);
  
  list_T* all_symbols = list_merge(args.saved, compound->list_value);

  compound->list_value = all_symbols->size ? all_symbols : lookup->list_value;

  return compound;
}
