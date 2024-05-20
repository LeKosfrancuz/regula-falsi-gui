#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define DEFAULT_OUT "logs.txt"
#include "mathomatic/lib/mathomatic.h"
#include "mathomatic/includes.h"

float run_function(char* func, float val);
void regula_falsi(float *x_next, float x0, float x1, float fx0, float fx1, size_t *iter);
float nultocka(char *func, float x0, float x1, float eps, size_t max_iter);

void exit_program(int exit_val) { exit(exit_val); }
void usage(FILE *fp) { (void)fp; return; }

int main(int argc, char** argv) {

	// run_function("144x^2-24x+1", 5);
	// run_function("10x", 2.53);
	// run_function("7x", 2.53);
	FILE *logs = fopen(DEFAULT_OUT, "w");
	gfp = logs;

	size_t buff_size = 100;
	char *func = malloc(buff_size);
	printf("Upiši funkciju: ");
	getline(&func, &buff_size, stdin);
	nultocka(func, 0, 1, 1e-3, 100);

	fclose(logs);

	exit(EXIT_SUCCESS);
}


float run_function(char* func, float val) {

	char *output;
	int rv;

	if (!matho_init()) {
		printf("Not enough memory.\n");
		exit(1);
	}
	matho_parse(func, NULL);
	char buff[50];
	snprintf(buff, sizeof(buff), "replace x with %f", val);
	rv = matho_process(buff, NULL);

	rv = matho_process("calculate", &output);
	float value = NAN;
	if (output) {
		value = strtof(output, NULL);
		fprintf(gfp, "Value: %f\n", value);
		if (rv) {
		    free(output);
		} else {
		    printf("Error return.\n");
		}
	}

	return value;
}

void regula_falsi(float *x_next, float x0, float x1, float fx0, float fx1, size_t *iter) {

	*x_next = x0 - ((x1 - x0) / (fx1 - fx0))*fx0;
	++(*iter);
	printf("Iteracija #%3zu X = %7.5f \n", *iter, *x_next);
}


float nultocka(char *func, float x0, float x1, float eps, size_t max_iter) {

	size_t iter = 0;
	float x_curr,x_next;
	regula_falsi(&x_curr, x0, x1, run_function(func, x0), run_function(func, x1), &iter);

	do {
		if (run_function(func, x0)*run_function(func, x_curr) < 0) {
			x1 = x_curr;
		} else {
			x0 = x_curr;
		}

		regula_falsi(&x_next, x0, x1, run_function(func, x0), run_function(func, x1), &iter);

		if (fabs(x_next-x_curr) < eps)
		{
			printf("Nakon %zu iteracija, nultočka = %6.4f\n", iter, x_next);
			return x_next;
		}

		x_curr = x_next;
	} while (iter < max_iter);

	printf("Riješenje ne postoji ili je broj iteracija premalen!\n");

	return NAN;
}
