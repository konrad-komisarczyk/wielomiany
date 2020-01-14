#include "poly.h"

int main() {
    Poly c_1 = PolyFromCoeff(1);
    Poly c_2 = PolyClone(&c_1);
    Mono m1 = MonoFromPoly(&c_1, 3);
    Mono m2 = MonoClone(&m1);
    Mono monos1[1];
    monos1[0] = m1;
    Poly p1 = PolyAddMonos(1, monos1);

    PolyPuts(&p1);
}