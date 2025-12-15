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
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status;

    while (1)
    {
        char *tokens[100];
        int i;

        /* Print prompt only if input is from terminal */
        if (isatty(STDIN_FILENO))
            printf("($) "), fflush(stdout);

        /* Read input line */
        nread = getline(&line, &len, stdin);
        if (nread == -1) /* Ctrl+D or EOF */
        {
            if (isatty(STDIN_FILENO))
                printf("\n");
            free(line);
            exit(EXIT_SUCCESS);
        }

        /* Remove newline at the end */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Skip empty lines or lines with only spaces/tabs */
        {
            int all_spaces = 1;
            for (i = 0; line[i] != '\0'; i++)
            {
                if (line[i] != ' ' && line[i] != '\t')
                {
                    all_spaces = 0;
                    break;
                }
            }
            if (all_spaces)
                continue;
        }

        /* Split the line into tokens */
        i = 0;
        tokens[i] = strtok(line, " \t");
        while (tokens[i] != NULL && i < 99)
        {
            i++;
            tokens[i] = strtok(NULL, " \t");
        }

        /* Fork child process */
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(line);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            /* Child executes command */
            if (execve(tokens[0], tokens, environ) == -1)
            {
                perror(tokens[0]);
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

