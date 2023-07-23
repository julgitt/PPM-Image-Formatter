#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARGUMENT_COUNT_MISMATCH "Wrong number of arguments"
#define EXTENSION_MISMATCH "Programs image input must be in PPM extension\n"
#define ARGUMENT_MISMATCH "There is no such instruction. Use one of:                        \
                            \n-gamma <intensity (value must be between 0 - 1)>              \
                            \n-filter <available filters (rgb) and all theirs combinations> \
                            \n-contrast <intensity (value must be between 0 - 1)>           \
                            \n-blackwhite\n"
#define PARAMETER_MISMATCH(EXPECTED) "You must provide a " #EXPECTED

void validateFilterParameters(char *str) {
    char *ptr = str;

    if (strcmp(ptr, "") == 0)
        return;

    while ( strcmp(ptr, "") != 0 && strchr("rgb", *ptr) != NULL) {
        ptr++;
    }

    if (ptr > str && *ptr == 0)
        return;

    printf(PARAMETER_MISMATCH(subsequence from (rgb)));
    exit(1);
}

double convertStringToDouble(char *str) {
    // Check if str is in floating point number format
    int len;
    double number;
    int isDouble = sscanf(str, "%lf %n", &number, &len);

    // Check if the number is within the range of 0 to 1
    if (isDouble == 1 && !str[len] && number >= 0.0 && number <= 1.0)
        return number;

    printf(PARAMETER_MISMATCH(floating point number in (0 - 1) range));
    exit(1);
}

void setBlackAndWhite(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    int r, g, b;
    int average;

    while (size > 0) {
        fscanf(inputFile, "%d %d %d ", &r, &g, &b);
        average = (r + g + b) / 3;

        fprintf(outputFile, "%d %d %d ", average, average, average);
        size--;
    }
}

void setFilter(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    validateFilterParameters(parameter);
    int r = 0, g = 0, b = 0;
    int red, green, blue;
    int i = 0;

    while (i < strlen(parameter)) {
        if (parameter[i] == 'r')
            r = 1;
        else if (parameter[i] == 'g')
            g = 1;
        else if (parameter[i] == 'b')
            b = 1;
        i++;
    }

    while (size > 0) {
        fscanf(inputFile, "%d %d %d ", &red, &green, &blue);
        red *= r;
        green *= g;
        blue *= b;

        fprintf(outputFile, "%d %d %d ", red, green, blue);
        size--;
    }
}

void modifyContrast(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double intensity = convertStringToDouble(parameter);
    double r, g, b;

    if (intensity == 0)
        intensity += 0.05;

    intensity *= 2;

    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf ", &r, &g, &b);

        r = fmax(0.0, fmin(255, (((r / 255) - 0.5) * intensity + 0.5) * 255));
        g = fmax(0.0, fmin(255, (((g / 255) - 0.5) * intensity + 0.5) * 255));
        b = fmax(0.0, fmin(255, (((b / 255) - 0.5) * intensity + 0.5) * 255));

        fprintf(outputFile, "%d %d %d ", (int) r, (int) g, (int) b);
        size--;
    }
}

void modifyGamma(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double gammaValue = convertStringToDouble(parameter);
    double r, g, b;

    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf", &r, &g, &b);

        r = (int)fmax(0.0, fmin(255, (r / 255) * (gammaValue + 0.5) * 255));
        g = (int)fmax(0.0, fmin(255, (g / 255) * (gammaValue + 0.5) * 255));
        b = (int)fmax(0.0, fmin(255, (b / 255) * (gammaValue + 0.5) * 255));

        fprintf(outputFile, "%d %d %d ", (int) r, (int) g, (int) b);
        size--;
    }
}

struct {
    char *name;
    void (*function)(int, char *, FILE *, FILE *);
} functions[] = {
        {"-blackwhite", &setBlackAndWhite},
        {"-filter",     &setFilter},
        {"-contrast",   &modifyContrast},
        {"-gamma",      &modifyGamma}
};


int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf(ARGUMENT_COUNT_MISMATCH);
        return 1;
    }

    char extension[2];
    int width;
    int height;
    int maxDepth;

    FILE *inputFile = fopen(argv[1], "r");

    char *outputFileName = argv[1];
    outputFileName[strlen(outputFileName) - 4] = 0;
    strcat(outputFileName, "_output.ppm");

    FILE *outputFile = fopen(outputFileName, "w");

    // extension
    fscanf(inputFile, "%2s", extension);

    if (strcmp(extension, "P6") != 0 && strcmp(extension, "P3") != 0) {
        printf(EXTENSION_MISMATCH);
        return 1;
    }

    fprintf(outputFile, "%s\n", extension);

    // resolution and max depth
    fscanf(inputFile, "%d %d %d", &width, &height, &maxDepth);
    fprintf(outputFile, "%d %d\n%d\n", width, height, maxDepth);
    int size = width * height;

    for (int i = 0; i < 4; i++) {
        if (strcmp(functions[i].name, argv[2]) == 0) {
            functions[i].function(size, (argv[3] == NULL)? "" : argv[3], inputFile, outputFile);
            return 0;
        }
    }

    printf(ARGUMENT_MISMATCH);
    return 1;
}