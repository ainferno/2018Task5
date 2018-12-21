#pragma once
#include "list.h"
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

typedef enum {NXT, AND, OR} type_next;
struct cmd_inf 
{
    char ** argv; // список из имени команды и аргументов
    char *infile; // переназначенный файл стандартного ввода
    char *outfile; // переназначенный файл стандартного вывода
    int append;
    int backgrnd; // =1, если команда подлежит выполнению в фоновом режиме
    struct cmd_inf* psubcmd; // команды для запуска в дочернем shell
    struct cmd_inf* pipe; // следующая команда после "|"
    struct cmd_inf* next; // следующая после ";" (или после "&")
    type_next tnext;
};

typedef struct cmd_inf *tree;
typedef struct cmd_inf node;

int ArgMax;

void print_tree(tree, int);
tree build_tree(string_struct, int);
tree clear_tree(tree);
