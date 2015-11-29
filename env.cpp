#include <cstdio>
#include <cstdlib>

int main(int, char**){
	ssize_t read;
	char* line = NULL;
	size_t len = 0;
	while((read = getdelim(&line, &len, '\0', stdin)) != -1){
		puts(line);
	}
	free(line);
	return 0;
}
