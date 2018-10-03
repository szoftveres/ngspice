#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IF   (10.7d)

double replus (double a, double b) {
    return (1.0d / ((1.0d / a) + (1.0d / b)));
}

double MHz (double pF, double nH) {
    return (1.0d / (3.1415926d * 2 * sqrt((pF * 1e-12) * (nH * 1e-9)))) / 1e6;
}

int trim (double *element, double varicap, double *lcoil, double* cbig, double* csmall) {
    const double tolerance = 0.0002d;
    const double step = tolerance * 1e-2;
    double fosc;
    double fmod;
    int trim = 0;
    int do_trim = 0;
    do {
        fosc = MHz(varicap + 5.0d, 110.0d);
        fmod = MHz(replus(varicap + *csmall, *cbig), *lcoil);
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
    printf("%.2f MHz: %02.02f        lcoil:%.2fnH     cbig:%.2fpF     csmall:%.2fpF             \n",
           fmod, fmod-fosc, *lcoil, *cbig, *csmall);
    return trim;
}


int main (int argc, char** argv) {

    const double ch = 35.0d;
    const double cl = 0.6d;
    const double cm = (((ch - cl) / 2.0d) + cl);

    int oot;
    double varicap;
    double lcoil = 150.0d;  /* starting values */
    double cbig = 100.0d;   /* starting values */
    double csmall = 5.0d;   /* starting values */

    do {
        oot = 0;
        printf("\n");

        /*** MID ***/
        oot += trim(&cbig, ch, &lcoil, &cbig, &csmall);
        if (oot) {
            continue;
        }

        /*** LO ***/
        oot += trim(&lcoil, cm, &lcoil, &cbig, &csmall);
        if (oot) {
            continue;
        }

        /*** HI ***/
        oot += trim(&csmall, cl, &lcoil, &cbig, &csmall);
        if (oot) {
            continue;
        }

    } while (oot);

    printf("\n\n");
    for (varicap = ch; varicap > cl; varicap -= 0.5d) {
        double fosc;
        double fmod;
        fosc = MHz(varicap + 5.0d, 110.0d);
        fmod = MHz(replus(varicap + csmall, cbig), lcoil);
        printf("%.1fpF, %.2f, %.3f\n", varicap, fmod, IF - (fmod-fosc + 0.0005d));
    }

    return 0;
}

