#include <newt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "config.h"

/*
 * nremind: Newt-based front end for adding events to remind(1).
 * Author: Paul M. Foster <paulf@quillandmouse.com>
 * Copyright: 2025
 * License: GPLv2
 */

extern CONFIG cfg;

struct event {
    bool personal;
    char date[21];
    int warn;
    char text[51];
};

bool empty(char *field)
{
    char *p;
    bool status = true;

    if (strlen(field) != 0) {
        p = field;
        while (*p != '\0') {
            if (!isblank(*p)) {
                status = false;
                break;
            }
        }
    }
    return status;
}

int save_event(struct event *ev)
{
    FILE *file;
    int result;

    if (empty(ev->date) || empty(ev->text))
        return -1;

    if (ev->personal)
        file = fopen(cfg.personal, "a");
    else
        file = fopen(cfg.general, "a");

    if (file == NULL)
        return -1;

    if (ev->warn == 0) 
        result = fprintf(file, "rem %s msg %s\n", ev->date, ev->text);
    else 
        result = fprintf(file, "rem %s +%d msg %s %%b\n", ev->date, ev->warn, ev->text);

    fclose(file);

    return result;
}

//              1         2         3         4         5         6
//    01234567890123456789012345678901234567890123456789012345678901234567890
//
//  1 Personal     123
//  2 Event Date   123456678901234567890
//  3 Warning Days 123
//  4 Event Text   123456789012345678901234567890123456789012345678901234567890
//  5
//  6 <Save> <Cancel>

int main()
{
    struct event *ev;
    int result = -2, i, rtn;
    char c;
    newtComponent fm, personal, label[4], evtdt, warndays, evttext, button[2];

    read_config();
    newtInit();
    newtCenteredWindow(63, 7, "Add Remind Event");
    fm = newtForm(NULL, NULL, 0);

    ev = malloc(sizeof(struct event));

    label[0] = newtLabel(0, 1, "Personal");
    label[1] = newtLabel(0, 2, "Event Date");
    label[2] = newtLabel(0, 3, "Warning Days");
    label[3] = newtLabel(0, 4, "Event Text");

    for (i = 0; i < 4; i++) 
        newtFormAddComponent(fm, label[i]);

    personal = newtCheckbox(13, 1, "", ' ', " *", NULL);
    evtdt = newtEntry(13, 2, "", 20, NULL, 0);
    warndays = newtEntry(13, 3, "", 3, NULL, 0);
    evttext = newtEntry(13, 4, "", 50, NULL, 0);

    newtFormAddComponents(fm, personal, evtdt, warndays, evttext, NULL);

    button[0] = newtCompactButton(0, 6, "Save");
    button[1] = newtCompactButton(7, 6, "Cancel");

    newtFormAddComponents(fm, button[0], button[1], NULL);

    // process form
    struct newtExitStruct es;
    do {
	    newtFormRun(fm, &es);
    } while (es.reason != NEWT_EXIT_COMPONENT);

    newtPopWindow();

    // which component caused the exit?
    newtComponent save_or_cancel = newtFormGetCurrent(fm);
    if (save_or_cancel == button[0]) {
        c = newtCheckboxGetValue(personal);
        if (c == '*')
            ev->personal = true;
        else
            ev->personal = false;

        strcpy(ev->date, newtEntryGetValue(evtdt));
        ev->warn = atoi(newtEntryGetValue(warndays));
        strcpy(ev->text, newtEntryGetValue(evttext));
        result = save_event(ev);
    }

    newtFormDestroy(fm);
    newtCls();
    newtFinished();
    free_config(&cfg);
    free(ev);

    switch (result) {
        case 0:
        case -1:
            printf("UNABLE to save event!\n");
            rtn = 1;
            break;
        case -2:
            printf("User CANCELED operation.\n");
            rtn = 2;
            break;
        default:
            printf("Event SUCCESSFULLY saved.\n");
            rtn = 0;
            break;
    }

    return rtn;
}
