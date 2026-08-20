/*MODELUL COMPLEX*/
//P6 - coada de asteptare 
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

const int nr_tranz = 11;

double ceas;
double D[nr_tranz + 1];

double lmd1 = 7, lmd2 = 0.000001, lmd3 = 1, lmd4 = 0.000001;

double miu11 = 11, miu12 = 9;
double miu21 = 11, miu23 = 9;
double miu32 = 11, miu34 = 9;
double miu43 = 11, miu44 = 9;

double sig11 = 1. / (5 * miu11), sig12 = 1. / (5 * miu12);
double sig21 = 1. / (5 * miu21), sig23 = 1. / (5 * miu23);
double sig32 = 1. / (5 * miu32), sig34 = 1. / (5 * miu34);
double sig43 = 1. / (5 * miu43), sig44 = 1. / (5 * miu44);

double P1_s[100000]; int P1_c[100000]; int nP1 = 0;
double P6_s[100000]; int P6_c[100000]; int nP6 = 0;

double P2[100000]; int nP2 = 0;
double P3[100000]; int nP3 = 0;
double P4[100000]; int nP4 = 0;
double P5[100000]; int nP5 = 0;

double t5_s = 0; int t5_c = 0;
double t6_s = 0; int t6_c = 0;
double t7_s = 0; int t7_c = 0;

int P11 = 1;//P11=1 poate veni cererea din P6 in t8
int t9 = 0, t10 = 0;

int P7P8 = 0;        //0=liber
double P7P8_s = 0; int P7P8_c = 0;

double STr[5] = { 0,0,0,0,0 };
long int CT[5] = { 0,0,0,0,0 };

double genExp(double lambda);
double genGauss(double medie, double sigma);
void executaTranzitie(int te);
void ruleazaTranzitii();

int main() {

    srand((unsigned)time(NULL));

    ceas = 0.0;
    double DS = 1e5;

    for (int i = 1; i <= nr_tranz; i++) D[i] = -1;

    D[1] = genExp(lmd1);
    D[2] = genExp(lmd2);
    D[3] = genExp(lmd3);
    D[4] = genExp(lmd4);

    while (ceas < DS) {

        ruleazaTranzitii();

        double t_min = 999999.0;
        int tranz = -1;
        for (int i = 1; i <= nr_tranz; i++) {
            if (D[i] >= 0 && D[i] < t_min) {
                t_min = D[i];
                tranz = i;
            }
        }

        if (tranz == -1) {
            cout << "Eroare: Blocaj sistem la momentul: " << ceas << "\n";
            break;
        }

        if (ceas + t_min > DS) t_min = DS - ceas;

        ceas += t_min;
        for (int i = 1; i <= nr_tranz; i++) {
            if (D[i] >= 0) D[i] -= t_min;
        }

        if (ceas >= DS) break;

        executaTranzitie(tranz);

    }

    cout << "DS: " << ceas << "\n\n";

    const char* nume[5] = { "", "culoare 1 (cerc plin)", "culoare 2 (cerc gol)", "culoare 3 (patrat)", "culoare 4 (romb)" };

    double STr_total = 0.0;
    long int CT_total = 0;
    for (int i = 1; i <= 4; i++) {
        if (CT[i] > 0) {
            double TMR_i = STr[i] / CT[i];
            cout << "Cereri " << nume[i] << ": " << CT[i] << " servite (TMR: " << TMR_i << ")\n";
        }
        else {
            cout << "Cereri " << nume[i] << ": 0 servite.\n";
        }
        STr_total += STr[i];
        CT_total += CT[i];
    }

    if (CT_total > 0) {
        double TMR = STr_total / (double)CT_total;
        cout << "\n -> Timpul Mediu de Raspuns (global): " << TMR << "\n";
    }
    else
        cout << "\nEroare: nicio cerere nu a trecut prin sistem.\n";

    return 0;
}

void ruleazaTranzitii() {
    int i;
    bool ok = true;
    while (ok) {
        ok = false;

        //t5 devine validata
        if (D[5] < 0 && nP1 > 0) {
            t5_c = P1_c[1];
            t5_s = P1_s[1];
            for (i = 1; i < nP1; i++) { P1_s[i] = P1_s[i + 1]; P1_c[i] = P1_c[i + 1]; }
            nP1--;
            D[5] = (t5_c == 1) ? genExp(miu11) : genExp(miu12);
            //D[5] = (t5_c == 1) ? genGauss(1./miu11, sig11) : genGauss(1./miu12, sig12);
            ok = true;
        }

        //t6 devine validata
        if (D[6] < 0 && (nP2 > 0 || nP3 > 0)) {
            if (nP2 > 0) {
                t6_c = 1;
                t6_s = P2[1];
                for (i = 1; i < nP2; i++) P2[i] = P2[i + 1];
                nP2--;
            }
            else {
                t6_c = 3;
                t6_s = P3[1];
                for (i = 1; i < nP3; i++) P3[i] = P3[i + 1];
                nP3--;
            }
            D[6] = (t6_c == 1) ? genExp(miu21) : genExp(miu23);
            //D[6] = (t6_c == 1) ? genGauss(1./miu21, sig21) : genGauss(1./miu23, sig23);
            ok = true;
        }

        //t7 devine validata
        if (D[7] < 0 && (nP5 > 0 || nP4 > 0)) {
            if (nP5 > 0) {
                t7_c = 2;
                t7_s = P5[1];
                for (i = 1; i < nP5; i++) P5[i] = P5[i + 1];
                nP5--;
            }
            else {
                t7_c = 4;
                t7_s = P4[1];
                for (i = 1; i < nP4; i++) P4[i] = P4[i + 1];
                nP4--;
            }
            D[7] = (t7_c == 2) ? genExp(miu32) : genExp(miu34);
            //D[7] = (t7_c == 2) ? genGauss(1./miu32, sig32) : genGauss(1./miu34, sig34);
            ok = true;
        }

        //t8
        if (!P7P8 && P11 && nP6 > 0) {
            P7P8_c = P6_c[1];
            P7P8_s = P6_s[1];
            for (i = 1; i < nP6; i++) { P6_s[i] = P6_s[i + 1]; P6_c[i] = P6_c[i + 1]; }
            nP6--;

            P7P8 = 1; P11 = 0;
            t9 = 0; t10 = 0;
            D[9] = (P7P8_c == 3) ? genExp(miu43) : genExp(miu44);
            //D[9] = (P7P8_c == 3) ? genGauss(1./miu43, sig43) : genGauss(1./miu44, sig44);
            D[10] = (P7P8_c == 3) ? genExp(miu43) : genExp(miu44);
            //D[10] = (P7P8_c == 3) ? genGauss(1./miu43, sig43) : genGauss(1./miu44, sig44);
            ok = true;
        }

        //t11-sincronizarea
        if (P7P8 && t9 && t10) {
            STr[P7P8_c] += ceas - P7P8_s;
            CT[P7P8_c]++;
            P7P8 = 0; P11 = 1;
            ok = true;
        }
    }
}

void executaTranzitie(int te) {
    switch (te) {

    case 1: //sosire cerere culoare 1 (●)
        nP1++; P1_s[nP1] = ceas; P1_c[nP1] = 1;
        D[1] = genExp(lmd1);
        break;

    case 2: //sosire cerere culoare 2 (○)
        nP1++; P1_s[nP1] = ceas; P1_c[nP1] = 2;
        D[2] = genExp(lmd2);
        break;

    case 3: //sosire culoare 3 (■)
        nP3++; P3[nP3] = ceas;
        D[3] = genExp(lmd3);
        break;

    case 4: //sosire culoare 4 (♦)
        nP4++; P4[nP4] = ceas;
        D[4] = genExp(lmd4);
        break;

    case 5: //finalizare t5
        if (t5_c == 1) {
			nP2++; P2[nP2] = t5_s;  //timpul sosirii initiale a cererii
        }
        else {
            nP5++; P5[nP5] = t5_s;
        }
        t5_c = 0; D[5] = -1;
        break;

    case 6: //finalizare t6
        if (t6_c == 1) {
            STr[1] += ceas - t6_s;
            CT[1]++;
        }
        else {
            nP6++;
            P6_s[nP6] = t6_s;
            P6_c[nP6] = 3;
        }
        t6_c = 0; D[6] = -1;
        break;

    case 7: //finalizare t7
        if (t7_c == 2) {
            STr[2] += ceas - t7_s;
            CT[2]++;
        }
        else {
            nP6++;
            P6_s[nP6] = t7_s;
            P6_c[nP6] = 4;
        }
        t7_c = 0; D[7] = -1;
        break;

    case 9: //finalizare t9
        t9 = 1; D[9] = -1;
        break;

    case 10: //finalizare t10
        t10 = 1; D[10] = -1;
        break;
    }
}

double genExp(double lambda) {
    double u, x;
    u = (double)rand() / ((double)RAND_MAX + 1.0);
    x = -1.0 / lambda * log(1.0 - u);
    return x;
}

double genGauss(double medie, double sigma) {
    double s = 0;
    int i;
    for (i = 1; i <= 12; i++)
        s += (double)rand() / RAND_MAX;
    return medie + sigma * (s - 6);
}