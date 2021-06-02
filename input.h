#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>

// Sets up terminal for one-char-at-a-time reads
void cbreak();

// Returns terminal to normal state after cbreak ()
void normal();

// Checks keyboard buffer (stdin) and returns key
// pressed, or -1 for no key pressed
int keypress();