#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char *general;
    char *personal;
} CONFIG;

int read_config();
void free_config(CONFIG *);

#endif

