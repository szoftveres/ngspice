#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double replus (double a, double b) {
    return (1.0d / ((1.0d / a) + (1.0d / b)));
}

double mhz (double pf, double nh) {
    return (1.0d / (3.1415926d * 2 * sqrt((pf * 1e-12) * (nh * 1e-9)))) / 1e6;
}

int main (int argc, char** argv) {
    double varicap;

    double fosc;
    double fmod;

    for (varicap = 30.0d; varicap > 10.0d; varicap -= 2.0d) {
        fosc = mhz(varicap + 5.0d, 110.0d);
        fmod = mhz(replus(varicap + 4.0d, 100.0d), 110.0d);

        printf("mod:%.2f     osc:%.2f     %.2f\n", fmod, fosc, fmod-fosc);
    }
    return 0;
}
