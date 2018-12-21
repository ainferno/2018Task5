#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "tree.h"

tree init()
{
    ArgMax = 10;
    tree tr = malloc(sizeof(struct cmd_inf));
    tr->argv = malloc(sizeof(char*)*ArgMax);
    tr->infile = NULL;
    tr->outfile = NULL;
    tr->backgrnd = 0;
    tr->psubcmd = NULL;
    tr->pipe = NULL;
    tr->next = NULL;
    tr->tnext = NXT;
    return tr;
}

tree addArg(tree tr, int i, char* str)
{
    if (i>=ArgMax) 
    {    
        ArgMax *=2;
        tr->argv = realloc(tr->argv,sizeof(char*)*ArgMax);
    }
    tr->argv[i] = str;
    return tr;
}

void setBack(tree tr)
{
    if(tr == NULL)
        return;
    tr->backgrnd = 1;
    setBack(tr->pipe);
    if(tr->tnext == AND || tr->tnext == OR)
        setBack(tr->next);
}

tree build_tree(string_struct lst, int n)
{
    if (lst.size_current == 0)
        return NULL;
    tree Start, Current, Last, Conveyor;
    int i = 0;
    enum next_type 
    {
        check, conv, back, out, next, in, psub, end, and, or
    };
    Start = Current = init();
    Conveyor = Last = Current;//NEW
    enum next_type cur = check;
    while(lst.array[n] != NULL)
    {
        switch(cur)
        {
            case check://Check lex
                if (lst.array[n] == NULL)
                {
                    cur = end;
                    break;
                }
                if (!strcmp(lst.array[n], "&&"))
                {
                    cur = and; 
                    n++;
                    break;
                }
                if (!strcmp(lst.array[n], "||"))
                {
                    cur = or; 
                    n++;
                    break;
                }
                if (!strcmp(lst.array[n], "|"))
                {
                    cur = conv; 
                    n++;
                    break;
                }
                if (!strcmp(lst.array[n], "&"))
                {
                    cur = back; 
                    break;
                }
                if (!strcmp(lst.array[n], ">"))
                {
                    cur = out; 
                    n++; 
                    break;
                }
                if (!strcmp(lst.array[n], ";"))
                {
                    cur = next; 
                    n++; 
                    break;
                }
                if (!strcmp(lst.array[n], "<"))
                {
                    cur = in; 
                    n++; 
                    break;
                }
                if (!strcmp(lst.array[n], "("))
                {
                    cur = psub;
                    n++;
                    break;
                }
                if (!strcmp(lst.array[n], ")"))
                {
                    cur = end; 
                    break;
                }
                Current = addArg(Current, i++, lst.array[n++]);
                break;
            case and://We found &&
                Current = addArg(Current,i,NULL);
                Current->tnext = AND;
                Current = init();//NEW
                i = 0;
                Last->next = Current;
                Last = Current;
                cur = check;
                break;
            case or://We found &&
                Current = addArg(Current,i,NULL);
                Current->tnext = OR;
                Current = init();//NEW
                i = 0;
                Last->next = Current;
                Last = Current;
                cur = check;
                break;
            case conv://We found |
                Current = addArg(Current, i, NULL);
                Current = init();
                i = 0;
                Last->pipe = Current;
                Last = Current;
                cur = check;
                break;
            case back://We found process that should be background
                setBack(Conveyor);//new
                n++;
                cur = check;
                break;
            case out://We found output file
                Current->outfile = lst.array[n++];
                cur = check;
                break;
            case next://We found process that is not background
                Current = addArg(Current,i,NULL);
                Current = init();//NEW
                i = 0;
                Last->next = Current;
                Last = Current;
                cur = check;
                break;
            case in://We found input file
                Current->infile = lst.array[n++];
                cur = check;
                break;
            case psub://We found sub shell
                Current = addArg(Current,i,NULL);
                Current->psubcmd = build_tree(lst, n);
                if ((lst.array[n] == NULL) || strcmp(lst.array[n++],")"))
                {
                    write(2, "ERROR IN SUBSHELL\n", 18); 
                    cur = end; 
                    break;
                }
                cur = check;
                break;
            case end:
                Current = addArg(Current, i, NULL);
                return Start;
                break;
        }
    }
    return Start;
}

tree clear_tree(tree tr)
{
    int i = 0;
    if (tr == NULL)
        return NULL;
    while((tr->argv)[i]!=NULL)
        (tr->argv)[i++] = NULL;
    free(tr->argv);
    tr->argv = NULL;
    if(tr->psubcmd!=NULL)
        clear_tree(tr->psubcmd);
    if(tr->pipe!=NULL)
        clear_tree(tr->pipe);
    if(tr->next!=NULL)
        clear_tree(tr->next);
    free(tr);
    tr = NULL;
    return tr;
}

void print_tree(tree tr, int n)
{
    char num[15];
    int num_l = 0;
    write(1, "\n_Tree No.", 11);
    num_l = copy_int(num, n);
    write(1, num, num_l);
    write(1, " _\n", 3);
    if(tr == NULL)
        return;
    char* arg = NULL;
    write(1, "Arguments: ", 12);
    for(int i = 0;(tr->argv)[i] != NULL;i++)
    {
        arg = (tr->argv)[i];
        write(1, arg, strlen(arg));
        write(1, " ", 1);
    }
    write(1, "\nBackground - ", 14);
    if(tr->backgrnd)
        write(1, "True", 4);
    else
        write(1, "False", 5);
    write(1, "\nNext type - ", 13);
    switch(tr->tnext)
    {
        case NXT:
            write(1,"NEXT\n", 5);
            break;
        case AND:
            write(1,"AND\n", 4);
            break;
        case OR:
            write(1,"OR\n", 3);
            break;
    }
    if (tr->infile != NULL)
    {
        write(1, "\nInput - ", 9);
        write(1, tr->infile, strlen(tr->infile));
    }
    if (tr->outfile != NULL)
    {
        write(1, "\nOutput - ", 10);
        write(1, tr->outfile, strlen(tr->outfile));
    }
    if (tr->psubcmd != NULL)
    {
        write(1, "\n_Start SUBSHELL_\n", 18);
        print_tree(tr->psubcmd, n+1);
        write(1,"\n_End SUBSHELL_\n", 16);
    }
    if (tr->pipe != NULL)
    {
        write(1, "\n_Start Pipe_\n", 14);
        print_tree(tr->pipe, n+1);
        write(1, "\n_End Pipe_\n", 12);
    }
    if (tr->next != NULL)
    {
        write(1, "\n_Start Next_\n", 14);
        print_tree(tr->next, n+1);
        write(1, "\n_End Next_\n", 12);
    }
    write(1, "\n_End Tree No.", 14);
    write(1, num, num_l);
    write(1, " _\n", 3);
    return;
}