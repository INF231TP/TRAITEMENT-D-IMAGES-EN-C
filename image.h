#ifndef IMAGE_H
#define IMAGE_H

   #include <stdio.h>
   #include <stdlib.h>
   #include <stdbool.h>
   #include <string.h>
   typedef struct 
   {
   __uint16_t r,v,b;
   }pixel;

   typedef struct {
      unsigned int largeurs;
      unsigned int hauteurs;
      unsigned int valmax;

      pixel **pixels;

   }image;
image *lectureimg(const char *nomfichier);
void ecrire_img(const char *nomfichier,const image *img);
image *allouer_image(unsigned int l, unsigned int h, unsigned int vm);
void liberer_img(image *img);
   
#endif
