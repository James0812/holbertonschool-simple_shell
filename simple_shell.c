#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_TOKENS 64
#define MAX_PATH 1024

/**
 * get_path - Return the value of PATH from environ
 *
 * Return: PATH string or NULL
 */
char *get_path(void)
{
    int i = 0;

    while (environ[i])
    {
        if (strncmp(environ[i], "PATH=", 5) == 0)
            return (environ[i] + 5);
        i++;
    }
    return NULL;
}

/**
 * find_command - Find full path of a command using PATH
 * @command: command name
 *
 * Return: full path if found, NULL otherwise
 */
char *find_command(char *command)
{
    char *path, *path_copy, *dir;
    static char full_path[MAX_PATH];

    if (command == NULL)
        return NULL;

    /* If command already contains '/' */
    if (strchr(command, '/') != NULL)
    {
        if (access(command, X_OK) == 0)
            return command;
        return NULL;
    }

    path = get_path();
    if (path == NULL || path[0] == '\0') /* PATH empty */
        return NULL;

    path_copy = strdup(path);
    if (path_copy == NULL)
        return NULL;

    dir = strtok(path_copy, ":");
    while (dir != NULL)
    {
        snprintf(full_path, MAX_PATH, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0)
        {
            free(path_copy);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

/**
 * main - Simple shell
 *
 * Return: exit status
 */
int main(void)
{
    char *line = NULL, *args[MAX_TOKENS];
    size_t len = 0;
    ssize_t read;
    pid_t pid;
    int status;
    char *cmd_path;
    int i;
    int interactive;
    int line_count = 0;
    int exit_status = 0;

    interactive = isatty(STDIN_FILENO);

    while (1)
    {
        line_count++;

        if (interactive)
            write(STDOUT_FILENO, "$ ", 2);

        read = getline(&line, &len, stdin);
        if (read == -1)
        {
            if (interactive)
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Tokenization */
        i = 0;
        args[i] = strtok(line, " ");
        while (args[i] != NULL && i < MAX_TOKENS - 1)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;

        /* Empty input */
        if (args[0] == NULL)
            continue;

        cmd_path = find_command(args[0]);
        if (cmd_path == NULL)
        {
            char msg[128];
            int len_msg;

            len_msg = snprintf(msg, sizeof(msg),
                               "./hsh: %d: %s: not found\n",
                               line_count, args[0]);
            write(STDERR_FILENO, msg, len_msg);
            exit_status = 127;  /* code de sortie correct */
            continue;
        }

        pid = fork();
        if (pid == 0)
        {
            if (execve(cmd_path, args, environ) == -1)
                exit(127);
        }
        else if (pid > 0)
        {
            wait(&status);
            if (WIFEXITED(status))
                exit_status = WEXITSTATUS(status);
        }
    }

    free(line);
    return exit_status;
}

