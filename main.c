#include <stdio.h>
#include <stdlib.h>
#include "image_utils.h"


int main() {

    char *filename_img = malloc(sizeof(char) * 40);
    if (filename_img == NULL) {
        printf("Failed to allocate memory");
        return 8;
    }

    printf("Enter destination image file name:");

    scanf("%39s", filename_img);

    int errcode1;
    struct image_t *m = load_image_t(filename_img, &errcode1);

    if (errcode1 == 2) {
        printf("Couldn't open file");
        free(filename_img);
        return 4;
    } else if (errcode1 == 3) {
        printf("File corrupted");
        free(filename_img);
        return 6;
    } else if (errcode1 == 4) {
        printf("Failed to allocate memory");
        free(filename_img);
        return 8;
    }


    printf("How many subimages you want to draw:");
    int img_count;
    if (scanf("%d", &img_count) != 1) {
        free(filename_img);
        printf("Incorrect input");
        destroy_image(&m);
        return 1;
    }
    if (img_count <= 0) {
        printf("Incorrect input data");
        free(filename_img);
        destroy_image(&m);
        return 2;
    }

    int i = 0;


    while (i < img_count) {

        char *filename_src = malloc(sizeof(char) * 40);
        if (filename_src == NULL) {
            printf("Failed to allocate memory");
            free(filename_img);
            destroy_image(&m);
            return 8;
        }

        printf("Enter a name of a subimage:");
        scanf("%39s", filename_src);

        int err_code2;


        struct image_t *src = load_image_t(filename_src, &err_code2);

        if (err_code2 == 2) {
            printf("Couldn't open file\n");
            destroy_image(&src);
            free(filename_src);
            i++;
            continue;
        } else if (err_code2 == 3) {
            printf("File corrupted\n");
            destroy_image(&src);
            free(filename_src);
            i++;
            continue;
        } else if (errcode1 == 4) {
            printf("Failed to allocate memory");
            free(filename_img);
            free(filename_src);
            destroy_image(&m);
            destroy_image(&src);
            return 8;
        }


        int x, y;
        printf("Enter coordinates (x, y):");
        if (scanf("%d %d", &x, &y) != 2) {
            printf("Incorrect input\n");
            free(filename_img);
            free(filename_src);
            destroy_image(&m);
            destroy_image(&src);
            return 1;
        }
        if (x < 0 || y < 0) {
            printf("Incorrect input data\n");
            destroy_image(&src);
            free(filename_src);
            i++;
            continue;
        }

        int err_code3 = draw_image(m, src, x, y);

        if (err_code3 == 1) {
            printf("Incorrect input data\n");
            destroy_image(&src);
            free(filename_src);
            i++;
            continue;
        }

        destroy_image(&src);
        free(filename_src);
        i++;
    }


    char *new_file_name = add_modified_suffix(filename_img);
    int err_code4 = save_image_t(new_file_name, m);
    if (err_code4 != 0) {
        printf("Couldn't create file");
        free(filename_img);
        destroy_image(&m);
        free(new_file_name);
        return 5;
    }

    printf("File saved");
    free(filename_img);
    destroy_image(&m);
    free(new_file_name);
    return 0;
}
