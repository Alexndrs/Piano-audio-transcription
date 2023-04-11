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


void nb_data_Fe(FILE *wav, int* nb_data, float* Fe){
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
    nb_data = (header.bytes_in_data/header.bytes_by_capture);
    Fe = header.frequency;
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

void fenetrage_hamming(float* tab_amplitude, float** amplitude_fenetree, int nb_data, float Fe, float t1, float t2)
{
    float Te = 1/Fe;
    for(int k=0;k<nb_data;k++)
    {
        float t = k*Te;
        if(t>t1 && t<t2)
        {
            (*amplitude_fenetree)[k] = tab_amplitude[k]*(0.54 - 0.46 * cos(2*3.141592653589793*(t-t1)/(t2 - t1)));
        }
        else
        { 
            (*amplitude_fenetree)[k] = 0;
        }
    }
}

float frequence_preponderante(float* tab_amplitude,float* tab_frequence,float seuil, int H, int len_tab)
{
    float fprep;
    float amp_max = seuil;
    float f;
    float prod_spec;
    for(int k = 0;k<(len_tab/H);k++)
    {
        f = tab_frequence[k];
        prod_spec = 1;
        for (int i = 1; i <= H; i++)
        {
            prod_spec = prod_spec*(REAL(tab_frequence,i*k)^2+IMAG(tab_frequence,i*k)^2);
        }
        if(log10(prod_spec) > amp_max)
        {
            amp_max = log10(prod_spec);
            fprep = f;
        }
    }
    return (fprep); 
}


int main(int argc, char *argv[]){
    /* Prend un fichier .wav en argument */

    if (argc != 2)
        {printf("Il faut entrer un et un seul fichier !\n"); return 0;} //Erreur de lecture -> le programme s'arrête
    
    char* nom_fichier = argv[1];
    FILE *wav = fopen(nom_fichier,"rb"); //ouverture du fichier wave


    printf("\n nom_fichier : %s\n", nom_fichier);
    int len_tab; // taille des tableaux
    float Fe; // fréquence d'échantillonage
    nb_data(wav,len_tab,Fe);

    printf("\n nombre d'echantillons : %d\n\n",len_tab);

    if (len_tab == -1){return 0;} //Erreur de lecture -> le programme s'arrête

    float* tab_temps = malloc(sizeof(float) * len_tab);
    float* tab_amplitude = malloc(sizeof(float) * len_tab);
    if (tab_temps == NULL || tab_amplitude == NULL){printf("Données trop volumineuses\n"); return 0;}

    wav = fopen(nom_fichier,"rb");
    rempli_tab(wav, &tab_temps, &tab_amplitude);

    // Boucle de traitement : fenetrage transformée de fourier, note...
        //
    float T_total = len_tab/Fe; // durée de l'enregistrement
    float tau = 0.02;       // pas de décalage temporel entre 2 fenêtres (en secondes)
    float t1 = 0;
    float t2 = 0.2;
    float** amplitude_fenetree = malloc(sizeof(float)*len_tab);
    float* tab_frequence = malloc(sizeof(float)*len_tab);
    for(int k = 0,k<len_tab,k++){tab_frequence[k]=k;}
    float* amplitude_fourier = malloc(sizeof(float)*len_tab);
    while (t2<T_total)
    {
        // Fenetrage
        void fenetrage_hamming(tab_amplitude, amplitude_fenetree,len_tab,Fe,t1,t2);
        t1 = t1 + tau;
        t2 = t2 + tau;
        // Transformée de fourier (alexandre c'est pour toi)
        //utilise le tableau amplitude fourier stp, c'est lui que j'ai utilisé pour déterminer la fonction

        // Détection de la fréquence
        int H = 5; // nombre d'harmoniques qu'on étudie
        float seuil = 50; //seul en dB pour l'étude des fréquences
        float fprep = frequence_preponderante(amplitude_fourier,tab_frequence,seuil,H,len_tab);

    }    
{
    
    //test
    for (int i = 0; i < 50; i++)
    {
        printf("i = %d, t = %fs, A = %fdB\n",i,tab_temps[i],tab_amplitudes[i]);
    }

    return 0;
}
}