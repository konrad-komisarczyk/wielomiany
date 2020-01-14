#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "poly.h"

/**
 * @param a wykładnik
 * @param b wykładnik
 * @return wartość max z argumentów
 */
static poly_exp_t max(poly_exp_t a, poly_exp_t b) {
    if (a > b) {
        return a;
    }
    else {
        return b;
    }
}


/**
 * Porównuje jednomiany po wykładnikach
 * @param a jednomian Mono
 * @param b jednomian Mono
 * @return (-1, gdy a < b), (0, gdy a = b), (1 w p. p.)
 */
static int MonoCompareByExp(const void * a, const void * b) {
    Mono _a = *(Mono*)a;
    Mono _b = *(Mono*)b;
    if(_a.exp < _b.exp) {
        return -1;
    }
    else if(_a.exp == _b.exp) {
        return 0;
    }
    else {
        return 1;
    }
}

/**
 * @param m1 jednomian 1
 * @param m2 jedomian 2
 * @return czy jednomian 1 ma mniejszy wykładnik od 2
 */
static bool MonoIsLesserExp(const Mono *m1, const Mono *m2) {
    return (MonoCompareByExp(m1, m2) == -1);
}



/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
Mono MonoFromPoly(Poly *p, poly_exp_t e) {
    if ((e == 0) && PolyIsCoeff(p)) {
        fprintf(stderr, "Trying to create incorrect monomial (C*x^0)");
        exit(1);
    }

    Mono *m = (Mono *) malloc(sizeof(struct Mono));
    m->poly = *p;
    m->exp = e;
    return *m;
//    return (Mono) {.poly = *p, .exp = e};
}

/**
 * Tworzy pustą listę
 * @return pusta lista
 */
static MonoList MonoListEmpty() {
    return NULL;
}

/**
 * Sprawdza, czy lista jest pusta
 * @param l lista
 * @return Czy lista jest pusta?
 */
static bool MonoListIsEmpty(const MonoList l) {
    return (l == NULL);
}

/**
 * Dodaje jednomian na początek listy.
 * Dba o to, żeby nie dodać na listę jednomianu zerowego.
 * Przejmuje na własność listę oraz jednomian.
 * @param l lista
 * @param m jednomian o stopniu niższym lub równym niż najniższy na liście
 * @return lista zawierająca na początku jednomian m
 */
static MonoList MonoListPush(MonoList l, Mono *m) {
    if (MonoListIsEmpty(l)) {
        MonoList new = (MonoList) malloc(sizeof(struct MonoElem));
        new->head = m;
        new->tail = l;
        return new;
    }
    if (MonoIsLesserExp(m, l->head)) {
        if (MonoIsZero(m)) {
            return l;
        }
        else {
            MonoList new = (MonoList) malloc(sizeof(struct MonoElem));
            new->head = m;
            new->tail = l;
            return new;
        }
    }
    else {
        fprintf(stderr, "Trying to create incorrect MonoList (not sorted)");
        exit(1);
    }
}

/**
 * Tworzy wielomian z listy jednomianów i stałej.
 * Przejmuje listę na własność
 * @param l lista jednomianów
 * @param c stała
 * @return wielomian
 */
static Poly PolyFromMonoList(MonoList l, poly_coeff_t c) {
    Poly *poly = (Poly *) malloc(sizeof(struct Poly));
    poly->coeff = c;
    poly->list = l;
    return *poly;
//    return (Poly) {.list = l, .coeff= c};
}

/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
Poly PolyFromCoeff(poly_coeff_t c) {
    return PolyFromMonoList(MonoListEmpty(), c);
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
Poly PolyZero() {
    return PolyFromCoeff(0);
}



/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
bool PolyIsCoeff(const Poly *p) {
    return MonoListIsEmpty(p->list);
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
bool PolyIsZero(const Poly *p) {
    return (PolyIsCoeff(p) && (p->coeff == 0));
}

/**
 * Sprawdza, czy jednomian jest tożsamościowo równy 0.
 * @param m : jednomian
 * @return Czy jednomian jest równy zero?
 */
bool MonoIsZero(const Mono *m) {
    return PolyIsZero(&(m->poly));
}


/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
void MonoDestroy(Mono *m) {
    if (m != NULL) {
        PolyDestroy(&(m->poly));
        free(m);
    }
}

/**
 * Niszczy pierwszy element listy. Zwraca listę bez niego.
 * @param l lista
 * @return lista bez pierwszego elementu.
 */
static MonoList MonoListPop(MonoList l) {
    if (MonoListIsEmpty(l)) {
        fprintf(stderr, "Trying to pop empty list. Nothing will be done.");
        return NULL;
    }
    else {
        MonoList tail = l->tail;
        MonoDestroy(l->head);
        free(l);
        return tail;
    }
}

/**
 * Usuwa listę z pamięci.
 * @param l lista
 */
static void MonoListDestroy(MonoList l) {
    while (!MonoListIsEmpty(l)) {
        l = MonoListPop(l);
    }
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p) {
    if (p != NULL) {
        MonoListDestroy(p->list);
        free(p);
    }
}



/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
Mono MonoClone(const Mono *m) {
    Poly p_clone = PolyClone(&(m->poly));
    return MonoFromPoly(&p_clone, m->exp);
}

/**
 * Robi pełną, głęboką kopię listy jednomianów
 * @param l lista jednomianów
 * @return skopiowana lista jednomianów
 */
static inline MonoList MonoListClone(const MonoList l) {
    if (MonoListIsEmpty(l)) {
        return MonoListEmpty();
    }
    else {
        Mono mono_clone = MonoClone(l->head);
        return MonoListPush(MonoListClone(l->tail), &mono_clone);
    }
}

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
inline Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    else {
        return PolyFromMonoList(MonoListClone(p->list), p->coeff);
    }
}



/**
 * Dodaje dwa jednomiany o tych samych wykładnikach.
 * @param m1 jednomian Ax^n
 * @param m2 jednomian Bx^n
 * @return jednomian (A+B)x^n
 */
static Mono MonoAdd(const Mono *m1, const Mono *m2) {
    poly_exp_t new_exp = m1->exp;
    if (m2->exp != new_exp) {
        fprintf(stderr, "Trying to make monomial by adding monomials with different exponents");
        exit(1);
    }
    Poly new_poly = PolyAdd(&(m1->poly), &(m2->poly));
    return MonoFromPoly(&new_poly, new_exp);
}

/**
 * Dodaje dwie listy jednomianów.
 * Zachowuje posortowanie.
 * W wynikowej liście nie będą występować jednomiany zerowe, ani powtarzać się wykładniki.
 * @param l1 lista jednomianów
 * @param l2 lista jednomianów
 * @return lista jednomianów będąca sumą wejściowych
 */
static MonoList MonoListAdd(const MonoList l1, const MonoList l2) {
    if (MonoListIsEmpty(l1)) {
        return MonoListClone(l2);
    }
    else if (MonoListIsEmpty(l2)) {
        return MonoListClone(l1);
    }
    else {
        Mono *h1 = l1->head;
        Mono *h2 = l2->head;
        MonoList new_tail;
        Mono new_head;

        if (MonoIsLesserExp(h1, h2)) { // h1 < h2 co do exp
            new_head = MonoClone(h1);
            new_tail = MonoListAdd(l1->tail, l2);
        }
        else if (MonoIsLesserExp(h2, h1)) { // h1 > h2 co do exp
            new_head = MonoClone(h2);
            new_tail = MonoListAdd(l1, l2->tail);
        }
        else { // h1 = h2 co do exp
            new_head = MonoAdd(h1, h2);
            new_tail = MonoListAdd(l1->tail, l2->tail);
        }

        return MonoListPush(new_tail, &new_head);
    }
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q) {
    poly_coeff_t new_coeff = p->coeff + q->coeff;

    if (PolyIsCoeff(p)) {
        MonoList q_list_clone = MonoListClone(q->list);
        return PolyFromMonoList(q_list_clone, new_coeff);
    }
    else if (PolyIsCoeff(q)) {
        MonoList p_list_clone = MonoListClone(p->list);
        return PolyFromMonoList(p_list_clone, new_coeff);
    }
    else {
        MonoList new_l = MonoListAdd(p->list, q->list);
        return PolyFromMonoList(new_l, new_coeff);
    }
}



/**
 * Dodaje do listy jednomian.
 * Jednomian bmusi mieć mniejszy lub równy wykładnik od pierwszego jednomianu na liście. n <= n_1
 * Dba o to, aby na liście nie pojawił się jednomian zerowy oraz by nie powtórzyły się wykładniki.
 * Bierze na własność jednomian oraz listę.
 * @param l jednomian Ax^n
 * @param m lista jednomianów A_1x^{n_1} + ... + A_kx^{n_k}
 * @return Poprawna lista jednomianów Ax^n + A_1x^{n_1} + ... + A_kx^{n_k}
 */
static MonoList MonoListAddMono(MonoList l, Mono *m) {
    if (MonoListIsEmpty(l)) {
        return MonoListPush(l, m);
    }
    else if (MonoIsLesserExp(l->head, m)) {
        fprintf(stderr, "Trying to add mono with higher exp to front of a monomial list.");
        exit(1);
    }
    else if (MonoIsLesserExp(m, l->head)) {
        return MonoListPush(l, m);
    }
    else {
        Mono *head = l->head;
        Mono sum = MonoAdd(m, head);
        MonoDestroy(m);
        MonoList tail = MonoListPop(l);
        if (MonoIsZero(&sum)) {
            MonoDestroy(&sum);
            return tail;
        }
        else {
            return MonoListPush(tail, &sum);
        }
    }
}

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]) {
    Mono monos_copy[count];
    for (int i = 0; i < count; i++) {
        monos_copy[i] = monos[i];
    }

    qsort(monos_copy, count, sizeof(struct Mono), MonoCompareByExp);

    MonoList list = MonoListEmpty();
    poly_coeff_t coeff = 0;
    for (int i = count-1; i >= 0; i--) {
        Mono m = monos_copy[i];
        if (m.exp == 0) {
            //Wyciągamy stałą ze współczynnika m na zewnątrz
            coeff += (m.poly).coeff;
            (m.poly).coeff = 0;
        }

        list = MonoListAddMono(list, &m);
    }

    return PolyFromMonoList(list, coeff);
}



/**
 * Długość listy jednomianów
 * @param l lista
 * @return długość listy
 */
static unsigned MonoListLength(const MonoList l) {
    if (MonoListIsEmpty(l)) {
        return 0;
    }
    else {
        return (MonoListLength(l->tail) + 1);
    }
}


/**
 * Mnoży jednomian przez stałą liczbową różna od 0
 * @param m : jednomian
 * @param c : stała - liczba różna od 0
 * @return 'c * m'
 */
static Mono MonoCoeffMul(const Mono *m, poly_coeff_t c) {
    assert(c != 0);
    Poly c_poly = PolyFromCoeff(c);
    Poly mul_poly = PolyMul(&c_poly, &(m->poly));
    Mono mul = MonoFromPoly(&mul_poly, m->exp);
    PolyDestroy(&c_poly);
    return mul;
}

/**
 * Mnoży dwa jednomiany.
 * @param[in] m1 : jednomian
 * @param[in] m2 : jednomian
 * @return `m1 * m2`
 */
static Mono MonoMul(const Mono *m1, const Mono *m2) {
    Poly mul_poly = PolyMul(&(m1->poly), &(m2->poly));
    return MonoFromPoly(&mul_poly, m1->exp + m2->exp);
}

/**
 * Mnoży listę jednomianów przez stałą.
 * @param l : lista jednomianów
 * @param c : stała - liczba
 * @return 'c * l'
 */
static MonoList MonoListCoeffMul(const MonoList l, poly_coeff_t c) {
    if (MonoListIsEmpty(l) || (c == 0)) {
        return MonoListEmpty();
    }
    else {
        Mono m = MonoCoeffMul(l->head, c);
        return MonoListPush(MonoListCoeffMul(l->tail, c), &m);
    }
}

/**
 * Mnoży dwie listy jednomianów
 * @param l1 : lista jednomianów
 * @param l2 : lista jednomianów
 * @return 'l1 * l2'
 */
static Poly MonoListMul(const MonoList l1, const MonoList l2) {
    unsigned max_count = MonoListLength(l1) * MonoListLength(l2) + 1;
    unsigned count = 0;
    Mono muls[max_count];

    MonoList a = l1;
    while (!MonoListIsEmpty(a)) {
        MonoList b = l2;
        while (!MonoListIsEmpty(b)) {
            Mono curr_mul = MonoMul(a->head, b->head);
            muls[count] = curr_mul;
            count++;

            b = b->tail;
        }
        a = a->tail;
    }

    return PolyAddMonos(count, muls);
}


/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q)) {
        return PolyZero();
    }
    else {
        poly_coeff_t c = p->coeff * q->coeff;
        Poly pc_q = PolyFromMonoList(MonoListCoeffMul(q->list, p->coeff), c);
        Poly qc_p = PolyFromMonoList(MonoListCoeffMul(p->list, q->coeff), 0);
        Poly coeff_muls = PolyAdd(&pc_q, &qc_p);
        Poly higher_exp_muls = MonoListMul(p->list, q->list);

        return PolyAdd(&coeff_muls, &higher_exp_muls);
    }

}



/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p) {
    Poly minus_1 = PolyFromCoeff(-1);
    Poly neg = PolyMul(p, &minus_1);
    PolyDestroy(&minus_1);
    return neg;
}



/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q) {
    Poly neg_q = PolyNeg(q);
    Poly sub = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return sub;
}



/**
 * Zwraca stopień jednomianu ze względu na zadaną zmienną.
 * @param m : jednomian
 * @param var_idx : indeks zmiennej
 * @return stopień jednomianu @p m ze względu na zmienną o indeksie @p var_idx
 */
static poly_exp_t MonoDegBy(const Mono *m, unsigned var_idx) {
    if (var_idx == 0) {
        return m->exp;
    }
    else {
        return PolyDegBy(&(m->poly), var_idx - 1);
    }
}

/**
 * Zwraca stopień listy jednomianów ze względu na zadaną zmienną
 * @param l : lista jednomianów
 * @param var_idx : indeks zmiennej
 * @return stopień listy jednomianów @p l ze względu na zmienną o indeksie @p var_idx
 */
static poly_exp_t MonoListDegBy(const MonoList l, unsigned var_idx) {
    if (MonoListIsEmpty(l)) {
        return 0;
    }
    else {
        return max(MonoDegBy(l->head, var_idx), MonoListDegBy(l->tail, var_idx));
    }
}

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p ze względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if (PolyIsZero(p)) {
        return -1;
    }
    else {
        return MonoListDegBy(p->list, var_idx);
    }
}



/**
 * @param m : jednomian Ax^n
 * @return Stopień jednomianu (po wszystkich zmiennych) n + PolyDeg(A)
 */
static poly_exp_t MonoDeg(const Mono *m) {
    return ((m->exp) + PolyDeg(&(m->poly)));
}

/**
 * @param l : lista jednomianów
 * @return Stopień listy jednomianów
 */
static poly_exp_t MonoListDeg(const MonoList l) {
    if (MonoListIsEmpty(l)) {
        return 0;
    }
    else {
        return max(MonoDeg(l->head), MonoListDeg(l->tail));
    }
}

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p)) {
        return -1;
    }
    else {
        return MonoListDeg(p->list);
    }
}



/**
 * Sprawdza równość dwóch jednomianów
 * @param m1 : jednomian
 * @param m2 : jednomian
 * @return 'm1 = m2'
 */
static bool MonoIsEq(const Mono *m1, const Mono *m2) {
    return ((m1->exp == m2->exp) && PolyIsEq((&m1->poly), &(m2->poly)));
}

/**
 * Sprawdza równość dwóch list jednomianów
 * @param l1 : lista jednomianów
 * @param l2 : lista jednomianów
 * @return 'l1 = l2'
 */
static bool MonoListIsEq(const MonoList l1, const MonoList l2) {
    if (MonoListIsEmpty(l1) && MonoListIsEmpty(l2)) {
        return true;
    }
    else if (MonoListIsEmpty(l1) || MonoListIsEmpty(l2)) {
        return false;
    }
    else {
        return (MonoIsEq(l1->head, l2->head) && MonoListIsEq(l1->tail, l2->tail));
    }
}

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q) {
    return ((p->coeff == q->coeff) && MonoListIsEq(p->list, q->list));
}



/**
 * Liczy potęgę
 * @param x : x
 * @param e : e
 * @return 'x^e'
 */
static poly_coeff_t poly_coeff_t_pow(poly_coeff_t x, poly_exp_t e) {
    if (e == 0) {
        return 1;
    }
    else {
        return x * poly_coeff_t_pow(x, e - 1);
    }
}

/**
 * Wylicza wartość jednomianu w punkcie @p x
 * @param m : jednomian
 * @param x : argument
 * @return Wartość jednomianu w x.
 */
static Poly MonoAt(const Mono *m, poly_coeff_t x) {
    Poly val = PolyFromCoeff(poly_coeff_t_pow(x, m->exp));
    Poly at = PolyMul(&val, &(m->poly));
    PolyDestroy(&val);
    return at;
}

/**
 * Wylicza wartość listy jednomianóœ w punkcie @p x.
 * @param l : lista jednomianów
 * @param x : argument
 * @return Wartość listy jednomianów w x.
 */
static Poly MonoListAt(const MonoList l, poly_coeff_t x) {
    if (MonoListIsEmpty(l)) {
        return PolyZero();
    }
    else {
        Poly t = MonoListAt(l->tail, x);
        Poly h = MonoAt(l->head, x);
        Poly at = PolyAdd(&h, &t);
        PolyDestroy(&t);
        PolyDestroy(&h);
        return at;
    }
}

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianem
 * i zmniejszane są indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        //TODO - Nie powinien być taki przypadek dopuszczony?
        return PolyClone(p);
    }
    else {
        Poly coeff = PolyFromCoeff(p->coeff);
        Poly monos = MonoListAt(p->list, x);
        Poly at = PolyAdd(&monos, &coeff);
        PolyDestroy(&coeff);
        PolyDestroy(&monos);
        return at;
    }
}



#define PRINT_OUT stdout

/**
 * Wypisuje jednomian
 * @param m jednomian
 * @param depth liczba od której zaczynamy numerować zmienne (np. 0)
 */
void MonoPrint(const Mono *m, poly_exp_t depth) {
    fprintf(PRINT_OUT, " + ");
    PolyPrint(&(m->poly), depth+1);
    fprintf(PRINT_OUT, "x_%d^%d", depth, m->exp);
}

/**
 * Wypisuje liste jednomianów
 * @param l lista
 * @param depth liczba od której zaczynamy numerować zmienne (np. 0)
 */
static void MonoListPrint(const MonoList l, poly_exp_t depth) {
    if (!MonoListIsEmpty(l)) {
        MonoPrint(l->head, depth);
        MonoListPrint(l->tail, depth);
    }
}

/**
 * Wypisuje wielomian
 * @param p wielomian
 * @param depth liczba od której zaczynamy numerować zmienne (np. 0)
 */
void PolyPrint(const Poly *p, poly_exp_t depth) {
    fprintf(PRINT_OUT, "(");
    fprintf(PRINT_OUT, "%ld", p->coeff);
    MonoListPrint(p->list, depth);
    fprintf(PRINT_OUT, ")");
}

/**
 * Wypisuje wielomian
 * @param p wielomian
 */
void PolyPuts(const Poly *p) {
    PolyPrint(p, 0);
    fprintf(PRINT_OUT, "\n");
}