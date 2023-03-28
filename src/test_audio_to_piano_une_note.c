#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

void audio_to_piano_une_note( ??? )
{
    int i=0;
    char fichieraudio[100];
    char fichierdat[100];
    //float filtre_freq, filtre_large;


    /*---------------------selection du fichier audio-------------------------------*/
    printf ("entrer le nom du fichier audio a extraire en data :\n");
    scanf("%s", fichieraudio);
    printf ("nom du fichier : %s\n", fichieraudio);
    sprintf (fichierdat,"%s.dat", fichieraudio);
    /*--------------fin de selection du fichier audio-------------------------------*/

    int nbech= nb_data( ??? ); //nombre d echantillons extraits du fichier audio

}