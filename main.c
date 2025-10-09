#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "image.h"

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
