#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

/**
 * trim - Removes leading and trailing spaces from a string
 * @str: String to process
 *
 * Return: Pointer to the trimmed string
 */
char *trim(char *str)
{
    char *end;

    /* Trim leading spaces */
    while (*str == ' ' || *str == '\t')
        str++;

    if (*str == 0) /* empty string */
        return str;

    /* Trim trailing spaces */
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t'))
        end--;

    *(end + 1) = '\0';
    return str;
}

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
    char *args[2];

    while (1)
    {
        /* Display prompt only in interactive mode */
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 4);

        /* Read command line */
        nread = getline(&line, &len, stdin);
        if (nread == -1) /* EOF (Ctrl+D) */
        {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            free(line);
            exit(EXIT_SUCCESS);
        }

        /* Remove newline */
        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Trim spaces */
        args[0] = trim(line);
        args[1] = NULL;

        if (args[0][0] == '\0') /* empty line */
            continue;

        /* Fork child process */
        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(line);
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            /* Child executes the command */
            if (execve(args[0], args, environ) == -1)
            {
                perror(args[0]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent waits for child to finish */
            waitpid(pid, &status, 0);
        }
    }

    free(line);
    return (0);
}

