#include <stdio.h>
#include <string.h>
char aLine[256];
int lineNumber = 0;
int lineLength;
int maxLineLength = 0;
int loopCounter;

int main() {
	// read lines from stdin
	while ( fgets(aLine, 256, stdin) ) {
	// repeat as long as fgets() returns a non-NULL (i.e., non-zero) result
		// obtain the length of the string received
		lineLength = strlen(aLine);
		// update the max-length if new one is longer
		if (lineLength > maxLineLength) {
			maxLineLength = lineLength;
		}
		// replace the newline character, that fgets() obtains,
		// with a null byte
		if (lineLength > 0) {
			aLine[lineLength-1] = '\0';
		}
		// format the string as a code statement
		// and send it out through stdout
		printf("const char ct%.4d[] PROGMEM = \"%s\";\n", lineNumber, aLine);
		// increment the line number
		lineNumber++;
	}

	// output some housekeeping statistics 
	printf("\nMaximum line length: %d\n", maxLineLength);
	printf("Number of lines: %d\n", lineNumber);

	// prepare and output the array declaration
	printf("\nconst char * string_table[] PROGMEM = { ");
	for (loopCounter = 0; loopCounter < lineNumber; loopCounter++) {
		printf("ct%.4d, ", loopCounter);
		// insert a newline at 10-item intervals
		if (loopCounter % 9 == 0) { printf("\n"); }
	}
	printf("}\n");
	return 0;
}
