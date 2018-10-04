#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IF   (10.7d)


const double tolerance = 0.0002d;   /* MHz deviation from IF at the trim points */
const double cmodsmall = 5.0d;
const double lmodcoil = 110.0d;



double replus (double a, double b) {
    return (1.0d / ((1.0d / a) + (1.0d / b)));
}

double MHz (double pF, double nH) {
    return (1.0d / (3.1415926d * 2 * sqrt((pF * 1e-12) * (nH * 1e-9)))) / 1e6;
}

int check (double varicap, double *lcoil, double* cbig, double* csmall) {
    int rc = 0;
    double fosc;
    double fmod;

    fosc = MHz(varicap + cmodsmall, lmodcoil);
    fmod = MHz(replus(varicap + *csmall, *cbig), *lcoil);

    if (((fmod - fosc) < (IF - tolerance)) || ((fmod - fosc) > (IF + tolerance))) {
        return 1;
    }
    return 0;
}

void trim (double *element, double varicap, double *lcoil, double* cbig, double* csmall) {
    const double step = tolerance * 1e-2;
    double fosc;
    double fmod;
    while (1) {
        fosc = MHz(varicap + cmodsmall, lmodcoil);
        fmod = MHz(replus(varicap + *csmall, *cbig), *lcoil);
        if ((fmod - fosc) < (IF - (tolerance / 10.0d))) {
            *element -= step;
        } else if ((fmod - fosc) > (IF + (tolerance / 10.0d))) {
            *element += step;
        } else {
            break;
        }
    }
    printf("%.2f MHz: %02.02f        lcoil:%.2fnH     cbig:%.2fpF     csmall:%.2fpF\n",
           fmod, fmod-fosc, *lcoil, *cbig, *csmall);
    return;
}


int main (int argc, char** argv) {

    const double ch = 35.0d;
    const double cl = 7.0d;
    const double cm = (((ch - cl) / 2.0d) + cl);

    double varicap;
    double lcoil = lmodcoil;  /* starting values */
    double cbig = ch;   /* starting values */
    double csmall = cmodsmall;   /* starting values */

    while (1) {
        printf("\n");

        /*** LO ***/
        if (check(ch, &lcoil, &cbig, &csmall)) {
            trim(&cbig, ch, &lcoil, &cbig, &csmall);
            continue;
        }

        /*** MID ***/
        if (check(cm, &lcoil, &cbig, &csmall)) {
            trim(&lcoil, cm, &lcoil, &cbig, &csmall);
            continue;
        }

        /*** HI ***/
        if (check(cl, &lcoil, &cbig, &csmall)) {
            trim(&csmall, cl, &lcoil, &cbig, &csmall);
            continue;
        }
        break;
    }

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

