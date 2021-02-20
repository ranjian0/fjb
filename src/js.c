#include "include/js.h"
#include "include/enums/JS_EVENTS.gpp.h"
#include <string.h>

unsigned int is_js_event(char* name)
{
  if (!name)
    return 0;

  for (unsigned int i = 0; i < JS_EVENTS_LEN; i++) {
    if (strcasecmp(name, JS_EVENTS[i]) == 0)
      return 1;
  }

  return 0;
}
