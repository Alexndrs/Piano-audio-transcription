#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "lect_audio.h"
#include "fenetrage_hamming.h"
#include "Transformee_Fourier.h"
#include "freq_preponderante.h"



int main(){
    printf("TEST0\n");
    char* nom_fichier = "test_audio.wav";
    FILE *wav = fopen(nom_fichier,"rb");
    printf("\n nom_fichier : %s\n", nom_fichier);
    int len_tab;
    float Fe;
    nb_data_Fe(wav, &len_tab, &Fe);
    printf("\n nombre d'echantillons et fréquence : %d, %f\n\n",len_tab,Fe);
    if (len_tab == -1){return 0;} //Erreur de lecture -> le programme s'arrête



    //  RECUPERATION DES TEMPS D'ECHANTILLONAGE ET DES AMPLITUDES ASSOCIEES
    printf("TEST1\n");
    float* tab_temps = malloc(sizeof(float) * len_tab);
    short* tab_amplitude = malloc(sizeof(short) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){printf("Données trop volumineuses\n"); return 0;}
    wav = fopen(nom_fichier,"rb");
    printf("TEST2\n");
    rempli_tab(wav, tab_temps, tab_amplitude);
    printf("TEST3\n");


    // Boucle de traitement : fenetrage, transformée de fourier, fréquence détectée
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.2;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0.2;
    float t2 = 0.4;
    float* tab_frequence = malloc(sizeof(float)*len_tab);
    short* amplitude_fenetree = malloc(sizeof(short)*len_tab);
    double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);          // La fonction Transformee_Fourier ne s'applique que sur des tableaux de double et ces tableaux sont composés de nombres complexes d'où le 2*sizeof(double)*len_tab.
    while (t2<T_total)
    {
        printf("TEST4\n");
        // Fenetrage
        fenetrage_hamming(tab_amplitude, amplitude_fenetree, len_tab, Fe, t1, t2);
        printf("TEST5\n");

        // Transformée de fourier
        FourierTransform(amplitude_fenetree, len_tab, amplitude_fourier);
        printf("TEST6\n");
        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 50; //seuil en dB pour l'étude des fréquences
        float fprep = frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab);
        printf("TEST7\n");
        // Mise des notes dans un tableau
        printf("[t1,t2] = [%f,%f],  fprep = %f\n", t1,t2,fprep);

        t1 = t1 + tau;
        t2 = t2 + tau;
    }

    free(amplitude_fenetree);
    free(amplitude_fourier);
    free(tab_frequence);
    free(tab_temps);
    free(tab_amplitude);

    return 0;
}