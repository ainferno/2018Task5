#include "list.h"

//BUFFER
static int fill_buf(char *buf)
{
    return read(0,buf,buf_size);
    // return fread(buf, sizeof(char), buf_size, stdin);//Берем символы из ввода
}
extern void clean_input()
{
    char c;
    while((c = getchar()) != '\n' && c != EOF);//Пока входной поток не пуст отчищаем его
}
static int clean_buf(char *buf)
{
    int i = 0;
    for(;(i < buf_size) && (buf[i] != '\n');i++, buf[i] = '\0');//Отчищаем буффер
    return i;
}
extern int get_char(char *buf, int *i)
{
    int n = 0;
    if(*i == buf_size)
    {
        clean_buf(buf);//В данном случае отчистка буфера не имеет особого смысла
        n = fill_buf(buf);
        // printf("POINT N = %d\n", n);
        if(n == 0)
            return END_OF_INP;
        if(n < buf_size)//Если мы нажали enter/ctrl-d в конец последнего буфера попадет символ конца строки
            buf[n] = '\n';
        *i = 0;
    }
    // printf("C[%d] = %c\n", *i, buf[*i]);
    if(buf[*i] == '\n')
    {
        *i = buf_size;
        return END_OF_STR;
    }
    return (unsigned char)buf[(*i)++];//Char -> unsigned char, так как потом переводим это число в int
}//BUFFER

//STRING LIST
static void copy_str(char *a, char *b)
{
    for(;*b != '\0';a++, b++)
        *a = *b;
    *a = '\0';
}
extern int copy_int(char *a, int b)
{
    int i = 0, j = 0, k = 1;
    char c[100] = "\0";
    if(b < 0)
    {
        b = -b;
        k = -1;
    }
    if(b == 0)
    {
        a[0] = '0';
        a[1] = '\0';
        i = 1;
    }
    else
    {
        for(;b > 0;b/=10,j++)
        c[j] = b%10 + '0';
        c[j--] = '\0';
        if(k == -1)
            a[i++] = '-';
        for(;j >= 0;i++,j--)
            a[i] = c[j];
        a[i] = '\0';
    }
    return i;
}

extern string_struct add_string_list(string_struct str, char *elem, int elem_size)
{
    if(str.size_current + 1 == str.size)//Если выходим за пределы массива то перевыделяем память под него
    {
        str.size*=2;
        str.array = realloc(str.array, sizeof(char*) * str.size);
    }
    str.array[str.size_current] = (char*)malloc(sizeof(char) * elem_size);
    copy_str(str.array[str.size_current], elem);//Нельзя делать простое приравнивание так как в таком случае
    str.array[++str.size_current] = NULL;//если вне функции очистить то что передали очистим, то и тут пропадет
    return str;//А мы хотим функцию которая работает корректно не зависимо от реализации программы ее вызывающей
}
extern string_struct clean_string_list(string_struct str)
{
    for(int i = 0;i < str.size_current;i++)
        free(str.array[i]);
    free(str.array);
    str.size = array_size;
    str.size_current = 0;
    return str;
}
extern void print_string_list(string_struct str)
{
    char out_buf_int[12];
    int n = copy_int(out_buf_int,str.size_current)+1;
    out_buf_int[n-1] = '\n';
    write(1, "List size = ", 12);
    write(1,out_buf_int, n);
    for(int i = 0;i < str.size_current;i++)
    {
        int size_of_str = strlen(str.array[i])+1;//Так как мы выводим в файл, то мы используем буфер чтобы
        char out_buf[size_of_str];
        n = copy_int(out_buf_int,i+1)+1;
        out_buf_int[n-1] = ' ';
        copy_str(out_buf, str.array[i]);//Добавить символы ' '/'\n' в конец
        // out_buf[size_of_str-1] = (i == str.size_current-1) ? '\n' : ' ';
        out_buf[size_of_str-1] = '\n';
        write(1, "Elem No.", 8);
        write(1, out_buf_int, n);
        write(1, "- ", 2);
        write(1, out_buf,size_of_str);
    }
}
extern void sort_string_list(string_struct str)
{
    char *buff = NULL;
    int max = 0;

    for(int i = 0;i < str.size_current;i++)
    {
        max = i;
        for(int j = i+1;j < str.size_current;j++)
        {
            if(strcmp(str.array[max], str.array[j]) > 0)
                max = j;
        }
        if(max != i)
        {
            buff = str.array[max];
            str.array[max] = str.array[i];
            str.array[i] = buff;
        }
    }
}
extern string_struct init_string_list()
{
    string_struct str_list;
    str_list.array = (char**)malloc(10 * sizeof(char*));
    str_list.array[0] = NULL;

    str_list.size = array_size;
    str_list.size_current = 0;
    
    return str_list;
}
//STRING LIST

static void error(int c, char *buf, int i)
{
    int a = 0;
    char message[100] = "\nError!Symbol ' ' is not allowed. Please restart.\n\n";
    message[15] = (unsigned char)c;
    write(2,message,strlen(message));//Выводим сообщение об ошибке
    
    a = get_char(buf,&i);
    while(a != END_OF_STR && a != END_OF_INP)
        a = get_char(buf,&i);
    if(a == END_OF_INP)
        clean_input();
    longjmp(begin,1);//В случае ошибки перезапускаем пронрамму
}
extern void set_colour(char* colour)
{
    if(strcmp(colour, "DEFAULT") == 0)
    {
        write(1, "\033[0m", 4);
        return;
    }
    if(strcmp(colour, "WHITE") == 0)
    {
        write(1, "\033[01;37m", 8);
        return;
    }
    if(strcmp(colour, "BLUE") == 0)
    {
        write(1, "\033[22;34m", 8);
        return;
    }
    if(strcmp(colour, "GREEN") == 0)
    {
        write(1, "\033[22;32m", 8);
        return;
    }
    write(1, colour, strlen(colour));
}
static void inv()
{
    char s1[200], *s2, s3[200];
    int i = 0;

    gethostname(s1, 200);
    s2 = getenv("USER");
    getcwd(s3,200);

    for(int j = 0;j < 3;)
        if(s3[i++] == '/')
            j++;
    i--;

    set_colour("GREEN");
    write(1, s2, strlen(s2));
    write(1, "@", 1);
    write(1, s1, strlen(s1));
    set_colour("WHITE");
    write(1,":",1);
    set_colour("BLUE");
    write(1, "~", 1);
    write(1, s3+i, strlen(s3+i));
    set_colour("WHITE");
    write(1, "$ ", 2);
}
static string_struct input_self(string_struct lst, char *buf)
{
    char *str = NULL;//Текущая строка
    int c = 0;//Текущий символ
    int N = input_size, i = 0, buf_i = buf_size;//N - макс размер текущей строки, i - размер текущей строки
    str = (char*)malloc(N*sizeof(char));//Выделяем память
    inv();
    c = get_char(buf,&buf_i);
    if(c == END_OF_INP)
    {
        free(str);
        raise(SIGUSR1);
        return lst;
        // return NULL;
    }
    while(c != END_OF_STR && c != END_OF_INP)
    {
        if(i == N)//Если считанная строка не укладывается в нашу строку то перевыделяем память увеличивая в 2 раза место
        {
            N*=2;
            str = (char*)realloc(str, N*sizeof(char));
        }
        switch(c)
        {
            case '|': case '&': case '>'://Если текущее слово не определенно однозначно тоесть | или ||, & или && и.т.д.
                if(i!=0)//Если текущее слово не пустое, то вначале добавляем его в массив
                {
                    str[i] = '\0';
                    lst = add_string_list(lst,str,i+1);
                    if(N >= 1000)
                    {
                        N = input_size;
                        str = (char*)realloc(str, N*sizeof(char));
                    }
                    else
                        str[0] = '\0';
                    i = 0;
                }
                while(c == '|' || c == '&' || c == '>')//Обрабатываем весь набор особыз спецсимволов | & > за раз, пока не встретим обычное слово
                {//Или не повторяющееся
                    str[i++] = c;
                    c = get_char(buf,&buf_i);
                    if(c == str[i-1])//Если символ повторятся т.е. мы имеем && || >>
                    {//Добавляем его в слово
                        str[i++] = c;
                        c = get_char(buf,&buf_i);
                    }
                    str[i] = '\0';//Записываем это слово в массив
                    lst = add_string_list(lst,str,i+1);
                     if(N >= 1000)
                    {
                        N = input_size;
                        str = (char*)realloc(str, N*sizeof(char));
                    }
                    else
                        str[0] = '\0';
                    i = 0;
                }
                break;
            case ';': case '<': case '(': case ')':
                if(i!=0)//Если слово не пустое, то добавляем его в массив
                {
                    str[i] = '\0';
                    lst = add_string_list(lst,str,i+1);
                    if(N >= 1000)
                    {
                        N = input_size;
                        str = (char*)realloc(str, N*sizeof(char));
                    }
                    else
                        str[0] = '\0';
                    i = 0;
                }
                str[i++] = c;//Записываем наше спецслово в строку, а дальше аналогично case ' ':...
            case ' ': case '\t': case '\n':
                if(i!=0)
                {//Если слово не пустое(т.е. мы не смотрим на посл. пробелов) то добавляем его в массив
                    str[i] = '\0';
                    lst = add_string_list(lst,str,i+1);
                 if(N >= 1000)
                    {
                        N = input_size;
                        str = (char*)realloc(str, N*sizeof(char));
                    }
                    else
                        str[0] = '\0';
                    i = 0;
                }
        }//Слова состоят из символов+букв+набора символов '&' '_' '/ '.'
        if( c >= '0' && c <= '9' ||
            c >= 'a' && c <= 'z' ||
            c >= 'A' && c <= 'Z' ||
            c == '&' || c == '_' || c == '/' || c == '.' || c == '-' || c == '$')
        {
            str[i++] = c;
            c = get_char(buf,&buf_i);
        }
        else
        {
            switch(c)
            {
                case ' ': case '\n': case '\t': 
                case ';': case '<': case '(': case ')': 
                case '&': case '|': case '>':
                    c = get_char(buf,&buf_i);
                case -1: case -2:
                    break;
                default:
                    lst = clean_string_list(lst);
                    free(str);
                    error(c, buf, buf_i);
            }
        }
    }
    if(i!=0)
    {//Если слово не пустое(т.е. мы не смотрим на посл. пробелов) то добавляем его в массив
        str[i] = '\0';
        lst = add_string_list(lst,str,i+1);
    }

    free(str);//Отчистка памяти выделенной под строку
    return lst;
}

extern string_struct input(string_struct lst)
{
    char buf[buf_size] = "\0";
    setjmp(begin);
    lst = input_self(lst, buf);//Вводим слова в массив
    set_colour("DEFAULT");
    return lst;
}