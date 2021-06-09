//
// Created by Rostislav Davydov on 09.06.2021.
//

#include "api.h"
#include <bson/bson.h>

#define BUFFER_SIZE 1024


void init_db() {

    bson_t parent;
    bson_t child;

    bson_init(&parent);
    bson_append_utf8(&parent, "pizda_key",-1,"123",-1);

    bson_append_document_begin(&parent,"root1", -1, &child);
    bson_append_utf8(&child, "pizda_key",-1,"12",-1);
    bson_append_document_end(&parent, &child);
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    fptr = fopen("mongo.db", "w");

    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    fwrite(&parent, sizeof(parent), 1, fptr);
    fclose(fptr);

}

bson_t read_bson() {
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    fptr = fopen("mongo.db", "r");

    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }
    bson_t b;

    fread(&b, sizeof(b), 1, fptr);
    fclose(fptr);
    return b;
}

void write_bson(bson_t b) {
    FILE *fptr;

    // use appropriate location if you are using MacOS or Linux
    fptr = fopen("mongo.db", "w");

    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }
    fread(&b, sizeof(b), 1, fptr);
    fclose(fptr);
}

char *api_create(char *path, char *param, char *value, char *cond) {
    bson_t b = read_bson();
    bson_iter_t iter;


}

const char *api_read(char *path, char *param, char *value, char *cond) {
    bson_t b = read_bson();

    printf("%s\n", bson_as_canonical_extended_json(&b,NULL));
    bson_iter_t iter;
    bson_iter_t baz;

    if (bson_iter_init(&iter, &b) &&
        bson_iter_find_descendant(&iter, ".root1", &baz) &&
        BSON_ITER_HOLDS_UTF8 (&baz)) {
        //bson_iter_value (&baz)

//        printf("baz = %s", bson_iter_utf8(&baz,NULL));
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        strcpy(buffer, (&baz, NULL));
        return buffer;
    } else if (bson_iter_init(&iter, &b) &&
               bson_iter_find_descendant(&iter, ".root1", &baz) &&
               BSON_ITER_HOLDS_DOCUMENT(&baz)) {
        //bson_iter_value (&baz)

        printf("result: ");

        bson_t b_res;
        //bzero(b_res, BUFFER_SIZE);



        bson_iter_document(&baz, &baz.len, &b_res);
        char* res = bson_as_relaxed_extended_json(&b_res, NULL);
        return res;
    }


}

