#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int rdmstr(char *dest, size_t length) {

    FILE *fs;
    if (!(fs = fopen("/dev/urandom", "r")))
		return -1;

	char charset[] = "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	while (length-- > 0) {
        size_t index = fgetc(fs) % (sizeof(charset) - 1);
        *dest++ = charset[index];
    }

	// Add null byte to end
    *dest = '\0';

    fclose(fs);

    return 0;

}
