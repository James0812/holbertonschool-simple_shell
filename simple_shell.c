#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

/**
 * get_path_value - Get PATH value from environ
 * Return: pointer to PATH value or NULL
 */
char *get_path_value(void)
{
	int i;

	for (i = 0; environ[i] != NULL; i++)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
	}
	return (NULL);
}

/**
 * find_command - Find command in PATH
 * @cmd: command name
 * Return: full path or NULL
 */
char *find_command(char *cmd)
{
	char *path, *path_copy, *dir;
	char full_path[1024];

	if (cmd[0] == '/' || cmd[0] == '.')
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}

	path = get_path_value();
	if (path == NULL || *path == '\0')
		return (NULL);

	path_copy = strdup(path);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (strdup(full_path));
		}
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * main - Simple UNIX command interpreter
 * Return: status
 */
int main(void)
{
	char *line = NULL, *tokens[100], *cmd_path;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status = 0, i;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("($) "), fflush(stdout);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			free(line);
			exit(status);
		}

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		/* Skip empty or space-only lines */
		for (i = 0; line[i] == ' ' || line[i] == '\t'; i++)
			;
		if (line[i] == '\0')
			continue;

		/* Tokenize input */
		i = 0;
		tokens[i] = strtok(line, " \t");
		while (tokens[i] != NULL && i < 99)
		{
			i++;
			tokens[i] = strtok(NULL, " \t");
		}

		/* Built-in: exit */
		if (strcmp(tokens[0], "exit") == 0)
		{
			free(line);
			exit(status);
		}

		cmd_path = find_command(tokens[0]);
		if (cmd_path == NULL)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", tokens[0]);
			status = 127;
			continue;
		}

		pid = fork();
		if (pid == 0)
		{
			execve(cmd_path, tokens, environ);
			exit(1);
		}
		else
		{
			waitpid(pid, &status, 0);
			free(cmd_path);
		}
	}

	free(line);
	return (status);
}

