#include "image.h"
int main(){
  
#include "IMAGE_H" // Assurez-vous que cela corresponde à votre nom de fichier d'en-tête (où se trouvent vos structures)
#include <stdio.h>

/**
 * @brief Modifie une image couleur pour la convertir en niveaux de gris.
 * * Parcourt tous les pixels de l'image et remplace les composantes R, V, B
 * par la moyenne de ces composantes (le niveau de gris).
 *
 * @param img Pointeur vers la structure image à modifier.
 */
void modifier_image_en_gris(image *img) {
    if (img == NULL || img->pixels == NULL) {
        fprintf(stderr, "Erreur : L'image est nulle ou non allouée.\n");
        return;
    }

    // Parcourir les lignes (hauteurs)
    for (unsigned int h = 0; h < img->hauteurs; h++) {
        // Parcourir les colonnes (largeurs)
        for (unsigned int l = 0; l < img->largeurs; l++) {
            // Pointeur vers le pixel courant
            pixel *p = &(img->pixels[h][l]);

            // Calcul du niveau de gris par moyenne arithmétique.
            // (Note : une moyenne pondérée est plus précise pour la perception
            // humaine, mais la moyenne simple est la plus facile pour un
            // calcul avec des entiers non signés.)
            uint16_t gris = (p->r + p->v + p->b) / 3;

            // Appliquer la nouvelle valeur de gris à toutes les composantes
            // du pixel (R, V, B) pour le rendre monochrome.
            p->r = gris;
            p->v = gris;
            p->b = gris;
            
            // La composante 'valmax' n'est pas modifiée car elle représente
            // la valeur maximale possible des composantes (e.g., 255).
        }
    }
}
 

    return 0; 
