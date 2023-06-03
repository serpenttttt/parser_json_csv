#include "functions.h"

// Функция для записи строки, полученной от пользователя
char *input_string(char *string) {
    char input;
    for (int i = 0; (input = getchar()) != '\n'; ++i) {
        string = realloc(string, sizeof(char) * (i + 2));
        string[i] = input;
        string[i + 1] = '\0';
    }
    return string;
}

// Функция валидации файла определяет тип файла
int validate_file(FILE *parse_file) {
    char c;
    while ((c = getc(parse_file)) != EOF) {
        if (c == '"') {
            while ((c = getc(parse_file)) != '"');
            c = getc(parse_file);
            if (c == ':') {
                return 1; //json
            }
        }
    }
    return 2; //csv
}

// HEADERS functions
HEADERS* init_HEADERS(HEADERS *headers_to_init) {

    headers_to_init = (HEADERS *) malloc(sizeof (HEADERS));
    headers_to_init->column = NULL;

    return headers_to_init;
}

HEADER* init_column(HEADER *column_to_init) {

    column_to_init = (HEADER *) malloc(sizeof (HEADER));
    //column_to_init->child = NULL;
    column_to_init->header_name = (char *) malloc(sizeof (char));
    column_to_init->values = (char **) malloc(sizeof (char *));

    return column_to_init;
}

void write_headers_and_data_in_struct(FILE *parse_file, HEADERS *json_columns) {
    char c; // Переменная для чтения
    int i;
    int k = 0; // Позиция символа
    int spaces = 0; // Число пробелов

    int array_size = 0; // Размер массива
    int str_size = 0; // Размер строки

    char *string = (char *) malloc(sizeof (char ));

    int number_of_headers = 0;
    int number_of_quotes = 0;
    int number_of_commas = 0;

    rewind(parse_file);

    while ((c = fgetc(parse_file)) != EOF) {
        if (c == '"') {

            json_columns = realloc(json_columns, sizeof (HEADERS) * (number_of_headers + 1));
            json_columns[number_of_headers].column = init_column(json_columns[number_of_headers].column);

            for (i = 0; (c = fgetc(parse_file)) != ':'; i++) {
                json_columns[number_of_headers].column->header_name = realloc(json_columns[number_of_headers].column->header_name, sizeof (char) * (i + 1));
                json_columns[number_of_headers].column->header_name[i] = c;
            }

            json_columns[number_of_headers].column->header_name[i - 1] = '\0';

            /*
             * Проверяем, есть ли в заголовке другие заголовки
             * 1. Если есть, то вместо только что записанного пишем новый
             * 2. Если нет, то увеличиваем число заголовков на 1
             */

            for (str_size = 0; (c = fgetc(parse_file)) != '\n'; ++str_size) {
                string = realloc(string, sizeof (char ) * (str_size + 2));
                string[str_size] = c;

                if (c == '{' || c == '[') {
                    break;
                }
            }
            string[str_size] = '\0';

            //printf("%s", string);

            if (c != '{') {

                // Считаем число кавычек и запятых
                for (int j = 0; string[j] != '\0'; ++j) {
                    if (string[j] == '"')
                        number_of_quotes++;
                    if (string[j] == ',' && j != strlen(string) - 1)
                        number_of_commas++;
                }
                // Если число кавычек 0 или 2, а число запятых 0, то нам нужно записать значение в структуру без кавычек
                if ((number_of_quotes == 0 || number_of_quotes == 2) && number_of_commas == 0) {

                    json_columns[number_of_headers].column->values[0] = (char *) malloc(sizeof (char));

                    // Пропускаем пробелы
                    for (spaces = 0; string[spaces] == ' '; ++spaces);

                    // Читаем строку string
                    for (int j = spaces; string[j] != '\0'; ++j) {
                        if (string[j] == ',')
                            break;
                        if (string[j] != '"') {
                            json_columns[number_of_headers].column->values[0] = realloc(json_columns[number_of_headers].column->values[0], sizeof (char) * (k + 2));
                            json_columns[number_of_headers].column->values[0][k] = string[j];
                            ++k;
                        }
                    }
                    json_columns[number_of_headers].column->values[0][k] = '\0';
                    //printf("%s\n", json_columns[number_of_headers].column->values[0]);
                }
                // Если число кавычек четное или количество запятых != 0, то записываем в структуру в кавычках
                else if ((number_of_quotes % 2 == 0) || (number_of_commas > 0 && (number_of_quotes % 2 == 0))) {

                    json_columns[number_of_headers].column->values[0] = (char *) malloc(sizeof (char));

                    // Пропускаем пробелы
                    for (spaces = 0; string[spaces] == ' '; ++spaces);

                    // Читаем строку string
                    for (int j = spaces; string[j] != '\0'; ++j) {
                        json_columns[number_of_headers].column->values[0] = realloc(json_columns[number_of_headers].column->values[0], sizeof (char) * (k + 2));
                        json_columns[number_of_headers].column->values[0][k] = string[j];
                        ++k;
                    }
                    json_columns[number_of_headers].column->values[0][k - 1] = '\0';
                    //printf("%s\n", json_columns[number_of_headers].column->values[0]);
                }
                // Если число кавычек нечетное, то выводим пользователю сообщение о вероятной ошибке в данных
                else {
                    puts("Something wrong with data in the json file.");
                    // Записать ошибку
                }
                number_of_headers = number_of_headers + 1;
            }
            if (c == '[') {

                // Ищем ']'
                while ((c = fgetc(parse_file)) != ']') {

                    k = 0;

                    // Ищем символ, отличный от ' ' и '\n'
                    while (c == ' ' || c == '\n')
                        c = fgetc(parse_file);

                    // Проверяем c на ']'
                    if (c == ']')
                        break;

                    // Записываем строку
                    for (str_size = 0; c != '\n'; ++str_size) {
                        string = realloc(string, sizeof (char ) * (str_size + 2));
                        string[str_size] = c;

                        if (c == '{' || c == '[') {
                            puts("Can't parse the data.");
                            break;
                        }

                        c = fgetc(parse_file);
                    }
                    if (string[str_size - 1] == ',') {
                        string[str_size - 1] = '\0';
                    }
                    else {
                        string[str_size] = '\0';
                    }
                    //printf("%s", string);

                    // Считаем число кавычек и запятых
                    for (int j = 0; string[j] != '\0'; ++j) {
                        if (string[j] == '"')
                            number_of_quotes++;
                        if (string[j] == ',' && j != strlen(string) - 1)
                            number_of_commas++;
                    }

                    // Если число кавычек 0 или 2, а число запятых 0, то нам нужно записать значение в структуру без кавычек
                    if ((number_of_quotes == 0 || number_of_quotes == 2) && number_of_commas == 0) {

                        json_columns[number_of_headers - 1].column->values = realloc(json_columns[number_of_headers - 1].column->values, sizeof (char *) * (array_size + 2));
                        json_columns[number_of_headers - 1].column->values[array_size] = (char *) malloc(sizeof (char));

                        /*// Пропускаем пробелы
                        for (spaces = 0; string[spaces] == ' '; ++spaces);*/

                        // Читаем строку string
                        for (int j = spaces; string[j] != '\0'; ++j) {
                            if (string[j] == ',')
                                break;
                            if (string[j] != '"') {
                                json_columns[number_of_headers - 1].column->values[array_size] = realloc(json_columns[number_of_headers - 1].column->values[array_size], sizeof (char) * (k + 2));
                                json_columns[number_of_headers - 1].column->values[array_size][k] = string[j];
                                ++k;
                            }
                        }
                        json_columns[number_of_headers - 1].column->values[array_size][k] = '\0';
                        //printf("%s\n", json_columns[number_of_headers - 1].column->values[array_size]);
                        array_size += 1;
                    }
                    // Если число кавычек четное или количество запятых != 0, то записываем в структуру в кавычках
                    else if ((number_of_quotes % 2 == 0) || (number_of_commas > 0 && (number_of_quotes % 2 == 0))) {

                        json_columns[number_of_headers - 1].column->values = realloc(json_columns[number_of_headers - 1].column->values, sizeof (char *) * (array_size + 2));
                        json_columns[number_of_headers - 1].column->values[array_size] = (char *) malloc(sizeof (char));

                        /*// Пропускаем пробелы
                        for (spaces = 0; string[spaces] == ' '; ++spaces);*/

                        // Читаем строку string
                        for (int j = spaces; string[j] != '\0'; ++j) {
                            json_columns[number_of_headers - 1].column->values[array_size] = realloc(json_columns[number_of_headers - 1].column->values[array_size], sizeof (char) * (k + 2));
                            json_columns[number_of_headers - 1].column->values[array_size][k] = string[j];
                            ++k;
                        }
                        json_columns[number_of_headers - 1].column->values[array_size][k - 1] = '\0';
                        //printf("%s\n", json_columns[number_of_headers - 1].column->values[array_size]);
                        array_size += 1;
                    }
                    // Если число кавычек нечетное, то выводим пользователю сообщение о вероятной ошибке в данных
                    else {
                        puts("Something wrong with data in the json file.");
                    }
                    number_of_quotes = 0;
                }
                puts("\nData configured.");
            }
            array_size = 0;
            number_of_commas = 0;
            number_of_quotes = 0;
            k = 0;
            i = 0;
            str_size = 0;
        }
    }
    puts("Headers and data configured.");
}

// Parsing json file to csv file
void json_to_csv(FILE *parse_file, char const *csv_output) {

    // Инициализация структуры для записи в нее json файла
    HEADERS* json_columns = NULL;
    json_columns = init_HEADERS(json_columns);

    // Запись в структуру заголовков и данных
    write_headers_and_data_in_struct(parse_file, json_columns);

    // Открытие файла, который указан в #define csv_output
    FILE *csv = fopen(csv_output, "w");

    // Проверка открытия файла, вывод ошибки в случае неудачи
    if (csv == NULL) {
        puts("Can't open output file.");
        //clear_struct(json_columns);
        fclose(csv);
        exit(-1);
    }

    // Запись в файл информации из структуры

    // Закрытие файла
    fclose(csv);
}

// Parsing csv file to json file
void csv_to_json() {

}