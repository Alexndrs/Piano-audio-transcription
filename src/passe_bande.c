#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

void passe_bande(int nb_data, double* tab_amplitude_frequenciel, float fprep, float Fe)
{
    // On se propose de simplement mettre des zéros sur une bande de +/- 10 Hz autour de la fréquence qu'on souhaite éliminer
    int kprep = fprep*nb_data/Fe;
    int largeur = 10*nb_data/Fe;
    for (int k=kprep-largeur;k<kprep+largeur;k++){tab_amplitude_frequenciel[k]=0;}
}