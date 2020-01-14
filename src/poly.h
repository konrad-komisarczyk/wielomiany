/** @file
   Interfejs klasy wielomianów

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>, Konrad Komisarczyk
   @copyright Uniwersytet Warszawski
   @date 2017-04-24, TODO
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;



/**
 * Struktura przechowująca jednomian.
 */
typedef struct Mono Mono;

/**
 * Lista jednomianów.
 * Poprawne są jedynie listy posortowane rosnąco według wykładnika.
 * Listy zawierające jednomiany tożsamościowo równe zero nie są poprawne.
 */
typedef struct MonoElem * MonoList;

/**
 * Element listy jednomianów.
 * Składa się z głowy (jednomianu) oraz ogona (listy jednomianów)
 */
struct MonoElem {
    Mono *head; ///< głowa - jednomian
    MonoList tail; ///< ogon
};

/**
 * Struktura przechowująca wielomian.
 * Wielomian jest sumą pewnego zbioru jednomianów oraz wyrazu wolnego będącego liczbą.
 *
 * Jednomiany wchodzące w skład wielomianu przechowywane są w liście.
 * Lista jest posortowana rosnąco według wykładników.
 * Nie powtarzają się w niej wykładniki.
 *
 * Jest tylko jedna poprawna reprezentacja wielomianu równego 0.
 *
 * Wielomiany x^0 * ... * z^0 * C, gdzie C jest stałą są nie powinny powstać.
 * Stała C jest w takim przypadku 'wyciągana' na zewnątrz.
 *
 */
typedef struct Poly {
    MonoList list; ///< lista jednomianów
    poly_coeff_t coeff; ///< wyraz wolny
} Poly;

/**
 * Struktura przechowująca jednomian.
 * Jednomian ma postać `p * x^e`.
 * Współczynnik `p` może też być wielomianem.
 * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
 * Niepoprawny jest jednomian Cx^0, gdzie C jest liczbą. Nie powinien taki powstać.
 */
struct Mono {
    Poly poly; ///< współczynnik
    poly_exp_t exp; ///< wykładnik
};





/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
Poly PolyFromCoeff(poly_coeff_t c);

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
Poly PolyZero();

/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
Mono MonoFromPoly(Poly *p, poly_exp_t e);

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
bool PolyIsCoeff(const Poly *p);

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
bool PolyIsZero(const Poly *p);

/**
 * Sprawdza, czy jednomian jest tożsamościowo równy 0.
 * @param m : jednomian
 * @return Czy jednomian jest równy zero?
 */
bool MonoIsZero(const Mono *m);

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
void MonoDestroy(Mono *m);

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
Mono MonoClone(const Mono *m);

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

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
Poly PolyAt(const Poly *p, poly_coeff_t x);




/**
 * Wypisuje jednomian
 * @param m jednomian
 * @param depth liczba od której zaczynamy numerować zmienne (np. 0)
 */
void MonoPrint(const Mono *m, poly_exp_t depth);

/**
 * Wypisuje wielomian
 * @param p wielomian
 * @param depth liczba od której zaczynamy numerować zmienne (np. 0)
 */
void PolyPrint(const Poly *p, poly_exp_t depth);

/**
 * Wypisuje wielomian
 * @param p wielomian
 */
void PolyPuts(const Poly *p);

#endif /* __POLY_H__ */
