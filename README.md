## Самое важное:

    0. Это чудо написано без библиотеки cJSON, запускается через parser_json_csv.exe

    1. В файл parse.txt вставляется json/csv, после запуска программы необходимо указать путь к parse.txt (./parse.txt)

    2. Результат работы программы записывается в output.csv/output.json. Можно изменить это в #define в main.c

    3. В папке examples есть примеры json и csv для удобства

### Примечание:

#### json -> csv

    1.  Изначально при встрече повторного ключа значения в структуре ОБНОВЛЯЛИСЬ, (решил попробовать расширить 
        функционал парсера, поэтому много закомменченного кода) сейчас же они ДОБАВЛЯЮТСЯ к тем, что уже были записаны 
        до этого, соответственно, на это есть некоторые ограничения: 
            А) значение можно добавить только к простым хедерам (в них есть только значение), при этом 
        если попытаться добавить значения к хедеру, где уже есть несколько значений, то ОБЯЗАТЕЛЬНО нужно во все другие
        хедеры добавить "null" или " " (examples/JSON3.txt), иначе значения в csv файле перемешаются
            Б) пока что нельзя добавить к значениям массив значений, только по одному с учетом пункта А)

    2.  Если в хедере есть вложенный(ые) хедер(ы), то код обрабатывает только вложенный(ые) хедер(ы), а внешний 
        игнорируется

	3.  В качестве значений массива допустимо записывать строки/числа

###### P.S. ~~я обязамтельно доработаю~~...