#include <stdio.h>

int main() {
	char name[BUFSIZ];
	printf("What is your name?\n");
	scanf("%s", name);
	printf("Hello %s! How are you doing?\n", name);
	return 0;
}
