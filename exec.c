#include "exec.h"

void printTable(struct cell* table)
{
    if (table == NULL)
        return;
    printf("%d", table->pid);
    printTable(table->next);
}
string_struct correct(string_struct list)
{
    char * str;
    if (!list.size_current)
        return list;
    for(int i = 0;i < list.size_current;i++)
    {
        if (list.array[i][0]=='$')
        {
            if (!strcmp(list.array[i], "$HOME"))
                str = getenv("HOME");
            else if (!strcmp(list.array[i], "$SHELL"))
                str = getenv("SHELL");
            else if (!strcmp(list.array[i], "$USER"))
                str = getenv("USER");
            else continue;
            list.array[i] = realloc(list.array[i],sizeof(char)*strlen(str));
            strcpy(list.array[i],str);
        }
    }
    return list;
}
struct cell* Clean_Table(struct cell* Table)
{
    int rez, status;
    struct cell* End;
    if (Table == NULL)
        return NULL;
    if ((rez = waitpid(Table->pid, &status,WNOHANG)) > 0)
    {
        printf("Process with PID: %d end!", Table->pid);
        End = Table;
        Table = Clean_Table(Table->next);
        free(End);
    }
    else 
        Table->next = Clean_Table(Table->next);
    return Table;
}
struct cell* Add_Table(int pid, struct cell* Table)
{
    if (Table == NULL)
    {
        Table = malloc(sizeof(struct cell));
        Table->pid = pid;
        printf("Started %d\n",Table->pid);
        Table->next = NULL;
        return Table;
    }
    Table->next = Add_Table(pid,Table->next);
    return Table;
}

void runexec(tree root, struct cell* MainTable)
{
    int pid_f, fd[2], fio;//file-input-output
    if(root == NULL)
        return;
    if(root->pipe != NULL)
        pipe(fd);
    if((pid_f = fork())==0)
    {
        if(root->backgrnd == 0)
            signal(SIGINT, SIG_DFL);
        if(root->pipe != NULL)
        {
            close(fd[0]);
            dup2(fd[1],1);
            close(fd[1]);
        }
        if(root->infile != NULL)
        {
            fio = open(root->infile, O_RDONLY);
            dup2(fio, 0);
            close(fio);
        }
        if(root->outfile != NULL)
        {
            fio = open(root->outfile, O_WRONLY|O_CREAT, 0666);
            dup2(fio, 1);
            close(fio);
        }
        if (!strcmp(root->argv[0],"exit"))
        {
            raise(SIGUSR1);
            exit(0);
        }
        if (!strcmp(root->argv[0],"cd"))//Обработка cd
        {
            if (root->argv[1] == NULL)
                chdir(getenv("HOME"));
            else chdir(root->argv[1]);
            exit(0);
        }
        
        if (root->psubcmd == NULL)//End
        {
            execvp((root->argv)[0],root->argv);
            exit(-1);
        }
        else
        { 
            runexec(root->psubcmd, MainTable);
            exit(0);
        }
    }
    if (root->backgrnd == 0)
        waitpid(pid_f,NULL,0);
    else 
        MainTable = Add_Table(pid_f,MainTable);
    if (root->next != NULL) 
        runexec(root->next, MainTable);
    if (root->pipe != NULL)
    {
        close(fd[1]);
        fio = dup(0);
        dup2(fd[0],0);
        close(fd[0]);
        runexec(root->pipe, MainTable);
        dup2(fio,0);
    }
    return;
}
