#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;

/**
 * get_path_from_env - get PATH value from environ
 *
 * Return: PATH string or NULL
 */
char *get_path_from_env(void)
{
	int i = 0;

	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_in_path - find command in PATH directories
 * @command: command name
 *
 * Return: full path to command or NULL
 */
char *find_in_path(char *command)
{
	char *path, *path_copy, *dir;
	static char full_path[1024];

	if (command == NULL)
		return (NULL);

	/* If command already contains a path */
	if (strchr(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
			return (command);
		return (NULL);
	}

	path = get_path_from_env();
	if (path == NULL)
		return (NULL);

	path_copy = strdup(path);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
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
 * main - Simple UNIX command interpreter
 *
 * Return: 0 on success
 */
int main(void)
{
	char *line = NULL, *cmd_path;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status;
	char *tokens[100];
	int i, only_spaces;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("($) "), fflush(stdout);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			if (isatty(STDIN_FILENO))
				printf("\n");
			free(line);
			exit(EXIT_SUCCESS);
		}

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		/* Check for empty or spaces-only input */
		only_spaces = 1;
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

		/* Tokenize input */
		i = 0;
		tokens[i] = strtok(line, " \t");
		while (tokens[i] && i < 99)
		{
			i++;
			tokens[i] = strtok(NULL, " \t");
		}

		cmd_path = find_in_path(tokens[0]);
		if (cmd_path == NULL)
		{
			fprintf(stderr, "command not found\n");
			continue;
		}

		/* Fork only if command exists */
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free(line);
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			if (execve(cmd_path, tokens, environ) == -1)
			{
				perror(tokens[0]);
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

