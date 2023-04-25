import numpy as np
import matplotlib.pyplot as plt

with open('amplitude_dB.txt', 'r') as fichier:
    amplitude = [int(ligne.strip()) for ligne in fichier.readlines()]

Te = 1/44100
n = len(amplitude)

tab_temps = Te * np.linspace(0,n-1)

plt.plot(tab_temps, amplitude)
plt.show()
