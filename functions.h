#ifndef PARSER_JSON_CSV_FUNCTIONS_H
#define PARSER_JSON_CSV_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct.h"

#define MAX_LENGTH_HEADER 30

// Pre-parsing
char *input_string(char *string);
int validate_file(FILE *parse_file);

// Parsing
void json_to_csv(FILE *parse_file, char const *csv_output);

void csv_to_json(FILE *parse_file, char const *json_output);

// HEADERS functions

#endif //PARSER_JSON_CSV_FUNCTIONS_H
