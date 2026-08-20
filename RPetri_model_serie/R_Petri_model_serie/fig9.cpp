//Retea Petri pentru modelul serie cu capacitate nelimitata
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

void executaTranzitie(int te);
double genExp(double lambda);
double genGauss(double medie, double sigma);

const int nr_loc = 2, nr_t = 3;

double lambda = 10;

double miu1 = 11;
double miu2 = 10;

double sigma1 = 1. / (5 * miu1);
double sigma2 = 1. / (5 * miu2);

int M[nr_loc + 1];
double D[nr_t + 1];
double STr = 0.0;

long int CT = 0;
double ceas;

int maxp1 = 0;
int maxp2 = 0;

int main() {

    srand((unsigned)time(NULL));

    double DS = 1e6;
    ceas = 0.0;

    for (int i = 1; i <= nr_loc; i++) M[i] = 0;
    for (int i = 1; i <= nr_t; i++) D[i] = -1;

    D[1] = genExp(lambda);

    while (ceas < DS) {

        double t_min = 99999;
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

        if (ceas + t_min > DS) {   //opresc simularea exact la DS
            t_min = DS - ceas;
        }

        STr += (double)(M[1] + M[2]) * t_min;

        ceas += t_min;
        for (int i = 1; i <= nr_t; i++) {
            if (D[i] >= 0) {
                D[i] -= t_min;
            }
        }

        if (ceas >= DS) break;

        executaTranzitie(tranzitie);
    }

    cout << "\nDurata simularii: " << ceas << "\n\n";

    if (CT > 0) {
        double Trm = STr / (double)CT;
        cout << "Trm: " << Trm << "\n";
    }

    cout << "\nmaxp1: " << maxp1 << "\n";
    cout << "maxp2: " << maxp2 << "\n";

    return 0;
}

void executaTranzitie(int te) {

    switch (te) {

    case 1:     //sosire client in statia 1
        M[1]++;

        if (M[1] > maxp1) maxp1 = M[1];

        D[1] = genExp(lambda);

        if (M[1] == 1) {
            D[2] = genExp(miu1);
            //D[2] = genGauss(1./miu1, sigma1);
        }
        break;

    case 2:     //trecerea de la statia 1 la statia 2
        M[1]--;
        M[2]++;

        if (M[2] > maxp2) maxp2 = M[2];

        if (M[1] > 0)
            D[2] = genExp(miu1);
            //D[2] = genGauss(1./miu1, sigma1);
        else
            D[2] = -1;

        if (M[2] == 1)
            D[3] = genExp(miu2);
            //D[3] = genGauss(1./miu2, sigma2);
        break;

    case 3:     //iesirea din sistem
        M[2]--;
        CT++;

        if (M[2] > 0)
            D[3] = genExp(miu2);
            //D[3] = genGauss(1./miu2, sigma2);
        else
            D[3] = -1;
        break;
    }
}

double genExp(double lambda) {
    double u, x;
    u = (double)rand() / (RAND_MAX + 1.0);
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