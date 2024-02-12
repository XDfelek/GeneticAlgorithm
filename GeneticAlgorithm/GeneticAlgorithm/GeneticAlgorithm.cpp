#include <random>
#include <iostream> 
#include <fstream>
using namespace std;

int a = -1;
int b = 250;
int c = -10000;            //a,b,c - ax^2 + bx + c
const int ile_wyn = 40;          //liczba uruchomien programu
int lb_pop = 12;           //liczba populacji
const int ile_os = 12;           //liczba osobnikow w populacji, lb_pop * ile_os <= 150
double pr_krzyz = 0.8;    //prawdopodobienstwo krzyzowania wysokie, np 0.8
double pr_mut = 0.1;      //prawdopodobienstwo mutacji niskie, np 0.1

int osoby_lista[ile_os];

int negateBits(unsigned int n) {
    n = ~n & 0xFF;
    return n;
}

int randomNumber() {
    random_device seeder;
    ranlux48 gen(seeder());
    uniform_int_distribution<int>  uniform_0_255(0, 255);
    return uniform_0_255(gen);
}

int randomNumber(int min, int max) {
    random_device seeder;
    ranlux48 gen(seeder());
    uniform_int_distribution<int>  uniform_0_255(min, max);
    return uniform_0_255(gen);
}

int binaryToDecimal(const char* binaryString) {                 // dwojkowy -> dziesiêtny
    int decimal = 0;
    int length = strlen(binaryString);

    for (int i = 0; i < length; ++i) {
        decimal = decimal * 2 + (binaryString[i] - '0');
    }

    return decimal;
}

int binaryCrossing(int num1, int num2, int punkt_ciecia) {                //krzyzowanie
    const int Bits = 2 * sizeof num1;
    char tmp[Bits + 1];
    int num_in_decimal;

    for (int i = 0; i < punkt_ciecia; ++i)
    {
        tmp[Bits - i - 1] = '0' + num1 % 2;
        num1 /= 2;
        num2 /= 2;
    }
    for (int i = punkt_ciecia; i < Bits; ++i)
    {
        tmp[Bits - i - 1] = '0' + num2 % 2;
        num2 /= 2;
    }
    tmp[Bits] = 0;
    //cout << tmp << '\n';
    num_in_decimal = binaryToDecimal(tmp) & 0xFF;
    //cout << num_in_decimal << '\n';
    return num_in_decimal;
}

double funkcjaKwadratowa(int x) {
    return (a * x * x) + (b * x) + c;
}

void rouletteWheelSelection() {                                             //ruletka
    // Obliczanie wartoœci funkcji w celu populacji Popm
    int f_suma = 0;
    double wartosc_minimalna = 0;

    for (int i = 0; i < ile_os; i++)
    {
        if (funkcjaKwadratowa(osoby_lista[i]) < wartosc_minimalna)
            wartosc_minimalna = funkcjaKwadratowa(osoby_lista[i]);
    }

    for (int i = 0; i < ile_os; i++)
    {
        f_suma += (funkcjaKwadratowa(osoby_lista[i]) - wartosc_minimalna);
    }

    // Obliczanie prawdopodobieñstwawybrania osobnika
    double osoby_prawdopodobienstwo_lista[ile_os];
    for (int i = 0; i < ile_os; i++)
    {
        osoby_prawdopodobienstwo_lista[i] = (funkcjaKwadratowa(osoby_lista[i]) - wartosc_minimalna) / f_suma;
    }

    // Selekcja osobników metoda kola ruletki
    int selected[ile_os];

    for (int j = 0; j < ile_os; j++)
    {
        double randomNum = ((double)rand() / RAND_MAX);
        double tempCounter = 0;
        for (int i = 0; i < ile_os; i++)
        {
            tempCounter += osoby_prawdopodobienstwo_lista[i];
            if (tempCounter > randomNum)
            {
                selected[j] = osoby_lista[i];
                break;
            }
        }
    }

    // Przypisanie wybranych osobników do oryginalnej listy osobników
    for (int i = 0; i < ile_os; ++i) {
        osoby_lista[i] = selected[i];
    }
}


int main()
{
    int best[ile_wyn];      //best to najlepszy osobnik w ostatniej populacji (f_best to jego wartosc funkcji)
    ofstream myfile;
    myfile.open("wyniki.txt");



    for (int uruchomienie_programu = 0; uruchomienie_programu < ile_wyn; uruchomienie_programu++)           //ile razy sie wykona
    {
        for (int i = 0; i < ile_os; i++)        //losowanie populacji 0
        {
            osoby_lista[i] = randomNumber();
        }


        for (int populacja = 0; populacja < lb_pop; populacja++)
        {
            random_shuffle(begin(osoby_lista), end(osoby_lista));       //losowanie par

            int temp_osobnik1;
            int temp_osobnik2;
            for (int pair = 0; pair < ile_os; pair += 2) {
                if (randomNumber(0, 1000) <= pr_krzyz * 1000) {             //krzyzowanie
                    int pc = randomNumber(1, 7);
                    temp_osobnik1 = binaryCrossing(osoby_lista[pair], osoby_lista[pair + 1], pc);
                    temp_osobnik2 = binaryCrossing(osoby_lista[pair + 1], osoby_lista[pair], pc);
                    osoby_lista[pair] = temp_osobnik1;
                    osoby_lista[pair + 1] = temp_osobnik2;
                    //cout << "doszlo do krzyzowania" << '\n';
                }
                if (randomNumber(0, 1000) <= pr_mut * 1000) {               //mutacja
                    osoby_lista[pair] = negateBits(osoby_lista[pair]);
                    //cout << "doszlo do mutacji" << '\n';
                }
                if (randomNumber(0, 1000) <= pr_mut * 1000) {               //mutacja
                    osoby_lista[pair + 1] = negateBits(osoby_lista[pair + 1]);
                    //cout << "doszlo do mutacji" << '\n';
                }

            }                                                               //populacja tymczasowa po mutacji gotowa

            rouletteWheelSelection();

        }

        for (int i = 0; i < ile_os; i++)
        {
            if (funkcjaKwadratowa(osoby_lista[0]) < funkcjaKwadratowa(osoby_lista[i]))
                osoby_lista[0] = osoby_lista[i];
        }

        best[uruchomienie_programu] = osoby_lista[0];

        //cout << "doszlo do zakonczenia programu po raz " << uruchomienie_programu+1 << " najlepszy x: " << best[uruchomienie_programu] << '\n' << " najlepszy f(x): " << funkcjaKwadratowa(best[uruchomienie_programu]) << '\n';
        myfile << "f(x): " << funkcjaKwadratowa(best[uruchomienie_programu]) << "       " << "  x: " << best[uruchomienie_programu] << '\n';
    }
    //cout << "doszlo do zakonczenia symulacji" << '\n';

    for (int i = 0; i < ile_wyn; i++)
    {
        if (funkcjaKwadratowa(best[0]) < funkcjaKwadratowa(best[i]))
            best[0] = best[i];
    }
    //cout << " najlepszy x: " << best[0] << '\n';
    //cout << " najlepszy f(x): " << funkcjaKwadratowa(best[0]) << '\n';

    myfile.close();

    //system("pause");
    return 0;
}