#pragma once
#include <stdio.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tree.h"
#include "list.h"

struct cell 
{
    int pid;
    struct cell* next;
};
void printTable(struct cell*);
string_struct correct(string_struct);
struct cell* Clean_Table(struct cell*);
struct cell* Add_Table(int pid, struct cell*);
void runexec(tree, struct cell*);
