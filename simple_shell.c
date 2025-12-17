#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

/**
 * find_command_in_path - Searches for command in PATH directories
 * @cmd: command name
 * Return: full path if found, else NULL
 */
char *find_command_in_path(char *cmd)
{
    char *path_env, *path_copy, *dir, *full_path;
    struct stat st;

    if (strchr(cmd, '/'))
    {
        if (stat(cmd, &st) == 0)
            return strdup(cmd);
        return NULL;
    }

    path_env = NULL;
    {
        int i = 0;
        while (environ[i])
        {
            if (strncmp(environ[i], "PATH=", 5) == 0)
            {
                path_env = environ[i] + 5;
                break;
            }
            i++;
        }
    }

    if (!path_env || path_env[0] == '\0')
        return NULL;

    path_copy = strdup(path_env);
    if (!path_copy)
        return NULL;

    dir = strtok(path_copy, ":");
    while (dir)
    {
        full_path = malloc(strlen(dir) + strlen(cmd) + 2);
        if (!full_path)
        {
            free(path_copy);
            return NULL;
        }

        sprintf(full_path, "%s/%s", dir, cmd);
        if (stat(full_path, &st) == 0)
        {
            free(path_copy);
            return full_path;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

/**
 * main - Simple UNIX command interpreter (0.4)
 * Return: 0 on success
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    pid_t pid;
    int status = 0;

    while (1)
    {
        char *tokens[100];
        int i;
        char *cmd_path;

        if (isatty(STDIN_FILENO))
        {
            write(STDOUT_FILENO, "($) ", 4);
        }

        nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            free(line);
            exit(status);
        }

        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        /* Skip empty lines */
        {
            int only_spaces = 1;
            for (i = 0; line[i]; i++)
            {
                if (line[i] != ' ' && line[i] != '\t')
                {
                    only_spaces = 0;
                    break;
                }
            }
            if (only_spaces)
                continue;
        }

        /* Tokenize */
        i = 0;
        tokens[i] = strtok(line, " \t");
        while (tokens[i] && i < 99)
        {
            i++;
            tokens[i] = strtok(NULL, " \t");
        }

        if (!tokens[0])
            continue;

        /* Built-in: exit */
        if (strcmp(tokens[0], "exit") == 0)
        {
            free(line);
            exit(status);
        }

        /* Built-in: env */
        if (strcmp(tokens[0], "env") == 0)
        {
            i = 0;
            while (environ[i])
            {
                write(STDOUT_FILENO, environ[i], strlen(environ[i]));
                write(STDOUT_FILENO, "\n", 1);
                i++;
            }
            status = 0;
            continue;
        }

        /* PATH resolution */
        cmd_path = find_command_in_path(tokens[0]);
        if (!cmd_path)
        {
            write(STDERR_FILENO, tokens[0], strlen(tokens[0]));
            write(STDERR_FILENO, ": not found\n", 12);
            status = 127;
            continue;
        }

        pid = fork();
        if (pid == -1)
        {
            perror("fork");
            free(cmd_path);
            free(line);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execve(cmd_path, tokens, environ);
            perror(cmd_path);
            free(cmd_path);
            exit(EXIT_FAILURE);
        }
        else
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                status = WEXITSTATUS(status);
        }

        free(cmd_path);
    }

    free(line);
    return (0);
}

