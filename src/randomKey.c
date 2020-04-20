#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int rdmstr(char *dest, size_t length) {

    FILE *fs;
    if (!(fs = fopen("/dev/random", "r")))
		return -1;

	char charset[] = "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	while (length-- > 0) {
        printf("%d", fgetc(fs));
        size_t index = (double) fgetc(fs) / 255 * (sizeof charset - 1);
        *dest++ = charset[index];
    }

	// Add null byte to end
    *dest = '\0';

    fclose(fs);

}
