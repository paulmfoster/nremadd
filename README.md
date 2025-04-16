---
title: README
author: Paul M. Foster
date: 2025-04-16
---

# NRemind

NRemind is a newt-based front end for adding simple events to `remind(1)`.
If you want to add oddly repeating events, or events which depend on phases
of the moon or something, this program will not work. It asks for a date
and a number of days in advance to warn you about the event (may be 0).

(For those unaware of newt, it is a terminal-based library for building,
handling and operating of forms. It is widely used by various programs
common in the Linux space.)

## Configuration

NRemind needs a config file, named `nremind.cfg`. It should be placed in
the `~/.config/nremind` directory. The program makes provision for
personal and general reminder files. There should be a key in the file for
each of these. If there is no actual personal reminders file, simply set
the personal key to the same file as the general key.

One important thing to note: for the filenames in the config file, be sure
you include the absolute full path of the reminder files. I tried to use
the abbreviated version with a tilde (~), and it didn't work. I'm guessing
`fprintf(3)` doesn't translate tildes into the home directory.

## The Package

This program is coded in C and uses the newt library. There's a makefile.
There is also a compiled executable which run under Arch Linux (and
probably every other distro). Since it's not statically compiled, you'll
need the newt library on your computer to make it work.

## Bugs

If you find one, let me know.

## License

This program is provided under the GPLv2 license. Thus, you're free to
alter it and pass your alterations on to others. Of course, I'm not
responsible for what happens with this program, altered or not.


