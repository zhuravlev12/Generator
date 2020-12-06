#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "MyGenerator.h"

void help() {
	printf("Instruction of using this program:\n\
[-h] - help\n\
[-b] - parameter for printing bits instead of bytes\n\
[-s%%NUMBER%%] - seed as integer number\n\
[-i%%FILENAME%%] - input file with seed\n\
[-o%%FILENAME%%] - output file to print output generated sequence\n\
-l%%NUMBER%% - length of output sequence (in bytes)\n\
[-n%%NUMBER%%] - number of steps, if you want to use default parameters (maximum 4)\n\
after these params you can insert params for steps of PRNG (in bits), if you weren't used -n param");
}

unsigned char ask() {
	unsigned char c = 0;
	while (c != 'y' && c != 'n') {
		c = tolower(getchar());
	}
	getchar();
	return c == 'y' ? -1 : 0;
}

int main(int argc, char ** argv) {
	FILE* input = NULL;
	FILE* output = stdout;
	uint32_t length = 0;
	uint32_t params_length;
	uint32_t * params = NULL;
	uint32_t number_of_steps = -1;
	uint32_t seed_number = 0;
	unsigned char print_bits = 0;
	if (argc < 2) {
		printf("Welcome to console random number generator with proved asymptotic properties!\n");
		printf("Do you want to print bits instead of bytes? (y/n)\n");
		print_bits = ask();
		printf("Do you want to put output sequence in file? (y/n)\n");
		if (ask()) {
			unsigned char buffer[256] = { 0 };
			printf("Enter path to output file: ");
			fgets(buffer, 255, stdin);
			buffer[strlen(buffer) - 1] = '\0';
			output = fopen(buffer, "wb");
			if (output == NULL) {
				printf("Error in opening output file");
				if (input != NULL) {
					fclose(input);
				}
				return -1;
			}
		}
		printf("Enter length of output sequence in bytes: ");
		scanf("%d", &length);
		printf("Do you want to use default parameters? (y/n)\n");
		if (ask()) {
			printf("Enter number of steps (maximum 4): ");
			scanf("%d", &number_of_steps);
		} else {
			printf("Do you want to use file for seed? (y/n)\n");
			if (ask()) {
				unsigned char buffer[256] = { 0 };
				printf("Enter path to seed file: ");
				fgets(buffer, 255, stdin);
				buffer[strlen(buffer) - 1] = '\0';
				input = fopen(buffer, "rb");
				if (input == NULL) {
					printf("Error in opening input file");
					return -1;
				}
			} else {
				printf("Do you want to enter seed as integer number? (y/n)\n");
				if (ask()) {
					printf("Enter seed as integer number: ");
					scanf("%d", &seed_number);
				}
			}
			printf("Enter number of steps: ");
			scanf("%d", &params_length);
			params = (uint32_t*)malloc(params_length * sizeof(uint32_t));
			printf("Enter parameters:\n");
			for (uint32_t i = 0; i < params_length; i++) {
				scanf("%d", &(params[i]));
			}
		}
	} else {
		for (uint32_t i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				switch (argv[i][1]) {
				case 'h': {
					help();
					return 0;
				}
				case 'b': {
					print_bits = -1;
					continue;
				}
				case 's': {
					seed_number = atoi(&(argv[i][2]));
					continue;
				}
				case 'i': {
					input = fopen(&(argv[i][2]), "rb");
					if (input == NULL) {
						printf("Error in opening input file");
						if (output != stdout) {
							fclose(output);
						}
						return -1;
					}
					continue;
				}
				case 'o': {
					output = fopen(&(argv[i][2]), "wb");
					if (output == NULL) {
						printf("Error in opening output file");
						if (input != NULL) {
							fclose(input);
						}
						return -1;
					}
					continue;
				}
				case 'l': {
					length = atoi(&(argv[i][2]));
					continue;
				}
				case 'n': {
					number_of_steps = atoi(&(argv[i][2]));
					continue;
				}
				default: {
					printf("Wrong parameter: %s", argv[i]);
					if (input != NULL) {
						fclose(input);
					}
					if (output != stdout) {
						fclose(output);
					}
					return -1;
				}
				}
			} else {
				if (params == NULL) {
					params_length = argc - i;
					params = (uint32_t*)malloc(params_length * sizeof(uint32_t));
				}
				params[i - (argc - params_length)] = atoi(argv[i]);
			}
		}
	}
	MyGeneratorInitStruct * init = (MyGeneratorInitStruct*)calloc(1, sizeof(MyGeneratorInitStruct));
	if (number_of_steps != -1) {
		init->params_length = number_of_steps;
	} else {
		init->params_length = params_length;
		init->params = params;
		if (input != NULL) {
			fseek(input, 0, SEEK_END);
			size_t file_length = ftell(input);
			uint32_t* seed = (uint32_t*)malloc(file_length);
			fread(seed, 1, file_length, input);
			fclose(input);
			init->seed = seed;
		} else {
			if (seed_number != 0) {
				uint32_t* seed = (uint32_t*)malloc(sizeof(uint32_t));
				seed[0] = seed_number;
				params[0] = 1;				
				init->seed = seed;
			}
		}
	}
	init->iterative_mode = 1;
	mysrand(*init);
	if (init->seed != NULL) {
		free(init->seed);
	}
	free(init);
	for (uint32_t i = 0; i < length; i += sizeof(uint32_t)) {
		uint32_t random = myrand();
		if (print_bits == 0) {
			fwrite(&random, sizeof(uint32_t), 1, output);
		} else {
			for (uint32_t j = 0; j < sizeof(uint32_t) * 8; j++) {
				unsigned char bit = '0' + ((random >> (sizeof(uint32_t) * 8 - j - 1)) & 1);
				fwrite(&bit, 1, 1, output);
			}
		}
	}
	if (output != NULL) {
		fclose(output);
	}
	if (params != NULL) {
		free(params);
	}
	return 0;
}
