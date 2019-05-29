int* gets() {
	int* str = malloc(256 * sizeof(int));
	int len = 0;
	while ((str[len] = getchar()) != '\n') {
		len += 1;
	}
	return str;
}

int puts(int* str) {
	while (*str) {
		putchar(*str);
		str += 1;
	}
	return 0;
}

int main() {
	while (1) {
		int* str = gets();
		puts(str);
	}
	return 0;
}