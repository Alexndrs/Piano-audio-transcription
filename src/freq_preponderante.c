#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


float frequence_preponderante(double* tab_amplitude,int Fe,float seuil, int H, int len_tab)
{
    float fprep = -1;
    float amp_max = 0;
    float f;
    float prod_spec;
    for(int k = 40*(242765./44100); k < (len_tab/H) ;k++) //C'est la première fréquence audible tel que (k/len_tab)*Fe > 20Hz
    {
        f = k*Fe/len_tab;
        prod_spec = 1;
        for (int i = 1; i <= H; i++)        // On indice de 1 à H compris pour prendre en compte le fondamental
        {
            double r = REAL(tab_amplitude,i*k);
            double im = IMAG(tab_amplitude,i*k);
            prod_spec *=(r*r + im*im);
        }

        if(log10(prod_spec) > amp_max)
        {
            amp_max = log10(prod_spec);
            fprep = f;
            printf("fprep : %f, amp_max : %f\n",f,amp_max);
        }
    }
    if (fprep == -1){printf("Il ne s'est rien passé\n");}
    return (fprep); 
}
