# Rapport TP1 - Gestion du trafic et lumière de circulation

## Thomas Barkley, Austin Brodeur et Dany Gagnon

Ce projet simule une intersection a l'aide d'un ESP32 sur FreeRTOS.
La simulation utilise plusieur composant pour refleter la réalité.
Nous avons des lumiere de circulation pour une voie principale et secondaire, un detecteur de distance qui permet de limiter le temps d'attente d'une voiture.
Une lumière permet d'ouvrir et fermer une voie d'autobus selon une plage horaire.
Deux bouton permettent l'altération de la simulation: un bouton peut changer le mode de temps, le mode détermine l'équivalent d'une seconde dans la simulation:
- Mode 1: 1 seconde = 1 minute
- Mode 2: 1 seconde = 2 minute
- Mode 3: 1 seconde = 3 minute

Le mode, le temps et le temps d'attente d'un véhicule sont affiché sur un écran LED



