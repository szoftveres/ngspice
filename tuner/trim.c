#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IF   (10.7d)

double replus (double a, double b) {
    return (1.0d / ((1.0d / a) + (1.0d / b)));
}

double mhz (double pf, double nh) {
    return (1.0d / (3.1415926d * 2 * sqrt((pf * 1e-12) * (nh * 1e-9)))) / 1e6;
}

int trim (double *element, double varicap, double *coil, double* cbig, double* csmall) {
    const double tolerance = 0.0001d;
    const double step = tolerance * 1e-2;
    double fosc;
    double fmod;
    int trim = 0;
    int do_trim = 0;
    do {
        fosc = mhz(varicap + 5.0d, 110.0d);
        fmod = mhz(replus(varicap, *cbig) + *csmall, *coil);
        if ((fmod - fosc) < (IF - (do_trim ? (tolerance / 10.0d) : tolerance))) {
            do_trim = 1;
            trim = 1;
            *element -= step;
        } else if ((fmod - fosc) > (IF + (do_trim ? (tolerance / 10.0d) : tolerance))) {
            do_trim = 1;
            trim = 1;
            *element += step;
        } else if (do_trim) {
            do_trim = 0;
        }
    } while (do_trim);
    printf("%.2f MHz: %02.02f        coil:%.2fnH     cbig:%.2fpF     csmall:%.2fpF             \n",
           fmod, fmod-fosc, *coil, *cbig, *csmall);
    return trim;
}


int main (int argc, char** argv) {

    double coil = 100.0d;
    double cbig = 300.0d;
    double csmall = 5.0d;

    do {
        usleep(100);

        /*** MID ***/
        if (trim(&cbig, 70.0d, &coil, &cbig, &csmall)) {
            continue;
        }

        /*** LO ***/
        if (trim(&coil, 20.0d, &coil, &cbig, &csmall)) {
            continue;
        }

        /*** HI ***/
        if (!trim(&csmall, 5.0d, &coil, &cbig, &csmall)) {
            break;
        }

    } while (1);
    return 0;
}
