#include "shell.h"

/**
 * find_command - finds a command in PATH
 * @command: command entered by user
 *
 * Return: full path of command or NULL
 */
char *find_command(char *command)
{
	char *path, *path_copy, *token, *file_path;
	struct stat st;

	path = getenv("PATH");
	if (!path)
		return (NULL);

	path_copy = strdup(path);
	token = strtok(path_copy, ":");

	while (token)
	{
		file_path = malloc(strlen(token) + strlen(command) + 2);
		if (!file_path)
			return (NULL);

		sprintf(file_path, "%s/%s", token, command);

		if (stat(file_path, &st) == 0 && (st.st_mode & S_IXUSR))
		{
			free(path_copy);
			return (file_path);
		}

		free(file_path);
		token = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

