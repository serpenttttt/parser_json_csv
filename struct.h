#ifndef PARSER_JSON_CSV_STRUCT_H
#define PARSER_JSON_CSV_STRUCT_H

typedef struct HEADER {
    //int size;
    //struct HEADERS **child; // если в заголовке есть другие заголовки

    char *header_name; // сам заголовок
    char **values; // значения (для массива [])
} HEADER;

typedef struct HEADERS {
    struct HEADER *column;
} HEADERS;

#endif //PARSER_JSON_CSV_STRUCT_H


/*
 * header1 header 2
 *    ''     12345
 *    ''      3252
 *
 *
 * firstName0
 */