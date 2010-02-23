#ifndef _PYUTILS_H_
#define _PYUTILS_H_

#include "servers.h"

void py_command(const char *cmd, SERVER_REC *server, WI_ITEM_REC *item);
char *file_get_ext(const char *file);
int file_has_ext(const char *file, const char *ext);
char *file_get_filename(const char *path);


#endif
