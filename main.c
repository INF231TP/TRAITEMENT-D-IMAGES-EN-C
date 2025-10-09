#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "image.h"
<<<<<<< HEAD

#define MAX_COMMAND_LENGTH 256

int main() {
    char commande[MAX_COMMAND_LENGTH];
    
    afficher_aide();
    
    while (1) {
        printf("📸 ppmviewer > ");
        
        if (fgets(commande, sizeof(commande), stdin) == NULL) {
            break;
        }
        
        commande[strcspn(commande, "\n")] = '\0';
        
        if (strlen(commande) == 0) continue;
        
        traiter_commande(commande);
    }
    
    return 0;
}
=======
int main(){



 

    return 0; 
>>>>>>> 47ef503713ee1cf4b90dbf8a13a14b3e6da02e87
