#pragma once
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifndef input_size
#define input_size 10u //Определяем размер буффера
#endif
#ifndef array_size
#define array_size 10u //Определяем размер буффера
#endif
#ifndef buf_size
#define buf_size 64u //Определяем размер буффера
#endif
#ifndef END_OF_STR//Признак конца строк
#define END_OF_STR -1
#endif
#ifndef END_OF_INP//Признак конца ввода
#define END_OF_INP -2
#endif

//Types
typedef char **string_list;//Массив строк

typedef struct str_lst_struct
{
    string_list array;
    int size, size_current;
}string_struct;

static jmp_buf begin;

//Functions buf
static int fill_buf(char*);//Заполняет буфер, возвращает количество считанных символов
static int clean_buf(char*);//Отчищает буфер, возвращает его размер
extern void clean_input();//Отчищает весь входной поток
extern int get_char(char*, int*);//Возвращает символ из буфера, если буфер пуст то заполняет его, если и там пусто возвращает '\0'  
//На вход принимает int, если int = 1 то отчищает буфер. 

//Functions string list
static void copy_str(char*, char*);//Копирует строку 2 в строку 1 посимвольно
extern int copy_int(char*, int);//Копирует integer 2 в строку 1 посимвольно, возвращает количество символов
extern string_struct add_string_list(string_struct, char*, int);//Добавляет строку в конец массива строк, возвращает этот массив
extern string_struct clean_string_list(string_struct);//Отчищает массив. НЕ ВЕРНО ТАК КАК ПРИСУТСВУЕТ УТЕЧКА ПАМЯТИ
extern void print_string_list(string_struct);//Выводит массив строк в stdout, через пробелы заканчивая \n
extern void sort_string_list(string_struct);//Сортирует массив строк по возрастанию методом простого выбора
extern string_struct init_string_list();//Выделяет памят под массив строк, возвращает этот массив

//Functions
extern string_struct input(string_struct);//Считывает строку, добавляет в массив сстрок, возвращает этот массив
static string_struct input_self(string_struct, char*);
static void error(int, char*, int);
static void inv();