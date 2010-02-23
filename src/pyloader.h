#ifndef _PYLOADER_H_
#define _PYLOADER_H_

typedef struct
{
    char *name;
    char *file;
} PY_LIST_REC;

void pyloader_add_script_path(const char *path);
int pyloader_load_script_argv(char **argv);
int pyloader_load_script(char *name);
int pyloader_unload_script(const char *name);
PyObject *pyloader_find_script_obj(void);
char *pyloader_find_script_name(void);

GSList *pyloader_list(void);
void pyloader_list_destroy(GSList **list);

int pyloader_init(void);
void pyloader_auto_load(void);
void pyloader_deinit(void);

#endif
