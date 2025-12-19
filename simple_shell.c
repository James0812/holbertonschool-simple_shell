#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

/**
 * find_in_path - find command in PATH
 * @cmd: command name
 *
 * Return: full path or NULL
 */
char *find_in_path(char *cmd)
{
	char *path, *path_copy, *dir;
	static char full_path[1024];
	struct stat st;
	int i;

	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
		{
			path = environ[i] + 5;
			break;
		}
	}
	if (!environ[i] || !path || path[0] == '\0')
		return (NULL);

	path_copy = strdup(path);
	if (!path_copy)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
		if (stat(full_path, &st) == 0 && (st.st_mode & S_IXUSR))
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
int main(int argc, char **argv)
{
	char *line = NULL, *cmd_path;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status, last_status = 0;
	unsigned int line_number = 0;
	char *tokens[100];
	int i;

	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("($) "), fflush(stdout);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			free(line);
			exit(last_status);
		}

		line_number++;

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		i = 0;
		tokens[i] = strtok(line, " \t");
		while (tokens[i] && i < 99)
			tokens[++i] = strtok(NULL, " \t");

		if (!tokens[0])
			continue;

		/* exit built-in */
		if (strcmp(tokens[0], "exit") == 0)
		{
			free(line);
			exit(last_status);
		}

		/* env built-in */
		if (strcmp(tokens[0], "env") == 0)
		{
			for (i = 0; environ[i]; i++)
				printf("%s\n", environ[i]);
			last_status = 0;
			continue;
		}

		/* absolute or relative path */
		if (tokens[0][0] == '/' || tokens[0][0] == '.')
			cmd_path = tokens[0];
		else
			cmd_path = find_in_path(tokens[0]);

		/* command not found → NO fork */
		if (!cmd_path)
		{
			dprintf(STDERR_FILENO, "%s: %u: %s: not found\n",
				argv[0], line_number, tokens[0]);
			last_status = 127;
			continue;
		}

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
				perror(argv[0]);
				exit(2);
			}
		}
		else
		{
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
		}
	}
}

