#include <stdio.h>
#include <stdlib.h>

int read_file(const char *path, char **contents) {
	FILE* file = fopen(path, "r");

	if (file == NULL) {
		return -1;
	}

	if (fseek(file, 0, SEEK_END) < 0) {
		return -1;
	}

	size_t file_size = ftell(file);
	rewind(file);

	char *buffer = malloc(file_size);
	size_t n_read = fread(buffer, 1, file_size, file);
	if (n_read < file_size) {
		return -1;
	}

	fclose(file);
	*contents = buffer;

	return (int) n_read;
}
