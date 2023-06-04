#ifndef PARSER_JSON_CSV_STRUCT_H
#define PARSER_JSON_CSV_STRUCT_H

typedef struct HEADER {
    char *header_name; // Сам заголовок
    int number_of_values; // Число значений в массиве
    char **values; // Значения (для массива [])
} HEADER;

typedef struct HEADERS {
    int number_headers;
    struct HEADER *column;
} HEADERS;

#endif //PARSER_JSON_CSV_STRUCT_H