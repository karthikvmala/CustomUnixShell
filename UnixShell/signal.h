#ifndef SIGNAL_H
#define SIGNAL_H
#include "standard.h"

void send_signal(int process, int signal, int flag);
void sighandler();
void dsignal();

#endif 