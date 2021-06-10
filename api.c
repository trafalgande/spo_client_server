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
    bson_init(&child);
    bson_append_utf8(&child, "pizda_key",-1,"123",-1);
    bson_append_utf8(&child, "pizda_key23",-1,"12323",-1);
    bson_append_document(&parent, "root",-1,&child);
    //bson_append_document_begin(&parent,"root1", -1, &child);
    //bson_append_utf8(&child, "pizda_key",-1,"12",-1);
    //bson_append_document_end(&parent, &child);
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
    bson_iter_t baz;
    int path_len= strlen(param);
    char path_t[path_len];
    strcpy(path_t, path);
    //printf("%s\n",path_t);
    char * new_path = strtok(path_t, ":");
    printf("%s\n",new_path);
    char * key = strtok(NULL, ":");
    printf("%s\n",key);
    int full_path_len= strlen(new_path)+1+strlen(key);

    char full_path[full_path_len];
    strcpy(full_path, new_path);
    //full_path1 = strcat(new_path,".");
    strcat(full_path,".");
    strcat(full_path,key);

    printf("%s\n",full_path);



    if (!(bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, new_path, &baz))) return "{\"error\": \"path_is_invalid\"}";
    if (bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, full_path, &baz)) return "{\"error\": \"already_exists\"}";

    bson_t* b_res;

    uint32_t object_len;
    const uint8_t *object_buf;

    bson_iter_document(&baz, &object_len, &object_buf);
    //bson_init_static(b_res,baz,object_len);
    b_res = bson_new_from_data(object_buf, object_len);
    if (strcmp(param, "dir") == 0){
        bson_t node;
        bson_init(&node);
        bson_append_document(b_res, key,-1,&node);
    } else if (strcmp(param, "node") == 0){
        bson_t node;
        bson_init(&node);
        bson_append_utf8(b_res, key,-1,value,-1);
    }

    //baz=b_res;
//    printf("%s\n\n\n", bson_as_canonical_extended_json(b_res,NULL));
//
//
//    bson_t a;
//
//    bson_init(&a);
//    bson_append_utf8(&a, "pizda_key",-1,"123",-1);

    write_bson(b);


    return "{\"status\": \"ok\"}";
    //else

}

const char *api_read(char *path, char *param, char *value, char *cond) {
    bson_t b = read_bson();

    //printf("%s\n", bson_as_canonical_extended_json(&b,NULL));
    bson_iter_t iter;
    bson_iter_t baz;
    if ((strcmp(path, "") == 0)||(strcmp(path, ".") == 0)) return bson_as_canonical_extended_json(&b,NULL);

    if (bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, path, &baz)){
        if (BSON_ITER_HOLDS_UTF8 (&baz)) {
            //bson_iter_value (&baz)

            //printf("baz = %s", bson_iter_utf8(&baz,NULL));
            char buffer[BUFFER_SIZE];
            bzero(buffer, BUFFER_SIZE);
            strcpy(buffer, bson_iter_utf8(&baz, NULL));
            return buffer;
        } else if (BSON_ITER_HOLDS_DOCUMENT(&baz)) {
            //bson_iter_value (&baz)

            bson_t* b_res;

            uint32_t object_len;
            const uint8_t *object_buf;

            bson_iter_document(&baz, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            char *res = bson_as_canonical_extended_json(b_res, NULL);
            return res;
            //return res;
        }
    }
    else return "{\"error\": \"not_found\"}";



}


char * api_update(char *path, char *param, char *value, char *cond){
    bson_t b = read_bson();

    bson_iter_t iter;
    bson_iter_t baz;
    int path_len= strlen(param);
    char path_t[path_len];
    strcpy(path_t, path);
    //printf("%s\n",path_t);
    char * new_path = strtok(path_t, ":");
    printf("%s\n",new_path);
    char * key = strtok(NULL, ":");
    printf("%s\n",key);
    int full_path_len= strlen(new_path)+1+strlen(key);

    char full_path[full_path_len];
    strcpy(full_path, new_path);
    //full_path1 = strcat(new_path,".");
    strcat(full_path,".");
    strcat(full_path,key);

    printf("%s\n",full_path);



    if (!(bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, new_path, &baz))) return "{\"error\": \"path_is_invalid\"}";
    if (!(bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, full_path, &baz))) return "{\"error\": \"don't_exists\"}";

    bson_t* b_res;

    uint32_t object_len;
    const uint8_t *object_buf;

    bson_iter_document(&baz, &object_len, &object_buf);
    //bson_init_static(b_res,baz,object_len);
    b_res = bson_new_from_data(object_buf, object_len);
    if (strcmp(param, "dir") == 0){
        //delete
        bson_t node;
        bson_init(&node);
        bson_append_document(b_res, key,-1,&node);
    } else if (strcmp(param, "node") == 0){
        //delete
        bson_t node;
        bson_init(&node);
        bson_append_utf8(b_res, key,-1,value,-1);
    }

    write_bson(b);


    return "{\"status\": \"ok\"}";
}


char * api_delete(char *path, char *param, char *value, char *cond){
    bson_t b = read_bson();

    bson_iter_t iter;
    bson_iter_t baz;
    int path_len= strlen(param);
    char path_t[path_len];
    strcpy(path_t, path);
    //printf("%s\n",path_t);
    char * new_path = strtok(path_t, ":");
    printf("%s\n",new_path);
    char * key = strtok(NULL, ":");
    printf("%s\n",key);
    int full_path_len= strlen(new_path)+1+strlen(key);

    char full_path[full_path_len];
    strcpy(full_path, new_path);
    //full_path1 = strcat(new_path,".");
    strcat(full_path,".");
    strcat(full_path,key);

    printf("%s\n",full_path);



    if (!(bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, new_path, &baz))) return "{\"error\": \"path_is_invalid\"}";
    if (bson_iter_init(&iter, &b) && bson_iter_find_descendant(&iter, full_path, &baz)) return "{\"error\": \"already_exists\"}";

    bson_t* b_res;

    uint32_t object_len;
    const uint8_t *object_buf;

    bson_iter_document(&baz, &object_len, &object_buf);
    //bson_init_static(b_res,baz,object_len);
    b_res = bson_new_from_data(object_buf, object_len);
    if (strcmp(param, "dir") == 0){
        //хуй знает че тут бля писать

    } else if (strcmp(param, "node") == 0){
        //хуй знает че тут бля писать

    }

    //baz=b_res;
//    printf("%s\n\n\n", bson_as_canonical_extended_json(b_res,NULL));
//
//
//    bson_t a;
//
//    bson_init(&a);
//    bson_append_utf8(&a, "pizda_key",-1,"123",-1);

    write_bson(b);


    return "{\"status\": \"ok\"}";
}


