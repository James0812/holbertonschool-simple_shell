#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

extern char **environ;
int main(void)

{
char *line = NULL;
size_t len = 0;
ssize_t nread;
pid_t pid;

while (1)
{
if (isatty(STDIN_FILENO))
write(STDOUT_FILENO, "$ ", 2);
nread = getline(&line, &len, stdin);
if (nread == -1)
break;
if (line[nread - 1] == '\n')
line [nread -1] = '\0';
if (line[0] == '\0')
continue;
if (strcmp(line, "exit") == 0)
break;
if (strcmp(line, "env") == 0)
{
int i = 0;
while (environ[i] != NULL)
{
printf("%s\n", environ[i]);
i++;
}
continue;
}
if (pid == 0)
{
char *args[2]; 
args[0] = line; 
args[1] = NULL;
execve(line, args, environ);
perror("execve failed");
exit(EXIT_FAILURE);
}
else if (pid > 0)

wait(NULL);
else
perror("fork failed");

free(line);
}
return (0);
}

