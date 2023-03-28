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


int nb_data(FILE *wav){
    //creation et initialisation du header
    struct wavfile header;
    

    //Gestion des erreurs de lecture
    if ( wav == NULL ){
        printf("\nne peut pas ouvrir le fichier demande, verifier le nom\n");
        printf("ne pas oublier l'extention .wav\n");
        return -1;
    }
    if ( fread(&header,sizeof(header),1,wav) < 1 ){
        printf("\nne peut pas lire le header\n");
        return -1;
    }
    if ((header.id[0] != 'R')||(header.id[1] != 'I')||(header.id[2] != 'F')||(header.id[3] != 'F')){
        printf("\nerreur le fichier n'est pas un format wave valide\n");
        return -1;
    }
    if (header.channels!=1){
        printf("\nerreur : le fichier n'est pas mono\n");
        return -1;
    }

    //Si tout se passe bien...
    return((header.bytes_in_data/header.bytes_by_capture));
}


 void rempli_tab(FILE *wav, float** tab_temps, float** tab_amplitudes){
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

    /*---------------------remplissage des tableaux----------*/
    int i=0;
    short value=0;
    while(fread(&value,(header.bits_per_sample)/8,1,wav)){
        //lecture des echantillons et enregistrement dans le tableau (en dB)
        //if (i%10 == 0){printf("value : %hd\n",value);}
        (*tab_amplitudes)[i] = 20*log10(value);
        (*tab_temps)[i] = 1.*i/header.frequency;
        i++;
    }
    /*-----------------fin de remplissage des tableaux-----------*/
 }



int main(int argc, char *argv[]){
    /* Prend un fichier .wav en argument */

    if (argc != 2)
        {printf("Il faut entrer un et un seul fichier !\n"); return 0;} //Erreur de lecture -> le programme s'arrête
    
    char* nom_fichier = argv[1];
    FILE *wav = fopen(nom_fichier,"rb");

    printf("\n nom_fichier : %s\n", nom_fichier);
    int len_tab = nb_data(wav);

    printf("\n nombre d'echantillons : %d\n\n",len_tab);

    if (len_tab == -1){return 0;} //Erreur de lecture -> le programme s'arrête

    float* tab_temps = malloc(sizeof(float) * len_tab);
    float* tab_amplitudes = malloc(sizeof(float) * len_tab);
    if (tab_temps == NULL || tab_amplitudes == NULL){printf("Données trop volumineuses\n"); return 0;}

    wav = fopen(nom_fichier,"rb");
    rempli_tab(wav, &tab_temps, &tab_amplitudes);

    //test
    for (int i = 0; i < 50; i++)
    {
        printf("i = %d, t = %fs, A = %fdB\n",i,tab_temps[i],tab_amplitudes[i]);
    }

    return 0;
}