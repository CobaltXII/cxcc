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

int main() {
	int seconds = time(0);
	int minutes = seconds / 60;
	int hours = minutes / 60;
	int days = hours / 24;
	int years = days / 365;
	puts("Seconds since Epoch: ");
	puti(seconds);
	putchar('\n');
	puts("Minutes since Epoch: ");
	puti(minutes);
	putchar('\n');
	puts("Hours since Epoch: ");
	puti(hours);
	putchar('\n');
	puts("Days since Epoch: ");
	puti(days);
	putchar('\n');
	puts("Years since Epoch: ");
	puti(years);
	putchar('\n');
	return 0;
}