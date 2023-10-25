#ifndef CONSTANTS_H
#define CONSTANTS_H

// dans l'application de base, des valeurs en dur se trouvaient dans les temps d'attente (usleep).
// pour pouvoir contrôler le temps de la simulation (à des fins de déboggage), j'ai ajouté dans tous
// les calculs de temps d'attente l'utilisation de la constante suivante.
// Elle spécifie le nombre de microsecondes réelles dans 10000 microsecondes dans la simulation.
// Avec la valeur 10000, le temps s'écoulera normalement. Avec la valeur 1000, tout ira 10 fois plus vite.
static constexpr unsigned TIME_MULTIPLIER = 5000;
static constexpr int NO_TRADE = 0;

#endif // CONSTANTS_H
