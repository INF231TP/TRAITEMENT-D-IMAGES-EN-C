#include "image.h"
int main(){
   

    return 0;
}

/* ---------- Programme principal de test de la fonction en gris ---------- */
int main() {
    char entree[100], sortie[100];
    printf("Nom du fichier PPM à convertir : ");
    scanf("%s", entree);
    printf("Nom du fichier de sortie : ");
    scanf("%s", sortie);

    Image *img = read_p3(entree);
    if (!img) {
        printf("Erreur de lecture de l'image.\n");
        return 1;
    }

    image_en_gris(img);
    write_p3(sortie, img);

    printf("Image convertie en gris avec succès : %s\n", sortie);

    free(img->data);
    free(img);
    return 0;
}
