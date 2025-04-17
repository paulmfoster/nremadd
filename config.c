#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "ini.h"

CONFIG cfg;

static int handler(void* user, const char* section, const char* name, const char* value)
{
    CONFIG *p = (CONFIG *) user;

    if (strcmp(section, "nremadd") == 0) {
        if (strcmp(name, "general") == 0)
            p->general = strdup(value);
        else if (strcmp(name, "personal") == 0) 
            p->personal = strdup(value);
        else
            return 0; // failure
    }
    else
        return 0; // failure
    
    return 1; // success
}

void free_config(CONFIG *cfg)
{
    if (cfg->general != NULL)
        free(cfg->general);
    if (cfg->personal != NULL)
        free(cfg->personal);
}

int read_config()
{
    char *home;
    char cfgfile[255];

    home = getenv("HOME");
    sprintf(cfgfile, "%s/%s", home, ".config/nremadd/nremadd.cfg");

    // safety of config
    cfg.general = NULL;
    cfg.personal = NULL;

    if (ini_parse(cfgfile, handler, &cfg) < 0) 
        return -1;

    return 0;
}

