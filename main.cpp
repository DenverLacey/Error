#include <iostream>

#include "error.h"

#define PRINT(expr) printf(#expr " = %d\n", expr);

Result<int> safediv(int a, int b) {
	verify(b != 0, "Division by zero!");
	return a / b;
}

Result<int> safedivadd(int a, int b, int c) {
	int ab = try_(safediv(a, b));
	return ab + c;
}

int main(int argc, const char *argv[]) {
	int a = 1;
	int b = 2;
	int z = 0;

	PRINT(safediv(a, b).unwrap());
	// PRINT(safediv(a, z).unwrap());
	PRINT(safedivadd(a, b, a).unwrap());
	// PRINT(safedivadd(a, z, a).unwrap());

	return 0;
}