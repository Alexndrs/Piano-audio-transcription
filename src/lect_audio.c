#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


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



void rempli_tab(FILE *wav, float* tab_temps, float* tab_amplitudes){
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
    float value=0;
    while(fread(&value,(header.bits_per_sample)/8,1,wav)){
        //lecture des echantillons et enregistrement dans le tableau (en dB)
        tab_amplitudes[i] = 20*log10(value);
        tab_temps[i] = 1.*i/header.frequency;
        i++;
    }
}