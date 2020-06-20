# Это репозиторий с выполненым тетстовым заданием AVSoft

Первое задание на многопоточность с WinAPI
Второе задание это менеджр управления XML файлом
Полное описание в pdf файле

## Первое задание

Так как в этом задании нужно использовать WinAPI, то для разработки была выбрана среда
Visual Studio 2019.
Я добавил все обслуживаемые файлы, для запуска проекта, но если в этом необходимости нет,
то путь до файла в этом репозитории такой
```
MultiThreadWIN/AVSoft/AVSoft.cpp
```
Настройки проекта стандратные, тип проекта - консольное приложение  С++

Листинг работы
Без сброса разделяемой памяти
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise1_without_flush.png?raw=true)
С сбросом разделяемой памяти
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise1_with_flush.png?raw=true)

## Второе задание

Разработка велась на операционной система Mac OS с использованием среды разработки CLion.
Сборка проекта производилась CMake файлом.
Более подробная документация кода представлена в main файле, путь до него
```
ManagerXML/main.cpp
```

В директории cmake-build-debug представлен собраный проект, если по каким-то причинам не удасться собрать cmake файлом

Настройки сборки
```
cmake --build cmake-build-debug --target AVSoft2 -- -j 4
```

Листинг работ
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise2_part1.png?raw=true)
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise2_part2.png?raw=true)
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise2_part3.png?raw=true)
![alt text](https://github.com/aeroshev/AVSoft/images/Exercise2_part4.png?raw=true)