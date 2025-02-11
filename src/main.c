/*
Compilation dans le fichier ~/Piano-audio-transcription:

    Pour Alexandre
    gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2

    Pour Tadeusz
    gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I src2/include -L src2/lib -lmingw32 -lSDL2main -lSDL2 

pour générer un prog.exe sans fenetre de commande qui s'ouvre:
  gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lm

include GSL L- lib -lgslcblas -lgsl -lm -o bin/test.exe : 


gcc -Wall -Werror -Wfatal-errors src/main.c -o bin/prog.exe -I include -L lib -lgslcblas -lgsl -lmingw32 -lSDL2main -lSDL2 -lm

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


/*-----------Ecriture dans un fichier .txt ----------*/
/*
   Ces fonctions vont servir à tester nos autres fonctions en enregistrant des tableaux dans des fichier .txt
   qui seront lu en python pour pouvoir tracer des courbes.
*/

void float_tab_to_txt(float* tab, int nb_elem, char* nom_tableau) {
    FILE *fichier = fopen(nom_tableau, "w");
    
    for (int i = 0; i < nb_elem; i++) {
        fprintf(fichier, "%f\n", tab[i]);
    }
    
    fclose(fichier);
}

void short_tab_to_txt(short* tab, int nb_elem, char* nom_tableau) {
    FILE *fichier = fopen(nom_tableau, "w");
    
    for (int i = 0; i < nb_elem; i++) {
        if (tab[i] > 0)
            fprintf(fichier, "%hd\n", tab[i]);
        else fprintf(fichier, "%hd\n", 0);
    }
    
    fclose(fichier);
}

/*------------------  Fonction lect_audio.c  ----------------*/

struct wavfile //définit la structure de l entete d un wave
{
    char        id[4];          // doit contenir "RIFF"
    int         totallength;        // taille totale du fichier moins 8 bytes
    char        wavefmt[8];     // doit etre "WAVEfmt "
    int         format;             // 16 pour le format PCM
    short       pcm;              // 1 for PCM format
    short       channels;         // nombre de channels
    int         frequency;          // frequence d echantillonage
    int         bytes_per_second;   // nombre de bytes par secondes
    short       bytes_by_capture;
    short       bits_per_sample;  // nombre de bytes par echantillon
    char        data[4];        // doit contenir "data"
    int         bytes_in_data;      // nombre de bytes de la partie data
};



void nb_data_Fe(FILE *wav, int* p_nb_data, float* p_Fe){
    //creation et initialisation du header
    struct wavfile header;
    

    //Gestion des erreurs de lecture
    if ( wav == NULL ){
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        *p_nb_data = -1;
        return;
    }
    if ( fread(&header,sizeof(header),1,wav) < 1 ){
        printf("\nne peut pas lire le header\n");
        *p_nb_data = -1;
        return;
    }
    if ((header.id[0] != 'R')||(header.id[1] != 'I')||(header.id[2] != 'F')||(header.id[3] != 'F')){
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        *p_nb_data = -1;
        return;
    }
    if (header.channels!=1){
        printf("\nerreur : le fichier n'est pas mono\n");
        *p_nb_data = -1;
        return;
    }

    //Si tout se passe bien...
    *p_nb_data = (header.bytes_in_data/header.bytes_by_capture);
    *p_Fe = header.frequency;
}



void rempli_tab(FILE *wav, float* tab_temps, short* tab_amplitudes){
    //creation et initialisation du header
    struct wavfile header;
    

    //Gestion des erreurs de lecture
    if ( wav == NULL ){
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        return ;
    }
    if ( fread(&header,sizeof(header),1,wav) < 1 ){
        printf("\nne peut pas lire le header\n");
        return ;
    }
    if ((header.id[0] != 'R')||(header.id[1] != 'I')||(header.id[2] != 'F')||(header.id[3] != 'F')){
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        return ;
    }
    if (header.channels!=1){
        printf("\nerreur : le fichier n'est pas mono\n");
        return ;
    }

    //Remplissage du tableau
    int i=0;
    short value=0;
    while(fread(&value,(header.bits_per_sample)/8,1,wav)){
        //lecture des echantillons et enregistrement dans le tableau (en dB)
        tab_amplitudes[i] = 20*log10(fabs(value));
        if (tab_amplitudes[i] < 0){tab_amplitudes[i] = 0;}
        tab_temps[i] = 1.*i/header.frequency;
        i++;
    }
}


/*-------------  Fonction de fenetrage_hamming.c  -----------------*/

void fenetrage_hamming(short* tab_amplitude, short* amplitude_fenetree, int nb_data, float Fe, float t1, float t2)
{
    float Te = 1/Fe;
    for(int k=0; k < nb_data; k++)
    {
        float t = k*Te;
        if(t>t1 && t<t2)
        {
            amplitude_fenetree[k] = tab_amplitude[k]*(0.54 - 0.46 * cos(2*3.141592653589793*(t-t1)/(t2 - t1)));
        }
        else
        { 
            amplitude_fenetree[k] = 0;
        }
    }
}

/*-----------  Fonction de transformee_fourier.c  ---------------*/

#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>


void FourierTransform(short* tab_amplitudes_temporel, int nb_data, double* tab_amplitude_frequenciel){
    /*Transformée de Fourier d'un tableau d'amplitudes*/


    //On complete notre tableau avec des 0 pour avoir un tableau de longueur une puissance de 2
    int nb_data_power_2 = pow(2, ceil(log2(nb_data)));
    double *pow2tab_data = malloc(nb_data_power_2*sizeof(double));
      
      
    for (int i = 0; i < nb_data; i++) {
        pow2tab_data[i] = tab_amplitudes_temporel[i];
    }


  gsl_fft_real_radix2_transform(pow2tab_data, 1, nb_data_power_2);

  for (int i = 0; i < nb_data; i++) {
    tab_amplitude_frequenciel[i] = pow2tab_data[i];
  }

    free(pow2tab_data);
}



/*------------  fonction de freq_preponderante.c  -----------*/
int frequence_preponderante(double* tab_amplitude,int Fe,float seuil, int H, int len_tab)
{
    int fprep = -1;
    float amp_max = seuil;
    int f;
    float prod_spec;
    for(int k = 40*(len_tab/Fe); k < (len_tab/H) ;k++) //C'est la première fréquence audible tel que (k/len_tab)*Fe > 20Hz
    {
        f = fabs(floor(k*Fe/len_tab));
        prod_spec = 1;
        for (int i = 1; i <= H; i++)        // On indice de 1 à H compris pour prendre en compte le fondamental
        {
            double val = tab_amplitude[i*k];
            prod_spec *=(val*val);
        }

        if(log10(prod_spec) > amp_max)
        {
            amp_max = log10(prod_spec);
            fprep = f;
        }
    }
    if (fprep < -1){printf("Il ne s'est rien passe\n");}
    printf("H=%d,freq_detectee = %d, prod = %f, seuil =%f\n",H, fprep, amp_max, seuil);
    return (fprep); 
}

/*-------- fonction coupe_bande  -------------*/

void coupe_bande(int nb_data, double* tab_amplitude_frequenciel, float fprep, float Fe)
{
    // On se propose de simplement mettre des zéros sur une bande de +/- 10 Hz autour de la fréquence qu'on souhaite éliminer
    int kprep = fprep*nb_data/Fe;
    int largeur = 10*nb_data/Fe;
    for (int k=kprep-largeur;k<kprep+largeur;k++){tab_amplitude_frequenciel[k]=0;}
}


/*--------  fonction frequence_to_note  ----------*/


int frequency_to_note_number(int freq){
    /*Fonction en escalier qui convertit les fréquences en note (numéro de touche du piano)*/

if (freq < 28){
return 0;
}
else{
if (freq <30){
return 1;
}
else{
if (freq < 31){
return 2;
}
else{
if (freq < 33){
return 3;
}
else{
if (freq < 35){
return 4;
}
else{
if (freq < 37){
return 5;
}
else{
if (freq < 39){
return 6;
}
else{
if (freq < 42){
return 7;
}
else{
if (freq < 44){
return 8;
}
else{
if (freq < 47){
return 9;
}
else{
if (freq < 49){
return 10;
}
else {
if (freq < 52){
return 11;
}
else{
if (freq < 56){
return 12;
}
else {
if (freq < 59){
return 13;
}
else{
if (freq < 62){
return 14;
}
else{
if (freq < 66){
return 15;
}
else{
if (freq < 70){
return 16;
}
else {
if (freq < 74){
return 17;
}
else{
if (freq < 78){
return 18;
}
else{
if (freq < 83){
return 20;
}
else{

if (freq < 88){
return 21;
}
else {
if (freq < 93){
return 22;
}
else{
if (freq < 104){
return 23;
}
else{
if (freq < 111){
return 24;
}
else{
if (freq < 117){
return 25;
}
else{
if (freq < 124){
return 26;
}
else {
if (freq < 131){
return 27;
}
else {
if (freq < 139){
return 28;
}
else {
if (freq < 147){
return 29;
}
else{
if (freq < 160){
return 30;
}
else{
if (freq < 169){
return 31;
}
else{
if (freq < 179){
return 32;
}
else{
if (freq < 189){
return 33;
}
else{
if (freq < 200){
return 34;
}
else{
if (freq < 215){
return 35;
}
else{
if (freq < 228){
return 36;
}
else{
if (freq < 240){
return 37;
}
else{
if (freq < 252){
return 38;
}
else{
if (freq < 269){
return 39;
}
else{
if (freq < 284){
return 40;
}
else{
if (freq < 302){
return 41;
}
else{
if (freq < 320){
return 42;
}
else{
if (freq < 340){
return 43;
}
else{
if (freq < 355){
return 44;
}
else{
if (freq < 385){
return 45;
}
else{
if (freq < 405){
return 46;
}
else{
if (freq < 430){
return 47;
}
else{
if (freq < 450){
return 48;
}
else{
if (freq < 480){
return 49;
}
else{
if (freq < 510){
return 50;
}
else{
if (freq < 540){
return 51;
}
else {
if (freq < 565){
return 52;
}
else {
if (freq < 605){
return 53;
}
else{
if (freq < 640){
return 54;
}
else{
if (freq < 675){
return 55;
}
else{
if (freq < 720){
return 56;
}
else{
if (freq < 760){
return 57;
}
else{
if (freq < 805){
return 58;
}
else {
if (freq < 850){
return 59;
}
else{
if (freq < 900){
return 60;
}
else{
if (freq < 955){
return 61;
}
else{
if (freq < 1020){
return 62;
}
else{
if (freq < 1075){
return 63;
}
else{
if (freq < 1130){
return 64;
}
else{
if (freq < 1205){
return 65;
}
else{
if (freq < 1285){
return 66;
}
else{
if (freq < 1355){
return 67;
}
else{
if (freq < 1435){
return 68;
}
else{
if (freq < 1515){
return 69;
}
else{
if (freq < 1610){
return 70;
}
else{
if (freq < 1710){
return 71;
}
else{
if (freq < 1810){
return 72;
}
else{
if (freq < 1920){
return 73;
}
else{
if (freq < 2015){
return 74;
}
else{
if (freq < 2150){
return 75;
}
else{
if (freq < 2290){
return 76;
}
else{
if (freq < 2400){
return 77;
}
else{
if (freq < 2575){
return 78;
}
else{
if (freq < 2700){
return 79;
}
else{
if (freq < 2850){
return 80;
}
else{
if (freq < 3050){
return 81;
}
else{
if(freq < 3240){
return 82;
}
else{
if(freq < 3450){
return 83;
}
else{
if (freq < 3650){
return 84;
}
else{
if (freq < 3840){
return 85;
}
else{
if (freq < 4050){
    return 86;
}
else{
    return 87;
}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}

/*--------  fonction piano_audio_to_piano_notes  ----------*/

/*
void piano_audio_to_piano_note(char* nom_fichier_audio, int** tab_temps_notes, int** tab_notes, int* nb_notes_jouees){

    FILE *wav = fopen(nom_fichier_audio,"rb");
    printf("\n nom_fichier : %s\n", nom_fichier_audio);
    int len_tab;
    float Fe;
    nb_data_Fe(wav, &len_tab, &Fe);
    printf("\n nombre d'echantillons et frequence : %d, %f\n\n",len_tab,Fe);
    if (len_tab == -1){
        printf("Erreur de lecture\n");
        *tab_temps_notes = NULL;
        *tab_notes = NULL;
        return ;
    }

    //  RECUPERATION DES TEMPS D'ECHANTILLONAGE ET DES AMPLITUDES ASSOCIEES
    float* tab_temps = malloc(sizeof(float) * len_tab);
    short* tab_amplitude = malloc(sizeof(int) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){
        printf("Données trop volumineuses\n");
        *tab_temps_notes = NULL;
        *tab_notes = NULL;
        return ;
    }
    wav = fopen(nom_fichier_audio,"rb");
    rempli_tab(wav, tab_temps, tab_amplitude);


    // Boucle de traitement : fenetrage, transformée de fourier, fréquence détectée
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.1;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0;
    float t2 = t1 + tau;
    // float* tab_frequence = malloc(sizeof(float)*len_tab);
    // short* amplitude_fenetree = malloc(sizeof(short)*len_tab);
    // double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);          // La fonction Transformee_Fourier ne s'applique que sur des tableaux de double et ces tableaux sont composés de nombres complexes d'où le 2*sizeof(double)*len_tab.
    // printf("test apres les mallocs pour les tab fenetrees\n");

    float* tab_frequence = malloc(sizeof(float)*len_tab);
    if (tab_frequence == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour tab_frequence\n");
        exit(1);
    }
    // printf("malloc tab_frequence reussie\n");


    short* amplitude_fenetree = malloc(sizeof(int)*len_tab);
    if (amplitude_fenetree == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour amplitude_fenetree\n");
        exit(1);
    }
    // printf("malloc amplitude_fenetree reussie\n");

    double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);
    if (amplitude_fourier == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour amplitude_fourier\n");
        exit(1);
    }
    // printf("malloc amplitude_fourrier reussie\n");

// Vérification : affiche un message si l'allocation de mémoire a réussi
// printf("Allocation de mémoire réussie pour les tableaux\n");

    int i = 0;
    *nb_notes_jouees = 0;
    printf("Test0");
    while (t2<T_total)
    {

        // Fenetrage
        // printf("Test avant fenetrage_haming");
        fenetrage_hamming(tab_amplitude, amplitude_fenetree, len_tab, Fe, t1, t2);
        // printf("Test apres fenetrage_haming");

        // Transformée de fourier
        // printf("Test avant fourierTranform");
        FourierTransform(amplitude_fenetree, len_tab, amplitude_fourier);
        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 33; //seuil en dB pour l'étude des fréquences
        int fprep;
    //     while (frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab) != -1){
    //         int new_fprep = frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab);
    //         printf(" new_fprep = %d, ",new_fprep);
    //         if (t1 > 0){
    //             if (fprep != -1 && ((new_fprep % fprep < 10) || fabs(fprep - (new_fprep % fprep)) < 10)){
    //                 //Alors on a surement une erreur de détection de la première harmonique mais c'est la même fréquence qui est joué qu'avant
    //                 new_fprep = fprep;
    //             }
    //         }

    //         fprep = new_fprep;

    //         // Mise des notes dans le tableau
    //         printf("  [t1,t2] = [%f,%f],  fprep = %d\n", t1,t2,fprep);
    //         (*tab_notes)[i] = frequency_to_note_number(fprep);
    //         (*tab_temps_notes)[i] = floor(1000 * (t1 + t2)/2); //Tableau en milisecondes 
    //         i++;
    //         (*nb_notes_jouees)++;
            
    //         printf("\n\n");
    //         coupe_bande(len_tab, amplitude_fourier, fprep, Fe);
    //     }

    //     t1 = t1 + tau;
    //     t2 = t2 + tau;
    // }
        int new_fprep = frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab);
        printf(" new_fprep = %d, ",new_fprep);
        if (t1 > 0){
            if (new_fprep != -1 && fprep != -1 && ((new_fprep % fprep < 10) || fabs(fprep - (new_fprep % fprep)) < 10)){
                //Alors on a surement une erreur de détection de la première harmonique mais c'est la même fréquence qui est joué qu'avant
                new_fprep = fprep;
            }
        }

        fprep = new_fprep;

        // Mise des notes dans le tableau
        printf("  [t1,t2] = [%f,%f],  fprep = %d\n", t1,t2,fprep);
        if (fprep != -1){
            (*tab_notes)[i] = frequency_to_note_number(fprep);
            (*tab_temps_notes)[i] = floor(1000 * (t1 + t2)/2); //Tableau en milisecondes 
            i++;
            (*nb_notes_jouees)++;
            
            printf("\n\n");

        }
        t1 = t1 + tau;
        t2 = t2 + tau;
    }

    printf("Test1");
    free(tab_temps);
    printf("Test2");
    free(amplitude_fenetree);
    printf("Test3");
    free(amplitude_fourier);
    printf("Test4");
    free(tab_frequence);
    printf("Test5");
    free(tab_amplitude);
    printf("Test6");
}

*/

void piano_audio_to_piano_note(char* nom_fichier_audio, int** tab_temps_notes, int** tab_notes, int* nb_notes_jouees){

    FILE *wav = fopen(nom_fichier_audio,"rb");
    printf("\n nom_fichier : %s\n", nom_fichier_audio);
    int len_tab;
    float Fe;
    nb_data_Fe(wav, &len_tab, &Fe);
    printf("\n nombre d'echantillons et frequence : %d, %f\n\n",len_tab,Fe);
    if (len_tab == -1){
        printf("Erreur de lecture\n");
        *tab_temps_notes = NULL;
        *tab_notes = NULL;
        return ;
    }

    //  RECUPERATION DES TEMPS D'ECHANTILLONAGE ET DES AMPLITUDES ASSOCIEES
    float* tab_temps = malloc(sizeof(float) * len_tab);
    short* tab_amplitude = malloc(sizeof(int) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){
        printf("Données trop volumineuses\n");
        *tab_temps_notes = NULL;
        *tab_notes = NULL;
        return ;
    }
    wav = fopen(nom_fichier_audio,"rb");
    rempli_tab(wav, tab_temps, tab_amplitude);


    // Boucle de traitement : fenetrage, transformée de fourier, fréquence détectée
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.1;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0;
    float t2 = t1 + tau;
    // float* tab_frequence = malloc(sizeof(float)*len_tab);
    // short* amplitude_fenetree = malloc(sizeof(short)*len_tab);
    // double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);          // La fonction Transformee_Fourier ne s'applique que sur des tableaux de double et ces tableaux sont composés de nombres complexes d'où le 2*sizeof(double)*len_tab.
    // printf("test apres les mallocs pour les tab fenetrees\n");

    float* tab_frequence = malloc(sizeof(float)*len_tab);
    if (tab_frequence == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour tab_frequence\n");
        exit(1);
    }
    // printf("malloc tab_frequence reussie\n");


    short* amplitude_fenetree = malloc(sizeof(int)*len_tab);
    if (amplitude_fenetree == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour amplitude_fenetree\n");
        exit(1);
    }
    // printf("malloc amplitude_fenetree reussie\n");

    double* amplitude_fourier = malloc(2*sizeof(double)*len_tab);
    if (amplitude_fourier == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour amplitude_fourier\n");
        exit(1);
    }
    // printf("malloc amplitude_fourrier reussie\n");

// Vérification : affiche un message si l'allocation de mémoire a réussi
// printf("Allocation de mémoire réussie pour les tableaux\n");

    int i = 0;
    *nb_notes_jouees = 0;
    printf("Test0");
    while (t2<T_total)
    {

        // Fenetrage
        // printf("Test avant fenetrage_haming");
        fenetrage_hamming(tab_amplitude, amplitude_fenetree, len_tab, Fe, t1, t2);
        // printf("Test apres fenetrage_haming");

        // Transformée de fourier
        // printf("Test avant fourierTranform");
        FourierTransform(amplitude_fenetree, len_tab, amplitude_fourier);
        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 33; //seuil en dB pour l'étude des fréquences
        int new_fprep = frequence_preponderante(amplitude_fourier,Fe,seuil,H,len_tab);
        printf(" new_fprep = %d, ",new_fprep);
        int fprep;
        if (t1 > 0){
            if (new_fprep != -1 && fprep != -1 && ((new_fprep % fprep < 10) || fabs(fprep - (new_fprep % fprep)) < 10)){
                //Alors on a surement une erreur de détection de la première harmonique mais c'est la même fréquence qui est joué qu'avant
                new_fprep = fprep;
            }
        }
        fprep = new_fprep;

        // Mise des notes dans le tableau
        if (fprep != -1){
            //Après des testes on a constaté que les fréquences détectés par la transformés était en moyenne supérieur de 20Hz à la fréquence de la vraoe note
            int note = frequency_to_note_number(fprep-30);
            (*tab_notes)[i] = note;
            printf("  [t1,t2] = [%f,%f],  fprep = %d, note=%d\n\n\n", t1,t2,fprep,note);
            (*tab_temps_notes)[i] = floor(1000 * (t1 + t2)/2); //Tableau en milisecondes 
            i++;
            (*nb_notes_jouees)++;
        }

        t1 = t1 + tau;
        t2 = t1 + tau;
    }
    printf("Test1");
    free(tab_temps);
    printf("Test2");
    free(amplitude_fenetree);
    printf("Test3");
    free(amplitude_fourier);
    printf("Test4");
    free(tab_frequence);
    printf("Test5");
    free(tab_amplitude);
    printf("Test6");
}



/*--------  fonctions pour la partie graphique  -----------*/

#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void SDL_ExitWithError(char* message)
{
    SDL_Log("ERREUR : %s > %s\n",message,SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}



void draw_faded_rectangle(SDL_Renderer *renderer,int x, int y,int width, int r,int g, int b,int r_bg, int g_bg, int b_bg)
{
    /* Dessine un rectangle centré en x,y de largeur width, de hauteur Window_height/4 et faded en haut de couleur r,g,b*/
    int h = WINDOW_HEIGHT/4;
    for (int i = x-width/2; i < x+width/2;i++){
        for(int j=y-h/2; j<y+h/2;j++){
            if (SDL_SetRenderDrawColor(renderer,(2*(r-r_bg)*(j-y) + h*(r_bg + r))/(2*h),(2*(g-g_bg)*(j-y) + h*(g_bg + g))/(2*h),(2*(b-b_bg)*(j-y) + h*(b_bg + b))/(2*h),SDL_ALPHA_OPAQUE))
                SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
            if (SDL_RenderDrawPoint(renderer, i , j) != 0)
                SDL_ExitWithError("Impossible de dessiner un point");

        }
    }

}



void draw_rect(SDL_Renderer *renderer,int x, int y,int width,int height, int r,int g, int b)
{
    /* Dessine un rectangle centré en x,y de largeur width, de hauteur height, de couleur r,g,b*/
    for (int i = x-width/2; i < x+width/2;i++){
        for(int j=y-height/2; j<y+height/2;j++){
            if (SDL_SetRenderDrawColor(renderer,r,g,b,SDL_ALPHA_OPAQUE))
                SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
            if (SDL_RenderDrawPoint(renderer, i , j) != 0)
                SDL_ExitWithError("Impossible de dessiner un point");
        }
    }
}

int is_white_note(int numero_note){
    switch (numero_note % 12) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
            return 1;
            break;
        case 1:
        case 4:
        case 6:
        case 9:
        case 11:
            return 0;
            break;
        default:
            return -1;
            break;
    }
}

int x_pos_of_note(int numero_note, int largeur_note_blanche, int largeur_note_noire){
    int nb_white_note_before = 0;
    for (int i = 0; i < numero_note; i++){
        if (is_white_note(i)==1){ nb_white_note_before++;}
    }
    if (is_white_note(numero_note)==1){return(largeur_note_blanche*nb_white_note_before+largeur_note_blanche/2);}
    else{return(largeur_note_blanche*nb_white_note_before);}
}

void update_renderer(SDL_Renderer *renderer, int t, int* tab_temps, int* tab_notes, int indice_derniere_note_en_cours,int animation_time,int nb_notes)
{
    int largeur_note_blanche = WINDOW_WIDTH/52; //Il y'a 52 touches blanches sur un piano
    int hauteur_note_blanche = WINDOW_HEIGHT/10;
    int largeur_note_noire = largeur_note_blanche/2; //les touches noires sont un peu plus petites que les blanches
    int hauteur_note_noire = hauteur_note_blanche*3./4; //et plus courtes

    //On dessine toutes les notes blanches puis dans un second temps les notes noirs pour qu'elles soient dessinés par dessus:
    for (int i=0; i<87; i++){
        int x = x_pos_of_note(i, largeur_note_blanche, largeur_note_noire);
        if(is_white_note(i)){
            draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,255,255);
        }
    }
    for (int i=0; i<87; i++){
        int x = x_pos_of_note(i, largeur_note_blanche, largeur_note_noire);
        if(is_white_note(i)==0){
            draw_rect(renderer,x,WINDOW_HEIGHT - hauteur_note_noire/2 - (hauteur_note_blanche-hauteur_note_noire),largeur_note_noire,hauteur_note_noire,20,20,20);
        }
    }
    
    //Notes en cours d'animation
    int i = indice_derniere_note_en_cours;
    while((tab_temps[i] < t)&(i < nb_notes))
    {   //La note i est en cours d'animation... et (t-tab_temps[i])/animation_time permet de situer où elle en est exactement dans son animation
        int x =  x_pos_of_note(tab_notes[i], largeur_note_blanche, largeur_note_noire);
        int y = (WINDOW_HEIGHT-hauteur_note_blanche)*(t-tab_temps[i])/animation_time;

        int isWhiteNote = is_white_note(tab_notes[i]);

        if(isWhiteNote == 1){
            if((tab_temps[i] + animation_time - t) > 300){
                draw_faded_rectangle(renderer,x,y,largeur_note_blanche,255,0,0,0,0,0);
            }
            else{
                draw_rect(renderer,x,WINDOW_HEIGHT-hauteur_note_blanche/2,largeur_note_blanche,hauteur_note_blanche,255,200,200);
                for (int i=0; i<87; i++){
                    int x = x_pos_of_note(i, largeur_note_blanche, largeur_note_noire);
                    if(is_white_note(i)==0){
                        draw_rect(renderer,x,WINDOW_HEIGHT - hauteur_note_noire/2 - (hauteur_note_blanche-hauteur_note_noire),largeur_note_noire,hauteur_note_noire,20,20,20);
                    }
                }
            }
        }
        if(isWhiteNote==0){
            if((tab_temps[i] + animation_time - t) > 300){
                draw_faded_rectangle(renderer,x,y,largeur_note_noire,0,0,255,0,0,0);
            }
            else{
            draw_rect(renderer,x,WINDOW_HEIGHT - hauteur_note_noire/2 - (hauteur_note_blanche-hauteur_note_noire),largeur_note_noire,hauteur_note_noire,100,100,200);
            }
        }
        i++;
    }
}


void piano_notes_to_video(int* tab_temps,int* tab_notes,int nb_notes, char* audio_name)
{
    printf("Debut Video\n");
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Lancement SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        SDL_ExitWithError("Initialisation SDL echouee");
        SDL_Quit();
        return;
    }

    // Creation fenêtre + rendu
    if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0){
        SDL_ExitWithError("Impossible de creer la fenetre et le rendu");
        SDL_Quit();
        return;
    }

    // Chargement de l'audio .WAV           
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    if(!SDL_LoadWAV(audio_name, &wavSpec, &wavBuffer, &wavLength))
    {
        printf("le son .WAV: '%s' n'a pas pu être chargé!\n"
                "SDL_Error: %s\n", audio_name, SDL_GetError());
        return;
    }

    // Ouverture audio device
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if(!deviceId)
    {
        printf("L'audio device n'a pas pu être ouvert!\n"
                "SDL_Error: %s\n", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        return;
    }

    // Ajout à la file (playlist)
    if(SDL_QueueAudio(deviceId, wavBuffer, wavLength) < 0)
    {
        printf("L'audio n'a pas pu être ajouté à la playlist!\n"
                "SDL_Error: %s\n", SDL_GetError());
        SDL_CloseAudioDevice(deviceId);
        SDL_FreeWAV(wavBuffer);
        return;
    }
        

    /*-------------- Fonctions utiles en SDL:----------------------

    ####### Dessin d'un point coloré:

    if (SDL_SetRenderDrawColor(renderer,150,159,255, SDL_ALPHA_OPAQUE) !=0 )
        SDL_ExitWithError("Impossible de changer la couleur pour le rendu");
    
    if (SDL_RenderDrawPoint(renderer, 400, 300) != 0)
        SDL_ExitWithError("Impossible de dessiner un point");
    

    ####### Décompte du temps:

    SDL_GetTicks(); --> nb de milisecondes depuis SDL_init

    ----------------------------------------------------------------------------------*/


    /*------------------Gestion d'Evenements------------------*/



    int audio_has_started = 0;
    int nb_note_deja_jouee = 0;
    int indice_derniere_note_en_cours = 0;
    int animation_time = 2000; //temps d'animation d'une note en ms
    SDL_bool program_launched = SDL_TRUE;
    int t = SDL_GetTicks();
    int tmax = tab_temps[nb_notes-1] + animation_time;
    
    while((program_launched) & (t<tmax))
    {   
        t = SDL_GetTicks(); 
        //Temps écoulé depuis le début (Depuis SDL_init) en ms

        if (t > animation_time && (audio_has_started==0)){
            // Lance l'audio
             SDL_PauseAudioDevice(deviceId, 0);
             audio_has_started = 1;
        }

        if (tab_temps[indice_derniere_note_en_cours] < t - animation_time)
        { //Dans ce cas cette dernière note à finis son animation, la note suivante est la suivante à finir son animation
            indice_derniere_note_en_cours++;
        }

        update_renderer(renderer,t,tab_temps,tab_notes,indice_derniere_note_en_cours,animation_time,nb_notes);

        if ((nb_note_deja_jouee < nb_notes) & (tab_temps[nb_note_deja_jouee] < t))
        {
            printf("note %d au temps %d\n", tab_notes[nb_note_deja_jouee], t);
            nb_note_deja_jouee++;
        }


        SDL_Event event;
        while(SDL_PollEvent(&event)) 
        //capture tous les évènment exemple déplacement souris, appuie sur une touche ...
        {
            if (event.type == SDL_QUIT){program_launched=SDL_FALSE;}
        }

        SDL_RenderPresent(renderer);
    }
    
    /*--------------------------------------------------------*/

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


/*-------- Main  ----------*/


int main(int argc, char* argv[]){

    if (argc < 2){
        printf("Wrong number of argument\n");
        return 1;
    }
    char* audio_file_name = argv[1];
    printf("transcripting %s\n",audio_file_name);

    int* tab_temps_notes = malloc(sizeof(int)*10000);
    int* tab_notes = malloc(sizeof(int)*10000);
    int nb_notes_jouees;

    //Lecture et enregistrement des notes jouees
    piano_audio_to_piano_note(audio_file_name, &tab_temps_notes, &tab_notes, &nb_notes_jouees);

    //Affichage graphique
    piano_notes_to_video(tab_temps_notes, tab_notes, nb_notes_jouees, audio_file_name);

    free(tab_temps_notes);
    free(tab_notes);
    return 0;
}


