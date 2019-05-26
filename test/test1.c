int puts(int* str) {
	while (*str) {
		putchar(*str);
		str = str + 8;
	}
	return 0;
}

int main() {
	puts("Hello, world!\n");
	return 0;
}