#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define IF   (10.7d)

/*
    Three-point tracking simulator
https://www.vintage-radio.info/download.php?id=485

                 -.
            Ctrim/|
              ||/
       +------||-------+
       |     /||       |  Cpad
       |               |   ||
       +--->||---||<---+---||---+
       |    Varicaps       ||   |
       |                        |
 ------+---       L       ------+------
           \_/\_/\_/\_/\_/
*/



const double tolerance = 0.001d;   /* MHz deviation from IF at the trim points */

double l_high = 110.0d;
double c_high_pad = 125.0d;
double c_high_trim = 3.0d;

double l_low = 135.0d;
double c_low_pad = 330.0d;
double c_low_trim = 3.0d;


double replus (double a, double b) {
    return (1.0d / ((1.0d / a) + (1.0d / b)));
}

double MHz (double pF, double nH) {
    return (1.0d / (3.1415926d * 2 * sqrt((pF * 1e-12) * (nH * 1e-9)))) / 1e6;
}

double freq (double varicap, double l, double c_pad, double c_trim) {
    return MHz(replus(varicap + c_trim, c_pad), l);
}

int check (double varicap) {
    int rc = 0;
    double f_low;
    double f_high;

    f_low = freq(varicap, l_low, c_low_pad, c_low_trim);
    f_high = freq(varicap, l_high, c_high_pad, c_high_trim);

    if ((f_high - f_low) < (IF - tolerance)) {
        return 1;
    } else if ((f_high - f_low) > (IF + tolerance)) {
        return -1;
    }
    return 0;
}

void trim (double *element, double varicap) {
    const double step = tolerance * 1e-2;
    double f_low;
    double f_high;
    while (1) {
        int rc = check(varicap);

        if (rc > 0) {
            *element -= step;
        } else if (rc < 0) {
            *element += step;
        } else {
            break;
        }
    }
    return;
}

void dump_elements(void) {
    printf("l_high: %.1fnH, c_high_pad:%.1fpf c_high_trim:%.1fpf\n"
           "l_low:  %.1fnH, c_low_pad:%.1fpf c_low_trim:%.1fpf\n\n",
           l_high, c_high_pad, c_high_trim,
           l_low, c_low_pad, c_low_trim);
}

void dump_res(double varicap_h, double varicap_l) {
    double varicap;
    double varicap_dec = 0.5d;

    printf("\n\n");
    printf("varicap (pF)    high_f (MHz)     low_f (MHz)         tracking (kHz)         Q\n\n");
    for (varicap = varicap_h; varicap >= varicap_l; varicap -= varicap_dec) {
        double f_low;
        double f_high;
        double f_diff;

//        varicap_dec *= 0.98d;

        f_low = freq(varicap, l_low, c_low_pad, c_low_trim);
        f_high = freq(varicap, l_high, c_high_pad, c_high_trim);

        f_diff = (IF - (f_high-f_low + 0.0005d));
        printf("%.1f            %.2f            %.2f                %.1f                %.1f\n",
                varicap,        f_high,      f_low,           f_diff * 1000.0d,           sqrt((f_low/f_diff)*(f_low/f_diff)));
    }
}

int main (int argc, char** argv) {

    const double varicap_h = 31.0d;
    const double varicap_l = 2.0d;
    const double varicap_m = (((varicap_h - varicap_l) / 4.0d) + varicap_l);

    double varicap;

    while (1) {

        dump_elements();

        /*** LO ***/
        if (check(varicap_h)) {
            trim(&c_high_pad, varicap_h);
            continue;
        }

        /*** MID ***/
        if (check(varicap_m)) {
            trim(&l_high, varicap_m);
            continue;
        }

        /*** HI ***/
        if (check(varicap_l)) {
            trim(&c_high_trim, varicap_l);
            continue;
        }
        break;
    }

    dump_res(varicap_h, varicap_l);

    return 0;
}

