int puts(int* str) {
	while (*str) {
		putchar(*str);
		str += 1;
	}
	return 0;
}

int puti(int n) {
	if (n < 0) {
		putchar('-');
		n = -n;
	}
	if (n / 10) {
		puti(n / 10);
	}
	return putchar(n % 10 + '0');
}

int putn() {
	return putchar('\n');
}

int putsin(int* str, int n) {
	puts(str);
	puti(n);
	return putn();
}

int a(int x) {
	return x * x - x;
}

int b(int x, int y) {
	return a(x) + a(y);
}

int main() {
	int c = b(13, 27);
	putsin("a(13) == ", a(13));
	putsin("a(27) == ", a(27));
	return putsin("b(13, 27) == ", c);
}