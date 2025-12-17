#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

#define MAX_TOKENS 64
#define MAX_PATH 1024

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
		return (NULL);

	/* If command already contains '/' */
	if (strchr(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
			return (command);
		return (NULL);
	}

	path = getenv("PATH");
	if (path == NULL)
		return (NULL);

	path_copy = strdup(path);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		snprintf(full_path, MAX_PATH, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * main - Simple shell
 *
 * Return: Always 0
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

	while (1)
	{
		write(STDOUT_FILENO, "$ ", 2);

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
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
			write(STDERR_FILENO, "command not found\n", 18);
			continue;
		}

		pid = fork();
		if (pid == 0)
		{
			if (execve(cmd_path, args, environ) == -1)
				exit(EXIT_FAILURE);
		}
		else if (pid > 0)
		{
			wait(&status);
		}
	}

	free(line);
	return (0);
}

