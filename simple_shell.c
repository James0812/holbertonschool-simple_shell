#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

/**
 * main - Simple UNIX command interpreter
 *
 * Return: 0 on success
 */
int main(void)
{
    char *line = NULL;
    char *args[2];
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        if (isatty(STDIN_FILENO))
            printf("($) ");

        nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            printf("\n");
            free(line);
            exit(EXIT_SUCCESS);
        }

        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(line);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            args[0] = line;
            args[1] = NULL;

            if (execve(line, args, environ) == -1)
            {
                perror(line);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            waitpid(pid, &status, 0);
        }
    }

    free(line);
    return (0);
}

