#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "list.h"
#include "tree.h"
#include "exec.h"

int PROG_END = 1;

void my_end()
{
    PROG_END = 0;
}
void sigC()
{
    printf("\n");
    return;
}
int main()
{
    tree tr = NULL;
    int n = 0;
    struct cell* MainTable = NULL;//Table of PID process
    string_struct lst = init_string_list();
    signal(SIGUSR1, my_end);
    void (*old)() = signal(SIGINT, SIG_IGN);
    while(PROG_END)
    {
        signal(SIGINT, SIG_IGN);
        lst = input(lst);
        if(lst.size_current != 0)
        {
            lst = correct(lst);
            print_string_list(lst);
            tr = build_tree(lst, n);
            print_tree(tr, 1);
            signal(SIGINT, sigC);
            runexec(tr, MainTable);
            clean_string_list(lst);
            clear_tree(tr);
            MainTable = Clean_Table(MainTable);

            tr = NULL;
            n = 0;
            lst = init_string_list();
            // print_string_list(lst);
        }
    }
    signal(SIGKILL, old);
    printf("EXIT\n");
}