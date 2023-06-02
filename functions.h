#ifndef PARSER_JSON_CSV_FUNCTIONS_H
#define PARSER_JSON_CSV_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "struct.h"

// Pre-parsing
char *input_string(char *string);
int validate_file(FILE *parse_file);

// Parsing
void json_to_csv(FILE *parse_file, char const *csv_output);

// HEADERS functions





#endif //PARSER_JSON_CSV_FUNCTIONS_H
