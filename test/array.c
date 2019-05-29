int main() {
	// Allocate a 2-dimensional array.
	int** array = malloc(8 * sizeof(int*));
	int i = 0;
	while (i < 8) {
		array[i] = malloc(8 * sizeof(int));
		i += 1;
	}

	// Assign data to the 2-dimensional array.
	i = 0;
	while (i < 8) {
		int j = 0;
		while (j < 8) {
			array[i][j] = 'A' + i + j;
			j += 1;
		}
		i += 1;
	}

	// Print the 2-dimensional array.
	i = 0;
	while (i < 8) {
		int j = 0;
		while (j < 8) {
			putchar(array[i][j]);
			j += 1;
		}
		putchar('\n');
		i += 1;
	}

	return 69;
}