#include "image.h"
#include <string.h>

int clamp(int val) {
    if (val < 0) return 0;
    if (val > 255) return 255; 
    return val;
}


image *allouer_image(unsigned int l, unsigned int h, unsigned int vm) {
    image *img = (image *)malloc(sizeof(image));
    if (!img) {
        perror("Erreur d'allocation de la structure image");
        return NULL;
    }

    img->largeurs = l;
    img->hauteurs = h;
    img->valmax = vm;

    // Allocation des pointeurs de lignes
    img->pixels = (pixel **)malloc(h * sizeof(pixel *));
    if (!img->pixels) {
        perror("Erreur d'allocation des lignes de pixels");
        free(img);
        return NULL;
    }

    
    for (unsigned int i = 0; i < h; i++) {
        img->pixels[i] = (pixel *)malloc(l * sizeof(pixel));
        if (!img->pixels[i]) {
            perror("Erreur d'allocation des colonnes de pixels");
            
            for (unsigned int j = 0; j < i; j++) free(img->pixels[j]);
            free(img->pixels);
            free(img);
            return NULL;
        }
    }
    return img;
} 

void liberer_img(image *img) {
    if (img) {
        if (img->pixels) {
            for (unsigned int i = 0; i < img->hauteurs; i++) {
                if (img->pixels[i]) {
                    free(img->pixels[i]); 
                }
            }
            free(img->pixels); 
        }
        free(img);
    }
}


image *lectureimg(const char *nomfichier){
     FILE *f = fopen(nomfichier, "r");
    if (!f) {
        fprintf(stderr, "Erreur: Fichier '%s' non trouvé ou inaccessible.\n", nomfichier);
        return NULL;
    }

    char version[3];
    unsigned int largeur, hauteur, valmax;
    if (fscanf(f, "%2s", version) != 1 || strcmp(version, "P3") != 0) {
        fprintf(stderr, "Erreur: Le format d'image doit être PPM P3.\n");
        fclose(f);
        return NULL;
     }
    if (fscanf(f,"%2s",&largeur)!=1){
        fprintf(stderr,"erreur de l'ecture de la largeur");
        }
    if (fscanf(f,"%2s",&hauteur)!=1) {
        fprintf(stderr,"erreur de l'ecture de la hauteur");
     }
    if (fscanf(f,"%2s",&valmax)!=1)
    {
        fprintf(stderr,"erreur de l'ecture de la valeur max");

    }
    if (valmax >255)
    {
        fprintf(stderr,"avertissement taille du fichier incorrecte");
    }
    
    image *img = allouer_image(largeur, hauteur, valmax);
    if (!img) { fclose(f); return NULL; }
    //lecture
    int r, v, b; 
    for (unsigned int i = 0; i < img->hauteurs; i++) {
        for (unsigned int j = 0; j < img->largeurs; j++) {
            if (fscanf(f, "%d %d %d", &r, &v, &b) != 3) {
                fprintf(stderr, "Erreur de lecture des données du pixel à (%u, %u).\n", i, j);
                liberer_img(img);
                fclose(f);
                return NULL;
            }
            
            img->pixels[i][j].r = (unsigned int)clamp(r); 
            img->pixels[i][j].v = (unsigned int)clamp(v);
            img->pixels[i][j].b = (unsigned int)clamp(b);
        }
    }

    fclose(f);
    return img;
};

void ecrire_img(const char *nomfichier,const image *img){
    FILE *f = fopen(nomfichier, "w");
    if (!f) {
        perror("Erreur lors de la création du fichier de résultat");
        return;
    }

   
    fprintf(f, "P3\n");
    fprintf(f, "# Image générée par ppmviewer\n");
    fprintf(f, "%u %u\n", img->largeurs, img->hauteurs);
    fprintf(f, "%u\n", img->valmax);

    
    for (unsigned int i = 0; i < img->hauteurs; i++) {
        for (unsigned int j = 0; j < img->largeurs; j++) {
            
            fprintf(f, "%u %u %u ", 
                    img->pixels[i][j].r, 
                    img->pixels[i][j].v, 
                    img->pixels[i][j].b);
        }
        fprintf(f, "\n");
    }

    fclose(f);


    
}
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
 




    

