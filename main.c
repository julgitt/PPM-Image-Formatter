#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARGUMENT_COUNT_MISMATCH "Wrong number of arguments"
#define ARGUMENT_MISMATCH "There is no such instruction. Use one of these:                  \
                            \n-gamma <intensity (value must be between 0 - 1)>              \
                            \n-filter <available filters (rgb) and all theirs combinations> \
                            \n-contrast <intensity (value must be between 0 - 1)>           \
                            \n-blackwhite\n"
#define PARAMETER_COUNT_MISMATCH "Wrong number of parameters"
#define EXTENCION_MISMATCH "Programs image input must be in PPM extencion\n"
#define PARAMETER_MISMATCH(EXPECTED) "Parameter must be a " #EXPECTED

void blackwhite(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    int r, g, b;
    int average;
    while (size > 0) {
        fscanf(inputFile, "%d %d %d ", &r, &g, &b);
        average = (r + g + b) / 3;
        fprintf(outputFile, "%d %d %d ", average, average, average);
        size--;
    }
}

void filter(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    int r = 0, g = 0, b = 0;
    int red, green, blue;
    int i = 0;
    while (i < strlen(parameter)) {
        if (parameter[i] == 'r') {
            r = 1;
        } else if (parameter[i] == 'g') {
            g = 1;
        } else if (parameter[i] == 'b') {
            b = 1;
        }
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

void contrast(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double intencity = atof(parameter);
    double r, g, b;
    if (intencity == 0)
        intencity += 0.05;
    double factor = intencity * 2;
    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf ", &r, &g, &b);

        r /= 255.0;
        g /= 255.0;
        b /= 255.0;

        // Apply contrast adjustment
        r = (r - 0.5) * factor + 0.5;
        g = (g - 0.5) * factor + 0.5;
        b = (b - 0.5) * factor + 0.5;


        // Clamp pixel values to the valid range [0, 1]
        r = fmin(1.0, fmax(0.0, r));
        g = fmin(1.0, fmax(0.0, g));
        b = fmin(1.0, fmax(0.0, b));

        // Scale back to the range [0, 255]
        r *= 255.0;
        g *= 255.0;
        b *= 255.0;

        fprintf(outputFile, "%d %d %d ", (int) r, (int) g, (int) b);
        size--;
    }
}

void gamma(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double gammaValue = atof(parameter);
    double r, g, b;
    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf", &r, &g, &b);
        int red = ((int) (pow(r * 0.001, gammaValue) * 100)) % 256;
        int green = ((int) (pow(g * 0.001, gammaValue) * 100)) % 256;
        int blue = ((int) (pow(b * 0.001, gammaValue) * 100)) % 256;

        fprintf(outputFile, "%d %d %d ", red, green, blue);
        size--;
    }
}

int isStringAValidSequence(char *str) {
    char *ptr = str;
    while (strcmp(ptr, "") != 0 && strchr("rgb", *ptr) != NULL)
        ptr++;

    if (ptr > str && *ptr == 0)
        return 1;
    printf(PARAMETER_MISMATCH(sequence from(rgb) set));
    return 0;
}

int isStringAValidNumber(char *str) {
    // Check if str is in floating point number format
    int len;
    float number;
    int isFloat = sscanf(str, "%f %n", &number, &len);

    // Check if the number is within the range of 0 to 1
    if (isFloat == 1 && !str[len] && number >= 0.0 && number <= 1.0)
        return 1;

    printf(PARAMETER_MISMATCH(floating point number in(0 - 1) range));
    return 0;
}


struct {
    char *name;

    void (*function)(int, char *, FILE *, FILE *);

    int parameterRequired;

    int (*validation)(char *);
} functions[] = {
        {"-blackwhite", &blackwhite, 0, NULL},
        {"-filter",     &filter,     1, &isStringAValidSequence},
        {"-contrast",   &contrast,   1, &isStringAValidNumber},
        {"-gamma",      &gamma,      1, &isStringAValidNumber}
};


int main(int argc, char *argv[]) {

    char extencion[2];
    int width;
    int height;
    int maxDepth;

    if (argc >= 3) {
        FILE *inputFile = fopen(argv[1], "r");
        FILE *outputFile = fopen("../output.ppm", "w");

        // extencion
        fscanf(inputFile, "%2s", extencion);

        if (strcmp(extencion, "P6") != 0 && strcmp(extencion, "P3") != 0) {
            printf(EXTENCION_MISMATCH);
            return 1;
        }
        fprintf(outputFile, "%s\n", extencion);

        // resolution and max depth
        fscanf(inputFile, "%d %d %d", &width, &height, &maxDepth);
        fprintf(outputFile, "%d %d\n%d\n", width, height, maxDepth);
        int size = width * height;

        for (int i = 0; i < 4; i++) {
            if (strcmp(functions[i].name, argv[2]) == 0) {
                if (functions[i].parameterRequired == 1) {
                    if (argc != 4) {
                        printf(PARAMETER_COUNT_MISMATCH);
                        return 1;
                    } else if (functions[i].validation(argv[3]) == 0) {
                        return 1;
                    }
                }
                functions[i].function(size, functions[i].parameterRequired ? argv[3] : NULL, inputFile, outputFile);
                return 0;
            }
        }
        printf(ARGUMENT_MISMATCH);
        return 1;
    } else {
        printf(ARGUMENT_COUNT_MISMATCH);
        return 1;
    }
}