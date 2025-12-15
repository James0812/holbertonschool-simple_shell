#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/**
 * main - Simple UNIX command interpreter
 *
 * Return: 0 on success
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        /* Print prompt */
        if (isatty(STDIN_FILENO))
            printf("($) ");

        /* Read input line */
        nread = getline(&line, &len, stdin);
        if (nread == -1) /* Ctrl+D */
        {
            printf("\n");
            free(line);
            exit(EXIT_SUCCESS);
        }

        /* Remove newline */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Fork a child process */
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(line);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            /* Child executes the command */
            char *args[] = {line, NULL};
            if (execve(line, args, NULL) == -1)
            {
                perror(line);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent waits for child */
            waitpid(pid, &status, 0);
        }
    }

    free(line);
    return (0);
}

