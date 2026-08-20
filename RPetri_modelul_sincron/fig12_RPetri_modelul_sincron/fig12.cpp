//Retea Petri pentru modelul de organizare sincron
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void executaTranzitie(int te);
double genExp(double lambda);
double genGauss(double medie, double sigma);

const int nr_loc = 6, nr_t = 3;

double lambda = 7;

double miu1 = 11;
double miu2 = 10;


double sigma1 = 1. / (5 * miu1);
double sigma2 = 1. / (5 * miu2);

int M[nr_loc + 1];
double D[nr_t + 1];

double ceas;
long int CT = 0;

double Q[1000000] = { 0.0 };
int nQ = 0;
double STr = 0.0;

int maxp1 = 0;

int main() {

    srand((unsigned)time(NULL));

    // INITIALIZARE
    ceas = 0.0;
    double DS = 1e6;

    for (int i = 1; i <= nr_loc; i++) M[i] = 0;
    for (int i = 1; i <= nr_t; i++) D[i] = -1;

    M[2] = 1;              // p2 are marcaj initial 1
    D[1] = genExp(lambda); // t1

    // BUCLA PRINCIPALA A SIMULARII DISCRETE
    while (ceas < DS) {

        // FAZA DE PRIETATE A TRANZITIILOR NETEMPORIZATE
        bool net = true;
        while (net) {
            net = false;

            if (M[5] > 0 && M[6] > 0) {   // t5
                M[5]--;
                M[6]--;
                M[2]++;

                CT++;

                if (nQ > 0) {
                    STr += ceas - Q[1];
                    for (int i = 1; i < nQ; i++) Q[i] = Q[i + 1];
                    nQ--;
                }

                net = true;
            }

            if (M[1] > 0 && M[2] > 0) {   // t2
                M[1]--;
                M[2]--;
                M[3]++;
                M[4]++;

                if (D[2] < 0)
                    //D[2] = genExp(miu1); // t3
                    D[2] = genGauss(1./miu1, sigma1);
                if (D[3] < 0) 
                    //D[3] = genExp(miu2); // t4
                    D[3] = genGauss(1./miu2, sigma2);

                net = true;
            }
        }

        // STABILESTE URMATOAREA TRANZITIE TEMPORIZATA
        double t_min = 99999.9;
        int tranzitie = -1;

        for (int i = 1; i <= nr_t; i++) {
            if (D[i] >= 0 && D[i] < t_min) {
                t_min = D[i];
                tranzitie = i;
            }
        }

        if (tranzitie == -1) {
            cout << "Eroare: Blocaj sistem la momentul: " << ceas << "\n";
            break;
        }

        if (ceas + t_min > DS) {
            t_min = DS - ceas;
        }

        // AVANSEAZA CEASUL SIMULARII
        ceas += t_min;

        for (int i = 1; i <= nr_t; i++) {
            if (D[i] >= 0) {
                D[i] -= t_min;
            }
        }

        if (ceas >= DS) break;

        // EXECUTA TRANZITIA TEMPORIZATA STABILITA
        executaTranzitie(tranzitie);
    }

    // CALCUL SI AFISARE STATISTICI FINALE
    cout << "DS: " << ceas << "\n\n";
  //  cout << "Cereri iesite din sistem: " << CT << "\n\n";

    if (CT > 0) {
        double Trm = STr / (double)CT;
        cout << "Trm: " << Trm << "\n\n";
    }

    cout << "maxp1: " << maxp1 << "\n\n";

    return 0;
}

void executaTranzitie(int te) {
    switch (te) {
    case 1:     // t1 - SOSIRE CERERE
        M[1]++;
        if (M[1] > maxp1) maxp1 = M[1];

        nQ++;
        Q[nQ] = ceas;

        D[1] = genExp(lambda);
        break;

    case 2:     // t3 - FINALIZARE PRELUCRARE LA PROCESORUL 1
        M[3]--;
        M[5]++;
        D[2] = -1;
        break;

    case 3:     // t4 - FINALIZARE PRELUCRARE LA PROCESORUL 2
        M[4]--;
        M[6]++;
        D[3] = -1;
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
    //return (val > 0) ? val : 0.001; val = medie + sigma * (s - 6)
}