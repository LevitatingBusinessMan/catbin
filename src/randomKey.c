#include <stdlib.h>
#include <string.h>

void rdmstr(char *dest, size_t length) {
	char charset[] = "abcdefghijklmnopqrstuvwxyz" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }

	// Add null byte to end
    *dest = '\0';
}
