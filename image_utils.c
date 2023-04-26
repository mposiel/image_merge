#include "image_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct image_t *load_image_t(const char *filename, int *err_code) {
    if (filename == NULL) {
        if (err_code != NULL) {
            *err_code = 1;
        }
        return NULL;
    }

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        if (err_code != NULL) {
            *err_code = 2;
        }
        return NULL;
    }

    struct image_t *tab = malloc(sizeof(struct image_t));
    if (tab == NULL) {
        if (err_code != NULL) {
            *err_code = 4;
        }
        fclose(f);
        return NULL;
    }

    int x = fscanf(f, "%2s", tab->type);
    if (x != 1 || (*tab->type != 'P' || *(tab->type + 1) != '2')) {
        if (err_code != NULL) {
            *err_code = 3;
        }
        fclose(f);
        free(tab);
        return NULL;
    }

    if (fscanf(f, "%d", &tab->width) != 1 || tab->width <= 0) {
        if (err_code != NULL) {
            *err_code = 3;
        }
        fclose(f);
        free(tab);
        return NULL;
    }


    if (fscanf(f, "%d", &tab->height) != 1 || tab->height <= 0) {
        if (err_code != NULL) {
            *err_code = 3;
        }
        fclose(f);
        free(tab);
        return NULL;
    }

    int max_val;
    if (fscanf(f, "%d", &max_val) != 1 || max_val <= 0) {
        if (err_code != NULL) {
            *err_code = 3;
        }
        fclose(f);
        free(tab);
        return NULL;
    }

    int er = matrix_create(tab, tab->width, tab->height);
    if (er == 2) {
        if (err_code != NULL) {
            *err_code = 4;
        }
        fclose(f);
        free(tab);
        return NULL;
    }

    for (int i = 0; i < tab->height; ++i) {
        for (int j = 0; j < tab->width; ++j) {
            if (fscanf(f, "%d", (*(tab->ptr + i) + j)) != 1) {
                if (err_code != NULL) {
                    *err_code = 3;
                }
                destroy_image(&tab);
                fclose(f);
                return NULL;
            }

            if (*(*(tab->ptr + i) + j) > max_val || *(*(tab->ptr + i) + j) < 0) {
                if (err_code != NULL) {
                    *err_code = 3;
                }
                destroy_image(&tab);
                fclose(f);
                return NULL;
            }
        }
    }

    if (err_code != NULL) {
        *err_code = 0;
    }
    fclose(f);
    return tab;
}


int matrix_create(struct image_t *m, int width, int height) {
    if (m == NULL || width <= 0 || height <= 0) {
        return 1;
    }

    m->ptr = malloc(height * sizeof(int *));
    if (m->ptr == NULL) {
        return 2;
    }
    for (int i = 0; i < height; ++i) {
        *((m->ptr) + i) = malloc(width * sizeof(int));
        if (*((m->ptr) + i) == NULL) {
            for (int j = 0; j < i; ++j) {
                free(*((m->ptr) + j));
            }
            free(m->ptr);
            return 2;
        }
    }
    return 0;
}

int save_image_t(const char *filename, const struct image_t *m1) {
    if (filename == NULL || m1 == NULL) {
        return 1;
    }
    if (m1->ptr == NULL || m1->width <= 0 || m1->height <= 0 || (*m1->type != 'p' && *(m1->type + 1) != '2')) {
        return 1;
    }


    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        return 2;
    }

    if (fprintf(f, "%s\n", m1->type) <= 0) {
        fclose(f);
        return 3;
    }
    if (fprintf(f, "%d %d\n", m1->width, m1->height) <= 0) {
        fclose(f);
        return 3;
    }

    if (fprintf(f, "255\n") <= 0) {
        fclose(f);
        return 3;
    }

    for (int i = 0; i < m1->height; ++i) {
        for (int j = 0; j < m1->width; ++j) {
            if (fprintf(f, "%d ", *(*(m1->ptr + i) + j)) < 0) {
                fclose(f);
                return 3;
            }
        }
        fprintf(f, "\n");
    }


    fclose(f);
    return 0;

}

void destroy_image(struct image_t **m) {
    if (m != NULL && *m != NULL) {
        if ((*m)->ptr != NULL) {
            for (int i = 0; i < (*m)->height; ++i) {
                free(*((*m)->ptr + i));
            }
            free((*m)->ptr);
        }
        free(*m);
        *m = NULL;
    }
}


const int *image_get_pixel(const struct image_t *img, int x, int y) {
    if (img == NULL) {
        return NULL;
    }
    if (x < 0 || y < 0 || img->ptr == NULL || img->height <= 0 || img->width <= 0 || x >= img->width ||
        y >= img->height) {
        return NULL;
    }

    const int *ptr = (*(img->ptr + y) + x);
    return ptr;
}

int *image_set_pixel(struct image_t *img, int x, int y) {
    if (img == NULL) {
        return NULL;
    }
    if (x < 0 || y < 0 || img->ptr == NULL || img->height <= 0 || img->width <= 0 || x >= img->width ||
        y >= img->height) {
        return NULL;
    }

    int *ptr = (*(img->ptr + y) + x);
    return ptr;
}


int draw_image(struct image_t *img, const struct image_t *src, int destx, int desty) {
    if (img == NULL || src == NULL || destx < 0 || desty < 0) {
        return 1;
    }
    if (img->ptr == NULL || img->height <= 0 || img->width <= 0 ||
        src->ptr == NULL || src->height <= 0 || src->width <= 0 || img->height < (desty + src->height) ||
        img->width < (destx + src->width)) {
        return 1;
    }
    for (int i = 0; i < src->height; ++i) {
        for (int j = 0; j < src->width; ++j) {
            *image_set_pixel(img, j + destx, i + desty) = *image_get_pixel(src, j, i);
        }
    }
    return 0;
}

char *add_modified_suffix(char *filename) {
    char *modified_filename = malloc(strlen(filename) + strlen("_modified") + 1);
    char *ext = strrchr(filename, '.');
    if (ext == NULL) {
        fprintf(stderr, "Error: file has no extension\n");
        exit(1);
    }
    int ext_index = ext - filename;
    strncpy(modified_filename, filename, ext_index);
    strcpy(modified_filename + ext_index, "_modified");
    strcpy(modified_filename + ext_index + strlen("_modified"), ext);
    return modified_filename;
}
