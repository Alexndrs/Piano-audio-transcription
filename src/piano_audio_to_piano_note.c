#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "lect_audio.c"
#include "fenetrage_hamming.c"
#include "Transformee_Fourier.c"
#include "freq_preponderante.c"



int main(int argc, char *argv[]){

    char* nom_fichier = "test_audio.wav";
    FILE *wav = fopen(nom_fichier,"rb");
    printf("\n nom_fichier : %s\n", nom_fichier);
    int len_tab;
    float Fe;
    nb_data_Fe(wav, &len_tab, &Fe);
    printf("\n nombre d'echantillons et fréquence : %d, %f\n\n",len_tab,Fe);
    if (len_tab == -1){return 0;} //Erreur de lecture -> le programme s'arrête



    //  RECUPERATION DES TEMPS D'ECHANTILLONAGE ET DES AMPLITUDES ASSOCIEES
    float* tab_temps = malloc(sizeof(float) * len_tab);
    float* tab_amplitude = malloc(sizeof(float) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){printf("Données trop volumineuses\n"); return 0;}
    wav = fopen(nom_fichier,"rb");
    rempli_tab(wav, tab_temps, tab_amplitude);



    // Boucle de traitement : fenetrage, transformée de fourier, fréquence détectée
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.2;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0.2;
    float t2 = 0.4;
    float* tab_frequence = malloc(sizeof(float)*len_tab);
    float* amplitude_fenetree = malloc(sizeof(float)*len_tab);
    double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);          // La fonction Transformee_Fourier ne s'applique que sur des tableaux de double et ces tableaux sont composés de nombres complexes d'où le 2*sizeof(double)*len_tab.
    printf("Test1\n");
    while (t2<T_total)
    {
        
        printf("Test2\n");
        // Fenetrage
        fenetrage_hamming(tab_amplitude, amplitude_fenetree, len_tab, Fe, t1, t2);
        
        for(int i=0; i<200; i++){
            printf("%f ;",amplitude_fenetree[100*i]);
        }
        printf("\n");


        printf("Test3\n");
        // Transformée de fourier
        FourierTransform(amplitude_fenetree, len_tab, amplitude_fourier);
        printf("Test4\n\n");

        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 50; //seuil en dB pour l'étude des fréquences
        float fprep = frequence_preponderante(amplitude_fourier,tab_frequence,seuil,H,len_tab);

        // Mise des notes dans un tableau
        printf("[t1,t2] = [%f,%f],  fprep = %f\n", t1,t2,fprep);

        t1 = t1 + tau;
        t2 = t2 + tau;

    }
    free(amplitude_fenetree);
    free(amplitude_fourier);
    free(tab_frequence);

    return 0;
}