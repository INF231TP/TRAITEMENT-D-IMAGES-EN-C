#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "image.h"

#define MAX_LINE_LENGTH 256

void afficher_banniere() {
    printf("=============================================\n");
    printf("✨ Application de Traitement d'Images PPM ✨\n");
    printf("=============================================\n");
    printf("Entrez 'help' pour la liste des commandes.\n");
}

char* obtenir_nom_sans_ext(const char* nom_fichier) {
    static char resultat[100];
    strcpy(resultat, nom_fichier);
    char* point = strrchr(resultat, '.');
    if (point) *point = '\0';
    return resultat;
}

int est_dans_limites(int x, int min, int max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

void trier_tableau(int* tab, int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (tab[j] > tab[j+1]) {
                int temp = tab[j];
                tab[j] = tab[j+1];
                tab[j+1] = temp;
            }
        }
    }
}

int mediane(int* tab, int n) {
    if (n == 0) return 0;
    trier_tableau(tab, n);
    return tab[n/2];
}

void supprimer_espaces(char* str) {
    char* fin;
    
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0) return;
    
    fin = str + strlen(str) - 1;
    while(fin > str && isspace((unsigned char)*fin)) fin--;
    
    *(fin + 1) = '\0';
}

int lire_image_ppm(const char* nom_fichier, ImagePPM* image) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) {
        printf("❌ ERREUR: Fichier non trouvé (%s)\n", nom_fichier);
        return 0;
    }
    
    char ligne[MAX_LINE_LENGTH];
    
    if (!fgets(ligne, sizeof(ligne), fichier)) {
        fclose(fichier);
        return 0;
    }
    sscanf(ligne, "%2s", image->version);
    
    if (strcmp(image->version, "P3") != 0) {
        printf("❌ ERREUR: Seul le format P3 est supporté\n");
        fclose(fichier);
        return 0;
    }
    
    while (fgets(ligne, sizeof(ligne), fichier)) {
        supprimer_espaces(ligne);
        
        if (strlen(ligne) == 0) continue;
        
        if (ligne[0] == '#') continue;
        
        if (sscanf(ligne, "%d %d", &image->largeur, &image->hauteur) == 2) break;
    }
    
    while (fgets(ligne, sizeof(ligne), fichier)) {
        supprimer_espaces(ligne);
        
        if (strlen(ligne) == 0) continue;
        
        if (ligne[0] == '#') continue;
        
        if (sscanf(ligne, "%d", &image->valeur_max) == 1) break;
    }
    
    image->pixels = (Pixel**)malloc(image->hauteur * sizeof(Pixel*));
    for (int i = 0; i < image->hauteur; i++) {
        image->pixels[i] = (Pixel*)malloc(image->largeur * sizeof(Pixel));
    }
    
    for (int i = 0; i < image->hauteur; i++) {
        for (int j = 0; j < image->largeur; j++) {
            int valeurs_lues;
            do {
                valeurs_lues = fscanf(fichier, "%d %d %d", &image->pixels[i][j].r, 
                          &image->pixels[i][j].g, &image->pixels[i][j].b);
                if (valeurs_lues != 3) {
                    if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
                        for (int k = 0; k <= i; k++) {
                            free(image->pixels[k]);
                        }
                        free(image->pixels);
                        fclose(fichier);
                        return 0;
                    }
                    supprimer_espaces(ligne);
                    if (strlen(ligne) == 0 || ligne[0] == '#') continue;
                    valeurs_lues = sscanf(ligne, "%d %d %d", &image->pixels[i][j].r, 
                              &image->pixels[i][j].g, &image->pixels[i][j].b);
                }
            } while (valeurs_lues != 3);
        }
    }
    
    fclose(fichier);
    return 1;
}

int ecrire_image_ppm(const char* nom_fichier, const ImagePPM* image) {
    FILE* fichier = fopen(nom_fichier, "w");
    if (!fichier) {
        printf("❌ ERREUR: Impossible de créer le fichier %s\n", nom_fichier);
        return 0;
    }
    
    fprintf(fichier, "%s\n", image->version);
    fprintf(fichier, "%d %d\n", image->largeur, image->hauteur);
    fprintf(fichier, "%d\n", image->valeur_max);
    
    for (int i = 0; i < image->hauteur; i++) {
        for (int j = 0; j < image->largeur; j++) {
            fprintf(fichier, "%d %d %d", image->pixels[i][j].r, 
                    image->pixels[i][j].g, image->pixels[i][j].b);
            if (j < image->largeur - 1) fprintf(fichier, " ");
        }
        fprintf(fichier, "\n");
    }
    
    fclose(fichier);
    return 1;
}

void liberer_image_ppm(ImagePPM* image) {
    if (image->pixels) {
        for (int i = 0; i < image->hauteur; i++) {
            free(image->pixels[i]);
        }
        free(image->pixels);
        image->pixels = NULL;
    }
}

int taille_image_ppm(const char* nom_fichier) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier, &image)) {
        return 0;
    }
    
    printf("📐 Dimensions: %d x %d (Largeur x Hauteur)\n", image.largeur, image.hauteur);
    liberer_image_ppm(&image);
    return 1;
}

int appliquer_dominante(const char* nom_fichier, char couleur_dom, int valeur) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier, &image)) {
        return 0;
    }
    
    char nom_fichier_sortie[150];
    snprintf(nom_fichier_sortie, sizeof(nom_fichier_sortie), "%s_dom.ppm", obtenir_nom_sans_ext(nom_fichier));
    
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = 0; j < image.largeur; j++) {
            Pixel* p = &image.pixels[i][j];
            int max_val = p->r;
            char dom = 'R';
            
            if (p->g > max_val) {
                max_val = p->g;
                dom = 'G';
            }
            if (p->b > max_val) {
                max_val = p->b;
                dom = 'B';
            }
            
            if (dom == couleur_dom) {
                p->r = est_dans_limites(p->r + valeur, 0, image.valeur_max);
                p->g = est_dans_limites(p->g + valeur, 0, image.valeur_max);
                p->b = est_dans_limites(p->b + valeur, 0, image.valeur_max);
            }
        }
    }
    
    int resultat = ecrire_image_ppm(nom_fichier_sortie, &image);
    liberer_image_ppm(&image);
    return resultat;
}

int convertir_gris(const char* nom_fichier) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier, &image)) {
        return 0;
    }
    
    char nom_fichier_sortie[150];
    snprintf(nom_fichier_sortie, sizeof(nom_fichier_sortie), "%s_gris.ppm", obtenir_nom_sans_ext(nom_fichier));
    
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = 0; j < image.largeur; j++) {
            Pixel* p = &image.pixels[i][j];
            int gris = (int)(0.299 * p->r + 0.587 * p->g + 0.114 * p->b);
            gris = est_dans_limites(gris, 0, image.valeur_max);
            p->r = p->g = p->b = gris;
        }
    }
    
    int resultat = ecrire_image_ppm(nom_fichier_sortie, &image);
    liberer_image_ppm(&image);
    return resultat;
}

int appliquer_negatif(const char* nom_fichier_entree, const char* nom_fichier_sortie) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier_entree, &image)) {
        return 0;
    }
    
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = 0; j < image.largeur; j++) {
            Pixel* p = &image.pixels[i][j];
            p->r = image.valeur_max - p->r;
            p->g = image.valeur_max - p->g;
            p->b = image.valeur_max - p->b;
        }
    }
    
    int resultat = ecrire_image_ppm(nom_fichier_sortie, &image);
    liberer_image_ppm(&image);
    return resultat;
}

int decouper_image(const char* nom_fichier, int l1, int l2, int c1, int c2, const char* nom_fichier_sortie) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier, &image)) {
        return 0;
    }
    
    if (l1 < 1 || l2 > image.hauteur || c1 < 1 || c2 > image.largeur || l1 >= l2 || c1 >= c2) {
        printf("❌ ERREUR: Coordonnées de découpage invalides. Doit respecter: 1 <= l1 < l2 <= %d et 1 <= c1 < c2 <= %d\n", 
               image.hauteur, image.largeur);
        liberer_image_ppm(&image);
        return 0;
    }
    
    l1--; l2--; c1--; c2--;
    
    ImagePPM resultat;
    strcpy(resultat.version, image.version);
    resultat.largeur = c2 - c1 + 1;
    resultat.hauteur = l2 - l1 + 1;
    resultat.valeur_max = image.valeur_max;
    
    resultat.pixels = (Pixel**)malloc(resultat.hauteur * sizeof(Pixel*));
    for (int i = 0; i < resultat.hauteur; i++) {
        resultat.pixels[i] = (Pixel*)malloc(resultat.largeur * sizeof(Pixel));
        for (int j = 0; j < resultat.largeur; j++) {
            resultat.pixels[i][j] = image.pixels[l1 + i][c1 + j];
        }
    }
    
    int res = ecrire_image_ppm(nom_fichier_sortie, &resultat);
    
    liberer_image_ppm(&resultat);
    liberer_image_ppm(&image);
    
    return res;
}

int appliquer_filtre_median(const char* nom_fichier_entree, const char* nom_fichier_sortie) {
    ImagePPM image;
    if (!lire_image_ppm(nom_fichier_entree, &image)) {
        return 0;
    }
    
    ImagePPM resultat;
    strcpy(resultat.version, image.version);
    resultat.largeur = image.largeur;
    resultat.hauteur = image.hauteur;
    resultat.valeur_max = image.valeur_max;
    
    resultat.pixels = (Pixel**)malloc(resultat.hauteur * sizeof(Pixel*));
    for (int i = 0; i < resultat.hauteur; i++) {
        resultat.pixels[i] = (Pixel*)malloc(resultat.largeur * sizeof(Pixel));
    }
    
    for (int i = 0; i < image.hauteur; i++) {
        for (int j = 0; j < image.largeur; j++) {
            int voisins_r[9], voisins_g[9], voisins_b[9];
            int count = 0;
            
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    
                    if (ni >= 0 && ni < image.hauteur && nj >= 0 && nj < image.largeur) {
                        voisins_r[count] = image.pixels[ni][nj].r;
                        voisins_g[count] = image.pixels[ni][nj].g;
                        voisins_b[count] = image.pixels[ni][nj].b;
                        count++;
                    }
                }
            }
            
            resultat.pixels[i][j].r = mediane(voisins_r, count);
            resultat.pixels[i][j].g = mediane(voisins_g, count);
            resultat.pixels[i][j].b = mediane(voisins_b, count);
        }
    }
    
    int res = ecrire_image_ppm(nom_fichier_sortie, &resultat);
    
    liberer_image_ppm(&resultat);
    liberer_image_ppm(&image);
    
    return res;
}

void afficher_aide() {
    printf("\nListe des commandes disponibles (format: commande arguments):\n");
    printf("------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("1️⃣  size <fichier.ppm>                                 : Affiche la largeur et la hauteur de l'image.\n");
    printf("2️⃣  dom <R|G|B> <valeur> <fichier.ppm>                 : Ajoute une dominante (R, G ou B) en augmentant les couleurs des pixels dominants.\n");
    printf("3️⃣  gris <fichier.ppm>                                 : Convertit l'image en niveaux de gris (sortie: <fichier>_gris.ppm).\n");
    printf("4️⃣  neg <fichier.ppm> <fichier_resultat.ppm>           : Crée le négatif de l'image.\n");
    printf("5️⃣  cut <fichier.ppm> <l1> <l2> <c1> <c2> <resultat> : Découpe l'image de la ligne l1 à l2 et de la colonne c1 à c2 (1-based).\n");
    printf("6️⃣  fil <fichier.ppm> <fichier_resultat.ppm>           : Applique un filtre médian 3x3 pour réduire le bruit.\n");
    printf("7️⃣  quit                                             : Quitte l'application.\n");
    printf("8️⃣  help                                             : Affiche cette aide.\n");
    printf("------------------------------------------------------------------------------------------------------------------------------------\n");
}


void traiter_commande(const char* commande) {
    char cmd[20], arg1[100], arg2[100], arg3[100], arg4[100], arg5[100], arg6[100];
    int l1, l2, c1, c2, val;
    char couleur;
    
    memset(arg1, 0, sizeof(arg1));
    memset(arg2, 0, sizeof(arg2));
    memset(arg3, 0, sizeof(arg3));
    memset(arg4, 0, sizeof(arg4));
    memset(arg5, 0, sizeof(arg5));
    memset(arg6, 0, sizeof(arg6));
    
    int args = sscanf(commande, "%19s %99s %99s %99s %99s %99s %99s", 
                     cmd, arg1, arg2, arg3, arg4, arg5, arg6);
    
    if (args < 1) return;
    
    supprimer_espaces(cmd);
    supprimer_espaces(arg1);
    supprimer_espaces(arg2);
    supprimer_espaces(arg3);
    supprimer_espaces(arg4);
    supprimer_espaces(arg5);
    supprimer_espaces(arg6);
    
    for(int i = 0; cmd[i]; i++){
      cmd[i] = tolower(cmd[i]);
    }

    if (strcmp(cmd, "size") == 0) {
        if (args >= 2) {
            if (taille_image_ppm(arg1)) {
                printf("✅ Opération 'size' réussie.\n");
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'size'. Utilisation: size fichier.ppm\n");
        }
    }
    else if (strcmp(cmd, "dom") == 0) {
        if (args >= 4) {
            if (strlen(arg1) != 1 || (arg1[0] != 'R' && arg1[0] != 'G' && arg1[0] != 'B')) {
                printf("❌ ERREUR: La couleur doit être R, G ou B\n");
            } else {
                couleur = arg1[0];
                val = atoi(arg2);
                if (appliquer_dominante(arg3, couleur, val)) {
                    printf("✅ Opération 'dom' réussie. Fichier créé: %s_dom.ppm\n", obtenir_nom_sans_ext(arg3));
                } else {
                    printf("❌ ERREUR lors du traitement de la dominante.\n");
                }
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'dom'. Utilisation: dom c val fichier.ppm\n");
        }
    }
    else if (strcmp(cmd, "gris") == 0) {
        if (args >= 2) {
            if (convertir_gris(arg1)) {
                printf("✅ Opération 'gris' réussie. Fichier créé: %s_gris.ppm\n", obtenir_nom_sans_ext(arg1));
            } else {
                printf("❌ ERREUR lors de la conversion en gris.\n");
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'gris'. Utilisation: gris fichier.ppm\n");
        }
    }
    else if (strcmp(cmd, "neg") == 0) {
        if (args >= 3) {
            if (appliquer_negatif(arg1, arg2)) {
                printf("✅ Opération 'neg' réussie. Fichier créé: %s\n", arg2);
            } else {
                printf("❌ ERREUR lors de la création du négatif.\n");
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'neg'. Utilisation: neg fichier.ppm fichier_resultat.ppm\n");
        }
    }
    else if (strcmp(cmd, "cut") == 0) {
        if (args >= 7) {
            l1 = atoi(arg2); l2 = atoi(arg3); c1 = atoi(arg4); c2 = atoi(arg5);
            if (l1 <= 0 || l2 <= 0 || c1 <= 0 || c2 <= 0) {
                printf("❌ ERREUR: Les coordonnées doivent être des nombres positifs (> 0).\n");
            } else if (decouper_image(arg1, l1, l2, c1, c2, arg6)) {
                printf("✅ Opération 'cut' réussie. Fichier créé: %s\n", arg6);
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'cut'. Utilisation: cut fichier.ppm l1 l2 c1 c2 fichier_resultat.ppm\n");
        }
    }
    else if (strcmp(cmd, "fil") == 0) {
        if (args >= 3) {
            if (appliquer_filtre_median(arg1, arg2)) {
                printf("✅ Opération 'fil' réussie. Fichier créé: %s\n", arg2);
            } else {
                printf("❌ ERREUR lors de l'application du filtre médian.\n");
            }
        } else {
            printf("❌ ERREUR: Arguments manquants pour 'fil'. Utilisation: fil fichier.ppm fichier_resultat.ppm\n");
        }
    }
    else if (strcmp(cmd, "help") == 0) {
        afficher_aide();
    }
    else if (strcmp(cmd, "quit") == 0) {
        printf("👋 Au revoir! Merci d'avoir utilisé notre application.\n");
        exit(0);
    }
    else {
        printf("⚠️ Commande '%s' non reconnue. Tapez 'help' pour la liste des commandes.\n", cmd);
    }
}
