#include <stdio.h>
#include "functions.h"

#define json_output "output.json"
#define csv_output "output.csv"

int main() {
    char *filename = NULL;

    // Запрос имени файла от пользователя
    puts("Input the name of the file, which you want to parse.");
    filename = input_string(filename);

    // Открытие файла
    FILE *parse_file = NULL;
    parse_file = fopen(filename, "r");

    /* Если файл не удалось открыть */
    if (parse_file == NULL) {
        puts("Can't open file. Check it's name.");
        exit(-1);
    }

    /* Определяем тип файла */
    int type = validate_file(parse_file);

    // Выбираем формат, в который будет переведен файл
    if (type == 1) {
        puts("Parsing JSON to CSV ...");
        json_to_csv(parse_file, csv_output);
    } else if (type == 2) {
        puts("Parsing CSV to JSON ...");
        //csv_to_json(json_output);
    }

    // Process finished
    puts("\nProcess finished.\n"
         "Output file has been updated.");
    fclose(parse_file);

    return 0;
}