
void free_image(Image *img) {
    if (!img) return;
    if (img->data) free(img->data);
    img->data = NULL;
    img->width = img->height = img->maxval = 0;
}



/* dominante : renvoie 'R','G' ou 'B' selon la composante la plus grande.
   En cas d'égalité, on applique la priorité R > G > B (choix arbitraire mais déterministe). */
char dominante_of_pixel(int R, int G, int B) {
    if (R >= G && R >= B) return 'R';
    if (G >= R && G >= B) return 'G';
    return 'B';
}

/* op_dom : "dom C val image" -> pour tous les pixels dont la dominante == C,
   ajoute val à chacune des 3 composantes (selon l'énoncé). Clamp entre 0 et maxval. */
void op_dom(Image *img, char C, int val) {
    int npix = img->width * img->height;
    for (int p = 0; p < npix; ++p) {
        int idx = 3*p;
        int R = img->data[idx];
        int G = img->data[idx+1];
        int B = img->data[idx+2];

        char dom = dominante_of_pixel(R,G,B);
        if (dom == C) { 
            img->data[idx]   = clamp(R + val, 0, img->maxval);
            img->data[idx+1] = clamp(G + val, 0, img->maxval);
            img->data[idx+2] = clamp(B + val, 0, img->maxval);
        }
    }
}


void op_gris(Image *img) {
    int npix = img->width * img->height;
    for (int p = 0; p < npix; ++p) {
        int idx = 3*p;
        int R = img->data[idx];
        int G = img->data[idx+1];
        int B = img->data[idx+2];
        int moy = (R + G + B) / 3; 
        img->data[idx] = img->data[idx+1] = img->data[idx+2] = moy;
    }
}


void op_size(const Image *img) {
    int npix = img->width * img->height;
    printf("%d x %d\n", img->width, img->height);
    printf("Nombre de pixels : %d\n", npix);
}


int op_cut(const Image *img, int l1, int l2, int c1, int c2, const char *outfile) {
    
    if (!(1 <= l1 && l1 <= l2 && l2 <= img->height && 1 <= c1 && c1 <= c2 && c2 <= img->width)) {
        fprintf(stderr, "Indices invalides pour cut : l1=%d l2=%d c1=%d c2=%d\n", l1, l2, c1, c2);
        return 0;
    }
    int new_h = l2 - l1 + 1;
    int new_w = c2 - c1 + 1;

    Image out;
    out.width = new_w;
    out.height = new_h;
    out.maxval = img->maxval;
    out.data = (int*)malloc(new_w * new_h * 3 * sizeof(int));
    if (!out.data) { perror("malloc"); return 0; }

    for (int r = 0; r < new_h; ++r) {
        for (int c = 0; c < new_w; ++c) {
            int src_r = (l1 - 1) + r; /* conversion 1-based -> 0-based */
            int src_c = (c1 - 1) + c;
            int src_idx = get_index(img, src_r, src_c, 0);
            int dst_idx = get_index(&out, r, c, 0);
            out.data[dst_idx]   = img->data[src_idx];
            out.data[dst_idx+1] = img->data[src_idx+1];
            out.data[dst_idx+2] = img->data[src_idx+2];
        }
    }

    int ok = write_ppm_p3(outfile, &out);
    free_image(&out);
    return ok;
}


int op_neg(const Image *img, const char *outfile) {
    Image out;
    out.width = img->width; out.height = img->height; out.maxval = img->maxval;
    int npix = out.width * out.height;
    out.data = (int*)malloc(npix * 3 * sizeof(int));
    if (!out.data) { perror("malloc"); return 0; }

    for (int i = 0; i < npix * 3; ++i) {
        out.data[i] = img->maxval - img->data[i];
    }

    int ok = write_ppm_p3(outfile, &out);
    free_image(&out);
    return ok;
}

