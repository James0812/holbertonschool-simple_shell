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
	char *args[2];

	while (1)
	{
		/* Display prompt only in interactive mode */
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "($) ", 4);

		/* Read input */
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

		/* Fork process */
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free(line);
			exit(EXIT_FAILURE);
		}

		if (pid == 0)
		{
			/* Child process */
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
			/* Parent process */
			waitpid(pid, &status, 0);
		}
	}

	free(line);
	return (0);
}

