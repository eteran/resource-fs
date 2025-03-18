
#include <stdio.h>

int main(void) {

	FILE *f = fopen("res:/alternate_name.md", "rb");
	if (f) {
		char buf[100];
		size_t read = fread(buf, 1, sizeof(buf), f);
		if (read > 0) {
			printf("Read %zu bytes: %s\n", read, buf);
		}
		fclose(f);
	} else {
		printf("Failed to open file\n");
	}

	return 0;
}
