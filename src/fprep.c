#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

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