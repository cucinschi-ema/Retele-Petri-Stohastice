//Retea Petri pentru modelul de organizare paralel
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

double genExp(double lambda);
double genGauss(double medie, double sigma);
void executaTranzitie(int te);

const int nr_loc = 4, nr_t = 3;

double lambda = 7;
double miu1 = 11;
double miu2 = 10;
double sigma1 = 1. / (5 * miu1);
double sigma2 = 1. / (5 * miu2);

int M[nr_loc + 1];
double D[nr_t + 1];

double ceas;
long int CT = 0;

double Q[100000] = { 0.0 };
int nQ = 0;
double STr = 0.0;

int maxp1 = 0;
int maxp2 = 0;

int main() {

    srand((unsigned)time(NULL));

    // INITIALIZARE
    ceas = 0.0;
    double DS = 1e6;

    for (int i = 1; i <= nr_loc; i++) M[i] = 0;
    for (int i = 1; i <= nr_t; i++) D[i] = -1;

    D[1] = genExp(lambda);

    // BUCLA PRINCIPALA A SIMULARII DISCRETE
    while (ceas < DS) {

        while (M[3] > 0 && M[4] > 0) {   //execut toate tranzitiile netemporizate t4 posibile
            M[3]--;
            M[4]--;
            CT++;

            if (nQ > 0) {
                STr += ceas - Q[1];
                for (int i = 1; i < nQ; i++) Q[i] = Q[i + 1];
                nQ--;
            }
        }

        //STABILESTE URMATOAREA TRANZITIE TEMPORIZATA
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

        executaTranzitie(tranzitie);
    }

    // CALCUL SI AFISARE STATISTICI FINALE
    cout << " Durata simularii: " << ceas << "\n\n";
    //cout << "Cereri iesite din sistem: " << CT << "\n\n";

    if (CT > 0) {
        double Trm = STr / (double)CT;
        cout << "Trm: " << Trm << "\n";
    }

    cout << "max p1: " << maxp1 << "\n";
    cout << "max p2: " << maxp2 << "\n";

    return 0;
}

void executaTranzitie(int te) {
    switch (te) {
        
    case 1:     //SOSIRE PERECHE 
        M[1]++;
        M[2]++;

        if (M[1] > maxp1) maxp1 = M[1];
        if (M[2] > maxp2) maxp2 = M[2];

        nQ++;
        Q[nQ] = ceas;

        D[1] = genExp(lambda);

        if (M[1] >= 1 && D[2] < 0) {
            //D[2] = genExp(miu1);
            D[2] = genGauss(1./miu1, sigma1);
        }

        if (M[2] >= 1 && D[3] < 0) {
            //D[3] = genExp(miu2);
            D[3] = genGauss(1./miu2, sigma2);
        }
        break;

    case 2:     //FINALIZARE PRELUCRARE
        M[1]--;
        M[3]++;

        if (M[1] > 0) {
            //D[2] = genExp(miu1);
            D[2] = genGauss(1./miu1, sigma1);
        }
        else {
            D[2] = -1;
        }
        break;

    case 3:     //FINALIZARE PRELUCRARE
        M[2]--;
        M[4]++;

        if (M[2] > 0) {
            //D[3] = genExp(miu2);
            D[3] = genGauss(1./miu2, sigma2);
        }
        else {
            D[3] = -1;
        }
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