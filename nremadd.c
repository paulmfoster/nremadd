#include <newt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "config.h"

/*
 * nremadd: Newt-based front end for adding events to remind(1).
 * Author: Paul M. Foster <paulf@quillandmouse.com>
 * Copyright: 2025
 * License: GPLv2
 */

void help()
{
    char *text =
        "This program presents a form for entering a simple reminder.\n\
It outputs the four values entered in a pipe-delimited string to STDERR.\n\
The program returns a 0 if the user enters a correctly formatted reminder.\n\
It returns 1 if the user omits the date or text. It returns 2 if the user\n\
aborts entry. And it returns 3 if the user asks for help (and also prints\n\
a help message).";

    printf("%s\n", text);
}

/*
 * NOTE: This program returns four values in a single line of text to
 * STDERR:
 *
 * PERSONAL (0 or 1) designating which reminder file to update (personal or
 * general) DATE (in ISO format; user is expected to enter the date in this
 * format) NUMBER OF WARNING DAYS (none if empty) REMINDER TEXT
 *
 * The program will return a value of 0 if the user input is valid (and the
 * reminder can be stored. It will return 1 if the user entered invalid
 * date (no date or no reminder text). It will return 2 if the user aborted
 * input. It will return 3 if you give the program an -h parameter, and
 * emit a help message. These return values can be checked by a script to
 * determine what action to take. Similarly, the text output to STDERR can
 * be captured in a variable and parse to determine what and how to write
 * to the reminder file.
 */

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

/**
 * Filter to force a date.
 *
 * This filter expects a date in the form mm/dd/yyyy. It will examine the
 * input at each cursor position and force it into this format.
 *
 * @param newtComponent the date field
 * @param void * not used
 * @param int character passed to filter
 * @param int cursor index at this character
 * @param int character or 0 if nothing to be done
 */

int iso_date_filter(newtComponent co, void *data, int ch, int cursor)
{
    char *content;
    char replaced[11] = "          ";

    if (ch == NEWT_KEY_DELETE ||
            ch == NEWT_KEY_LEFT ||
            ch == NEWT_KEY_RIGHT ||
            ch == NEWT_KEY_HOME ||
            ch == NEWT_KEY_END ||
            ch == NEWT_KEY_ENTER ||
            ch == NEWT_KEY_BKSPC)
        return ch;

    if (isdigit(ch)) {
        if (cursor == 4) {
            content = newtEntryGetValue(co);
            strcpy(replaced, content);
            replaced[4] = '-';
            replaced[5] = '\0';
            newtEntrySet(co, replaced, 1);
            return ch;
        }
        else if (cursor == 7) {
            content = newtEntryGetValue(co);
            strcpy(replaced, content);
            replaced[7] = '-';
            replaced[8] = '\0';
            newtEntrySet(co, replaced, 1);
            return ch;
        }
        else
            return ch;
    }
    else 
        if (ch == '-') {
            if (cursor == 4 || cursor == 7)
                return ch;
            else
                return 0;
        }
        else
            return 0;
}

// NOTE: no longer used
/*int save_event(struct event *ev)*/
/*{*/
/*    FILE *file;*/
/*    int result;*/
/**/
/*    if (empty(ev->date) || empty(ev->text))*/
/*        return -1;*/
/**/
/*    if (ev->personal)*/
/*        file = fopen(cfg.personal, "a");*/
/*    else*/
/*        file = fopen(cfg.general, "a");*/
/**/
/*    if (file == NULL)*/
/*        return -1;*/
/**/
/*    // fprintf returns number of bytes*/
/*    if (ev->warn == 0) */
/*        result = fprintf(file, "rem %s msg %s\n", ev->date, ev->text);*/
/*    else */
/*        result = fprintf(file, "rem %s +%d msg %s %%b\n", ev->date, ev->warn, ev->text);*/
/**/
/*    fclose(file);*/
/**/
/*    return result;*/
/*}*/
/**/

//              1         2         3         4         5         6
//    01234567890123456789012345678901234567890123456789012345678901234567890
//
//  1 Personal     123
//  2 Event Date   123456678901234567890
//  3 Warning Days 123
//  4 Event Text   123456789012345678901234567890123456789012345678901234567890
//  5
//  6 <Save> <Cancel>

int main(int argc, char *argv[])
{
    struct event *ev;
    int result = 2, i;
    char c;
    char *p;
    newtComponent fm, personal, label[4], evtdt, warndays, evttext, button[2];

    if (argc > 1) {
        p = strstr(argv[1], "-h");
        if (p != NULL) {
            help();
            result = 3;
            return result;
        }
    }

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
    newtEntrySetFilter(evtdt, iso_date_filter, NULL);
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

        result = 0;
    }

    newtFormDestroy(fm);
    newtCls();
    newtFinished();

    if (result == 0) {
        if (empty(ev->date) || empty(ev->text))
            result = 1;
        if (result == 0)
            fprintf(stderr, "%d|%s|%d|%s", ev->personal, ev->date, ev->warn, ev->text);
    }

    free(ev);

    return result;
}

