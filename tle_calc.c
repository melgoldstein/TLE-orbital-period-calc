#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int checksum(const char *line) {
    unsigned total = 0;
    if (strlen(line) != 69 || !isdigit((unsigned char)line[68])) return 0;
    for (size_t index = 0; index < 68; ++index) {
        if (isdigit((unsigned char)line[index])) total += line[index] - '0';
        else if (line[index] == '-') ++total;
    }
    return total % 10 == (unsigned)(line[68] - '0');
}

static int number(const char *line, size_t start, size_t width, double *result) {
    char field[20], *end;
    if (width >= sizeof field) return 0;
    memcpy(field, line + start, width);
    field[width] = '\0';
    errno = 0;
    *result = strtod(field, &end);
    if (errno || end == field || !isfinite(*result)) return 0;
    while (*end == ' ') ++end;
    return *end == '\0';
}

int main(int argc, char **argv) {
    if (argc == 2 && !strcmp(argv[1], "--help")) {
        puts("usage: tle_calc orbital_elements.txt"); return 0;
    }
    if (argc != 2) {
        fprintf(stderr, "usage: tle_calc orbital_elements.txt\n");
        return 2;
    }
    FILE *input = fopen(argv[1], "r");
    if (!input) { perror(argv[1]); return 1; }
    char line[256], first[70] = "";
    unsigned count = 0;
    int failed = 0;
    puts("catalog_id,period_min,semi_major_km,perigee_km,apogee_km");
    while (fgets(line, sizeof line, input)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] != '1' && line[0] != '2') continue;
        if (!checksum(line)) { failed = 1; break; }
        if (line[0] == '1') {
            if (*first) { failed = 1; break; }
            memcpy(first, line, 70);
            continue;
        }
        if (!*first || memcmp(first + 2, line + 2, 5)) { failed = 1; break; }
        double motion, eccentricity_digits;
        int valid_digits = 1;
        for (size_t index = 26; index < 33; ++index)
            if (!isdigit((unsigned char)line[index])) valid_digits = 0;
        if (!valid_digits || !number(line, 52, 11, &motion) ||
            !number(line, 26, 7, &eccentricity_digits) || motion <= 0) {
            failed = 1;
            break;
        }
        double eccentricity = eccentricity_digits * 1e-7;
        double period_seconds = 86400.0 / motion;
        double angular_rate = 2 * acos(-1.0) / period_seconds;
        double semi_major = cbrt(398600.4418 / (angular_rate * angular_rate));
        printf("%.5s,%.6f,%.6f,%.6f,%.6f\n", line + 2,
               period_seconds / 60, semi_major,
               semi_major * (1 - eccentricity) - 6378.137,
               semi_major * (1 + eccentricity) - 6378.137);
        first[0] = '\0';
        ++count;
    }
    failed = failed || ferror(input) || !count || *first;
    fclose(input);
    if (failed) fprintf(stderr, "invalid, incomplete or empty TLE input\n");
    return failed;
}
