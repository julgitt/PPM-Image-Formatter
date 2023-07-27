#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Messages macros
#define ARGUMENT_COUNT_MISMATCH "Wrong number of arguments\n"
#define EXTENSION_MISMATCH "Programs image input must be in PPM extension\n"
#define ARGUMENT_MISMATCH "There is no such instruction. Use one of:                        \
                            \n-gamma <intensity (value must be between 0 - 1)>              \
                            \n-filter <available filters (rgb) and all theirs combinations> \
                            \n-contrast <intensity (value must be between 0 - 1)>           \
                            \n-blackwhite\n"
#define PARAMETER_MISMATCH(EXPECTED) "You must provide a " #EXPECTED
#define SUCCESS "Successfully finished\n"

// Functions declarations
int isValidExtension(const char* extension);
void cleanupAndExit(FILE* inputFile,
                    FILE* outputFile,
                    char* outputFileInProgessName,
                    char* outputFileName,
                    char* message,
                    int status);

int modifyGamma(int size, char *parameter, FILE *inputFile, FILE *outputFile);
int modifyContrast(int size, char *parameter, FILE *inputFile, FILE *outputFile);
int setFilter(int size, char *parameter, FILE *inputFile, FILE *outputFile);
int setBlackAndWhite(int size, char *parameter, FILE *inputFile, FILE *outputFile);
double convertStringToDouble(char *str);
int validateFilterParameters(char *str);

// function to name mapping
struct {
    char *name;
    int (*function)(int, char *, FILE *, FILE *);
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

    // files set up
    FILE *inputFile = fopen(argv[1], "r");

    char *outputFileName = argv[1];
    outputFileName[strlen(outputFileName) - 4] = 0;
    char *outputFileInProgessName = malloc(sizeof(outputFileName) + 20);
    strcpy(outputFileInProgessName, outputFileName);
    strcat(outputFileName, "_output.ppm");
    strcat(outputFileInProgessName, "_output_in_progress.ppm");

    FILE *outputFile = fopen(outputFileInProgessName, "w");

    // extension validation
    fscanf(inputFile, "%2s", extension);

    if (!isValidExtension(extension))
        cleanupAndExit(inputFile,
                       outputFile,
                       outputFileInProgessName,
                       outputFileName,
                       EXTENSION_MISMATCH,
                       1);


    fprintf(outputFile, "%s\n", extension);

    // resolution and max depth
    fscanf(inputFile, "%d %d %d", &width, &height, &maxDepth);
    fprintf(outputFile, "%d %d\n%d\n", width, height, maxDepth);
    int size = width * height;

    // search and run function
    for (int i = 0; i < 4; i++) {
        if (strcmp(functions[i].name, argv[2]) == 0)
                cleanupAndExit(inputFile,
                               outputFile,
                               outputFileInProgessName,
                               outputFileName,
                               SUCCESS,
                               functions[i].function(size, (argv[3] == NULL)? "" : argv[3], inputFile, outputFile));

    }

    cleanupAndExit(inputFile, outputFile, outputFileInProgessName, outputFileName, ARGUMENT_MISMATCH, 1);
}

// functions definitions
int isValidExtension(const char* extension) {
    return (strcmp(extension, "P6") == 0 || strcmp(extension, "P3") == 0);
}

void cleanupAndExit(FILE* inputFile, FILE* outputFile, char* outputFileInProgessName,
                    char* outputFileName, char* message, int status) {
    fclose(inputFile);
    fclose(outputFile);
    if (status)
        remove(outputFileInProgessName);
    else {
        remove(outputFileName);
        rename(outputFileInProgessName, outputFileName);
    }
    printf("%s", message);
    exit(status);
}

int validateFilterParameters(char *str) {
    char *ptr = str;

    if (strcmp(ptr, "") == 0)
        return 0;

    while ( strcmp(ptr, "") != 0 && strchr("rgb", *ptr) != NULL) {
        ptr++;
    }

    if (ptr > str && *ptr == 0)
        return 0;

    printf(PARAMETER_MISMATCH(subsequence from (rgb).));
    return -1;
}

double convertStringToDouble(char *str) {
    // Check if str is in floating point number format
    int len;
    double number;
    int isDouble = sscanf(str, "%lf %n", &number, &len);

    // Check if the number is within the range of 0 to 1
    if (isDouble == 1 && !str[len] && number >= 0.0 && number <= 1.0)
        return number;

    printf(PARAMETER_MISMATCH(floating point number in (0 - 1) range.));
    return -1;
}

int setBlackAndWhite(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    int r, g, b;
    int average;

    while (size > 0) {
        fscanf(inputFile, "%d %d %d ", &r, &g, &b);
        average = (r + g + b) / 3;

        fprintf(outputFile, "%d %d %d ", average, average, average);
        size--;
    }
    return 0;
}

int setFilter(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    if (validateFilterParameters(parameter) == -1) return 1;
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
    return 0;
}

int modifyContrast(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double contrastValue = convertStringToDouble(parameter);
    if (contrastValue == -1) return 1;
    double r, g, b;

    if (contrastValue == 0)
        contrastValue += 0.05;

    contrastValue *= 2;

    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf ", &r, &g, &b);

        r = fmax(0.0, fmin(255, (((r / 255) - 0.5) * contrastValue + 0.5) * 255));
        g = fmax(0.0, fmin(255, (((g / 255) - 0.5) * contrastValue + 0.5) * 255));
        b = fmax(0.0, fmin(255, (((b / 255) - 0.5) * contrastValue + 0.5) * 255));

        fprintf(outputFile, "%d %d %d ", (int) r, (int) g, (int) b);
        size--;
    }
    return 0;
}

int modifyGamma(int size, char *parameter, FILE *inputFile, FILE *outputFile) {
    double gammaValue = convertStringToDouble(parameter);
    if (gammaValue == -1) return 1;
    double r, g, b;

    while (size > 0) {
        fscanf(inputFile, "%lf %lf %lf", &r, &g, &b);

        r = (int)fmax(0.0, fmin(255, (r / 255) * (gammaValue + 0.5) * 255));
        g = (int)fmax(0.0, fmin(255, (g / 255) * (gammaValue + 0.5) * 255));
        b = (int)fmax(0.0, fmin(255, (b / 255) * (gammaValue + 0.5) * 255));

        fprintf(outputFile, "%d %d %d ", (int) r, (int) g, (int) b);
        size--;
    }
    return 0;
}
