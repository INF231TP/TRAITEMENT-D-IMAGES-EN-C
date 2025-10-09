#ifndef _IMAGE_
#define _IMAGE_

typedef struct {
    int r, g, b;
} Pixel;

typedef struct {
    char version[3];
    int largeur;
    int hauteur;
    int valeur_max;
    Pixel** pixels;
} ImagePPM;

int lire_image_ppm(const char* nom_fichier, ImagePPM* image);
int ecrire_image_ppm(const char* nom_fichier, const ImagePPM* image);
void liberer_image_ppm(ImagePPM* image);

int taille_image_ppm(const char* nom_fichier);
int appliquer_dominante(const char* nom_fichier, char couleur, int valeur);
int convertir_gris(const char* nom_fichier);
int appliquer_negatif(const char* nom_fichier_entree, const char* nom_fichier_sortie);
int decouper_image(const char* nom_fichier, int l1, int l2, int c1, int c2, const char* nom_fichier_sortie);
int appliquer_filtre_median(const char* nom_fichier_entree, const char* nom_fichier_sortie);

void supprimer_espaces(char* str);
char* obtenir_nom_sans_ext(const char* nom_fichier);
int est_dans_limites(int x, int min, int max);
void trier_tableau(int* tab, int n);
int mediane(int* tab, int n);
void afficher_banniere();
void traiter_commande(const char* commande);
void afficher_aide();

#endif
