#include "functions.h"

/*<----------------------------------------------------Functions---------------------------------------------------->*/
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
                return 1; // json формат
            }
        }
    }
    return 2; // csv формат
}

void write_headers_to_csv(FILE *csv, HEADERS *json_columns) {
    for (int i = 0; i < json_columns->number_headers; ++i) {
        fprintf(csv, "%s", json_columns[i].column->header_name);
        if (i + 1 != json_columns->number_headers)
            fprintf(csv, "%c", ',');
        else
            fprintf(csv, "%c", '\n');
    }
}

void write_values_to_csv(FILE *csv, HEADERS *json_columns) {
    int i = 0;

    int max = 0;

    for (int j = 0; j < json_columns->number_headers; ++j) {
        if (json_columns[j].column->number_of_values > max)
            max = json_columns[j].column->number_of_values;
    }
    /*if (max > 1)
        max = max - 1;*/

    /* Тут происходят страшные вещи */
    for (int j = 0; j < max; ++j) {
        for (i = 0; i < json_columns->number_headers; ++i) {
            if (json_columns[i].column->number_of_values > j) {
                fprintf(csv, "%s", json_columns[i].column->values[j]);
                if (i + 1 != json_columns->number_headers)
                    fprintf(csv, "%c", ',');
                else
                    fprintf(csv, "%c", '\n');
            } else {
                if (i + 1 != json_columns->number_headers)
                    fprintf(csv, "%c", ',');
                else
                    fprintf(csv, "%c", '\n');
            }
        }
    }
}

// Функция записи в csv файл из структуры HEADERS
void write_data_to_csv(FILE *csv, HEADERS *json_columns) {
    write_headers_to_csv(csv, json_columns);
    write_values_to_csv(csv, json_columns);
}

// Функция для очистки структуры
void delete_struct(HEADERS *structure) {
    for (int i = 0; i < structure[i].number_headers; ++i) {
        free(structure[i].column->header_name);
        for (int j = 0; j < structure[i].column->number_of_values; ++j) {
            free(structure[i].column->values[j]);
        }
    }
    for (int i = 0; i < structure[i].number_headers; ++i) {
        free(structure[i].column);
    }
    free(structure);
}

char *read_string_from_file(char* string, FILE *parse_file) {
    char c;
    int i;

    for (i = 0; (c = fgetc(parse_file)) != '\n' && c != EOF; ++i) {
        string = realloc(string, sizeof (char) * (i + 2));
        string[i] = c;
    }
    string[i] = '\0';

    return string;
}

char *make_token_correct(char* token) {

    if (token != NULL) {

        // Если первая кавычка, то удаляем ее из токена
        if (token[0] == '"') {
            int i;
            for (i = 0; token[i + 1] != '\0'; ++i) {
                if (i >= 1) {
                    token[i - 1] = token[i];
                }
            }
            token[i - 1] = '\0';
        }
        // Если последняя кавычка, то удаляем ее
        for (int i = 0; token[i + 1] == '\0'; ++i) {
            if (token[i] == '"' && token[i + 1] == '\0')
                token[i] = '\0';
        }

    }

    return token;
}

/*<------------------------------------------------HEADERS functions------------------------------------------------>*/
HEADERS* init_HEADERS(HEADERS *headers_to_init) {

    headers_to_init = (HEADERS *) malloc(sizeof (HEADERS));
    headers_to_init->column = NULL;
    headers_to_init->number_headers = 0;

    return headers_to_init;
}

HEADER* init_column(HEADER *column_to_init) {

    column_to_init = (HEADER *) malloc(sizeof (HEADER));
    column_to_init->header_name = (char *) malloc(sizeof (char));
    column_to_init->values = (char **) malloc(sizeof (char *));
    column_to_init->number_of_values = 0;

    return column_to_init;
}

HEADERS *write_headers_and_data_in_struct(FILE *parse_file, HEADERS *json_columns) {
    char c; // Переменная для чтения
    int i;
    int k = 0; // Позиция символа
    int spaces = 0; // Число пробелов
    int flag = 0; // Флаг

    int array_size = 0; // Размер массива
    int str_size = 0; // Размер строки
    int old_position; // Старая позиция на запись в структуру

    char *string = (char *) malloc(sizeof (char ));

    int number_of_headers = 0;
    int number_of_quotes = 0;
    int number_of_commas = 0;

    rewind(parse_file);

    while ((c = fgetc(parse_file)) != EOF) {
        if (c == '"') {

            json_columns = realloc(json_columns, sizeof (HEADERS) * (number_of_headers + 1));
            json_columns[number_of_headers].column = init_column(json_columns[number_of_headers].column);

            /*for (i = 0; (c = fgetc(parse_file)) != ':'; i++) {
                json_columns[number_of_headers].column->header_name = realloc(json_columns[number_of_headers].column->header_name, sizeof (char) * (i + 1));
                json_columns[number_of_headers].column->header_name[i] = c;
            }

            json_columns[number_of_headers].column->header_name[i - 1] = '\0';*/

            // Считываем хедер в строку
            for (str_size = 0; (c = fgetc(parse_file)) != ':'; ++str_size) {
                string = realloc(string, sizeof (char) * (str_size + 1));
                string[str_size] = c;
            }
            string[str_size - 1] = '\0';
            //printf("%s", string);

            // Просматриваем хедеры в структуре, отмечаем флагом, если совпало
            for (int j = 0; j < json_columns->number_headers; ++j) {
                if (strcmp(string, json_columns[j].column->header_name) == 0) {
                    old_position = number_of_headers;
                    number_of_headers = j;
                    flag = 1;
                    //puts("Double");
                    break;
                }
            }
            // Если хедер уникален, то выделяем паямть под него в структуре и копируем
            if (flag == 0) {
                json_columns[number_of_headers].column->header_name = realloc(json_columns[number_of_headers].column->header_name, sizeof (char) * (str_size + 1));
                strcpy(json_columns[number_of_headers].column->header_name, string);
            }

            /*
             * Проверяем, есть ли в заголовке другие заголовки
             * 1. Если есть, то вместо только что записанного пишем новый
             * 2. Если нет, то увеличиваем число заголовков на 1
             */

            for (str_size = 0; (c = fgetc(parse_file)) != '\n'; ++str_size) {
                string = realloc(string, sizeof (char ) * (str_size + 1));
                string[str_size] = c;

                if (c == '{' || c == '[') {
                    break;
                }
            }
            string[str_size] = '\0';

            //printf("%s", string);

            if (c != '{' && c != ' ' && c != '[') {

                // Считаем число кавычек и запятых
                for (int j = 0; string[j] != '\0'; ++j) {
                    if (string[j] == '"')
                        number_of_quotes++;
                    if (string[j] == ',' && j != strlen(string) - 1)
                        number_of_commas++;
                }
                // Если число кавычек 0 или 2, а число запятых 0, то нам нужно записать значение в структуру без кавычек
                if ((number_of_quotes == 0 || number_of_quotes == 2) && number_of_commas == 0) {
                    if (flag == 0) {
                        json_columns[number_of_headers].column->values[0] = (char *) malloc(sizeof (char));
                    }
                    if (flag == 1) {
                        json_columns[number_of_headers].column->values = realloc(json_columns[number_of_headers].column->values, sizeof (char *) * (json_columns[number_of_headers].column->number_of_values + 1));
                        json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values] = (char *) malloc(sizeof (char));
                        //json_columns[number_of_headers].column->number_of_values += 1;
                    }

                    // Пропускаем пробелы
                    for (spaces = 0; string[spaces] == ' '; ++spaces);

                    // Читаем строку string
                    for (int j = spaces; string[j] != '\0'; ++j) {
                        if (string[j] == ',')
                            break;
                        if (string[j] != '"') {
                            if (flag == 0) {
                                json_columns[number_of_headers].column->values[0] = realloc(json_columns[number_of_headers].column->values[0], sizeof (char) * (k + 1));
                                json_columns[number_of_headers].column->values[0][k] = string[j];
                                ++k;
                            }
                            if (flag == 1) {
                                json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values] = realloc(json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values], sizeof (char) * (k + 1));
                                json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k] = string[j];
                                ++k;
                            }
                        }
                    }
                    if (flag == 0) {
                        json_columns[number_of_headers].column->values[0][k] = '\0';
                        json_columns[number_of_headers].column->number_of_values = 1;
                    }
                    if (flag == 1) {
                        json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k] = '\0';
                        json_columns[number_of_headers].column->number_of_values += 1;
                    }
                    //printf("%s\n", json_columns[number_of_headers].column->values[0]);
                }
                // Если число кавычек четное или количество запятых != 0, то записываем в структуру в кавычках
                else if ((number_of_quotes % 2 == 0) || (number_of_commas > 0 && (number_of_quotes % 2 == 0))) {

                    if (flag == 0) {
                        json_columns[number_of_headers].column->values[0] = (char *) malloc(sizeof (char));
                    }
                    if (flag == 1) {
                        json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values] = (char *) malloc(sizeof (char));
                    }

                    // Пропускаем пробелы
                    for (spaces = 0; string[spaces] == ' '; ++spaces);

                    // Читаем строку string
                    for (int j = spaces; string[j] != '\0'; ++j) {
                        if (flag == 0) {
                            json_columns[number_of_headers].column->values[0] = realloc(json_columns[number_of_headers].column->values[0], sizeof (char) * (k + 1));
                            json_columns[number_of_headers].column->values[0][k] = string[j];
                            ++k;
                        }
                        if (flag == 1) {
                            json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values] = realloc(json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values], sizeof (char) * (k + 1));
                            json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k] = string[j];
                            ++k;
                        }
                    }
                    if (flag == 0) {
                        if (json_columns[number_of_headers].column->values[0][k - 1] != ',')
                            json_columns[number_of_headers].column->values[0][k] = '\0';
                        else {
                            json_columns[number_of_headers].column->values[0][k - 1] = '\0';
                        }
                        json_columns[number_of_headers].column->number_of_values = 1;
                    }
                    if (flag == 1){
                        if (json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k - 1] != ',')
                            json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k] = '\0';
                        else {
                            json_columns[number_of_headers].column->values[json_columns[number_of_headers].column->number_of_values][k - 1] = '\0';
                        }
                        json_columns[number_of_headers].column->number_of_values += 1;
                    }
                    //printf("%s\n", json_columns[number_of_headers].column->values[0]);
                }
                // Если число кавычек нечетное, то выводим пользователю сообщение о вероятной ошибке в данных
                else {
                    puts("Something wrong with data in the json file.");
                    // Записать ошибку
                }
                if (flag == 0)
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
                        string = realloc(string, sizeof (char ) * (str_size + 1));
                        string[str_size] = c;

                        if (c == '{' || c == '[') {
                            puts("Can't parse the data.");
                            break;
                        }

                        c = fgetc(parse_file);
                    }
                    if (string[str_size - 1] == ',') {
                        //printf("\n%s string before\n", string);
                        string[str_size - 1] = '\0';
                        //printf("\n%s string after\n", string);
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

                        json_columns[number_of_headers].column->values = realloc(json_columns[number_of_headers].column->values, sizeof (char *) * (array_size + 2));
                        json_columns[number_of_headers].column->values[array_size] = (char *) malloc(sizeof (char));

                        /*// Пропускаем пробелы
                        for (spaces = 0; string[spaces] == ' '; ++spaces);*/

                        // Читаем строку string
                        for (int j = 0; string[j] != '\0'; ++j) {
                            if (string[j] == ',')
                                break;
                            if (string[j] != '"') {
                                json_columns[number_of_headers].column->values[array_size] = realloc(json_columns[number_of_headers].column->values[array_size], sizeof (char) * (k + 2));
                                json_columns[number_of_headers].column->values[array_size][k] = string[j];
                                ++k;
                            }
                        }
                        json_columns[number_of_headers].column->values[array_size][k] = '\0';
                        //printf("%s\n", json_columns[number_of_headers - 1].column->values[array_size]);
                        json_columns[number_of_headers].column->number_of_values += 1;
                        array_size += 1;
                    }
                    // Если число кавычек четное или количество запятых != 0, то записываем в структуру в кавычках
                    else if ((number_of_quotes % 2 == 0) || (number_of_commas > 0 && (number_of_quotes % 2 == 0))) {

                        json_columns[number_of_headers].column->values = realloc(json_columns[number_of_headers].column->values, sizeof (char *) * (array_size + 2));
                        json_columns[number_of_headers].column->values[array_size] = (char *) malloc(sizeof (char));

                        /*// Пропускаем пробелы
                        for (spaces = 0; string[spaces] == ' '; ++spaces);*/

                        // Читаем строку string
                        for (int j = 0; string[j] != '\0'; ++j) {
                            json_columns[number_of_headers].column->values[array_size] = realloc(json_columns[number_of_headers].column->values[array_size], sizeof (char) * (k + 2));
                            json_columns[number_of_headers].column->values[array_size][k] = string[j];
                            ++k;
                        }
                        json_columns[number_of_headers].column->values[array_size][k - 1] = '\0';
                        //printf("%s\n", json_columns[number_of_headers - 1].column->values[array_size]);
                        json_columns[number_of_headers].column->number_of_values += 1;
                        array_size += 1;
                    }
                    // Если число кавычек нечетное, то выводим пользователю сообщение о вероятной ошибке в данных
                    else {
                        puts("Something wrong with data in the json file.");
                    }
                    number_of_quotes = 0;
                }
                number_of_headers = number_of_headers + 1;
                //puts("\nData configured.");
            }
            if (flag == 1)
                number_of_headers = old_position;

            json_columns->number_headers = number_of_headers;

            flag = 0;
            array_size = 0;
            number_of_commas = 0;
            number_of_quotes = 0;
            k = 0;
            i = 0;
            str_size = 0;
        }
    }
    json_columns->number_headers = number_of_headers;
    puts("Headers and data configured.");
    return json_columns;
}

/*<------------------------------------------------Parsing functions------------------------------------------------>*/
// Parsing json file to csv file
void json_to_csv(FILE *parse_file, char const *csv_output) {

    // Инициализация структуры для записи в нее json файла
    HEADERS* json_columns = NULL;
    json_columns = init_HEADERS(json_columns);

    // Запись в структуру заголовков и данных
    json_columns = write_headers_and_data_in_struct(parse_file, json_columns);

    // Открытие файла, который указан в #define csv_output
    FILE *csv = fopen(csv_output, "w");

    // Проверка открытия файла, вывод ошибки в случае неудачи
    if (csv == NULL) {
        puts("Can't open output file.");
        //clear_struct(json_columns);
        fclose(parse_file);
        delete_struct(json_columns);
        exit(-1);
    }

    // Запись в файл информации из структуры
    puts("Writing data ...");
    write_data_to_csv(csv, json_columns);

    // Удаление структуры
    delete_struct(json_columns);

    // Закрытие файла
    fclose(csv);
}

// Parsing csv file to json file
void csv_to_json(FILE *parse_file, char const *json_output) {

    rewind(parse_file);

    char **headers = (char **) malloc(sizeof (char *));
    char *headers_read = NULL;
    char *data_read = NULL;

    int headers_count = 0;
    int data_count = 0;

    const char *separator = ",";
    char *token;

    // Открываем файл
    FILE *json = fopen(json_output, "w");

    // Проверка открытия файла, вывод ошибки в случае неудачи
    if (json == NULL) {
        puts("Can't open output file.");
        fclose(parse_file);
        exit(-1);
    }

    // Читаем хедеры
    headers_read = read_string_from_file(headers_read, parse_file);

    // Получаем строку с хедерами, проверяем, что получили
    if (strcmp(headers_read, "") == 0) {
        printf("Can't parse the data.");
        free(headers);
        fclose(parse_file);
        fclose(json);
        exit(-1);
    }

    // Разбиваем хедеры по сепаратору
    token = strtok(headers_read, separator);
    while (token != NULL) {

        headers = realloc(headers, sizeof (char *) * (headers_count + 1));
        headers[headers_count] = (char *) malloc(sizeof (char) * (MAX_LENGTH_HEADER));
        headers[headers_count] = strdup(token);

        headers_count++;
        token = strtok(NULL, separator);

    }

    // Записываем json
    fputs("[\n\t", json);

    // Цикл выполняется, пока в файле есть строки
    while (strcmp((data_read = read_string_from_file(data_read, parse_file)), "") != 0) {

        if (data_count > 0)
            fprintf(json, ",\n\t");
        fprintf(json, "{\n\t\t");

        // Получаем новый токен
        token = strtok(data_read, separator);
        // Если токен -- строка с запятыми или больше, чем с 2 кавычками, то функция убирает внешние кавычки у токена
        token = make_token_correct(token);

        for (int i = 0; token != NULL && i <= (headers_count - 1); i++) {
            fprintf(json, "\"%s\": \"%s\"", headers[i], token);

            // Получаем новый токен
            token = strtok(NULL, separator);
            // Если токен -- строка с запятыми или больше, чем с 2 кавычками, то функция убирает внешние кавычки у токена
            token = make_token_correct(token);

            if (i <= (headers_count - 2))
                fprintf(json, ",\n\t\t");
        }

        fprintf(json, "\n\t}");
        data_count++;
    }

    fputs("\n]", json);

    // Закрываем файлы
    fclose(json);
    fclose(parse_file);

    // Очищаем память
    free(headers_read);
    free(data_read);
    free(token);
    for (int i = 0; i < headers_count; i++)
        free(headers[i]);
}