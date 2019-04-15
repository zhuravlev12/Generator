#include <stdio.h>
#include <stdlib.h>

char * generate(int k, int space, int n) {
	char * input = (char*)malloc((k + 1) * sizeof(char));
	char * output = (char*)malloc((n + 1) * sizeof(char));
	output[n] = '\0';
	char next = 0;
	for (int i = 0; i < k; i++) {
		char rnd = (rand() % 2);
		next ^= rnd;
		input[i] = rnd;
	}
	for (int i = 0; i < n; i += k) {
		int j, length = (i + k < n) ? k : (n - i);
		for (j = 0; j < length; j++) {
			output[i + j] = '0' + next;
			char temp = input[j];
			if ((i + j) % space) {
				input[j] = next;
				next ^= temp;
			}
			else {
				char rnd = (rand() % 2);
				input[j] = rnd;
				next ^= rnd;
			}
		}
	}
	free(input);
	return output;
};

int main(void) {
	int k;
	int space;
	int n;
	printf("Enter length of begin generator\n");
	scanf("%d", &k);
	printf("Enter length of spaces\n");
	scanf("%d", &space);
	printf("Enter length of output\n");
	scanf("%d", &n);
	char * result = generate(k, space, n);
	printf("%s", result);
	free(result);
	return 0;
}