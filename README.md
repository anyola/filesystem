# Иерархическая файловая система (C++20)

Реализация иерархической файловой системы на **C++20** с полиморфной иерархией объектов и явным владением через `std::unique_ptr`.

## Архитектура

* `fsobject` — абстрактный базовый класс с общим интерфейсом объектов файловой системы, именем и указателем на родительский объект.
* `file` — лист дерева, хранящий содержимое в виде строки.
* `directory` — контейнер дочерних объектов, владеющий ими через `std::vector<std::unique_ptr<fsobject>>`.
* `symlink` — объект, хранящий путь до целевого объекта и поддерживающий разрешение цепочек символических ссылок.

Глубокое копирование реализовано через виртуальный метод `clone()`. При копировании директории рекурсивно создаётся независимая копия всего поддерева.

Перемещение объектов между директориями выполняется передачей `std::unique_ptr`, поэтому один объект всегда имеет единственного владельца.

## Возможности

* поиск объектов по имени с рекурсивным обходом дерева;
* поиск объектов по пути с последовательным разбором сегментов;
* добавление, удаление и переименование объектов;
* проверка дублирования имён и недопустимых операций;
* глубокое копирование файлов и директорий;
* перемещение объектов между директориями;
* защита от перемещения и копирования директории в саму себя или в собственного потомка;
* разрешение цепочек `symlink`;
* обнаружение циклических и битых символических ссылок;
* ограничение максимальной глубины разрешения `symlink`.

## Управление ресурсами

Владение дочерними объектами реализовано через RAII:

```cpp
std::vector<std::unique_ptr<fsobject>> children;
```

`directory` является владельцем своих дочерних объектов. При удалении директории все принадлежащие ей объекты автоматически уничтожаются.

Перемещение выполняется без копирования объекта:

```cpp
std::unique_ptr<fsobject> object = source.remove_child(name);
target.add_child(std::move(object));
```

Копирование создаёт независимую структуру объектов через `clone()`.

## Тестирование и статический анализ

Unit-тесты написаны с использованием **GoogleTest** и включают обычные сценарии и граничные случаи.

Проект собирается с флагами:

```text
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Werror
```

Для анализа используются:

* **clang-tidy** — `modernize-*`, `performance-*`, `bugprone-*`, `clang-analyzer-*`
* **AddressSanitizer**
* **UndefinedBehaviorSanitizer**


## Сборка и запуск тестов

```bash
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

ctest --output-on-failure
```

## Пример использования

```cpp
#include "directory.hpp"
#include "file.hpp"
#include "symlink.hpp"

filesystem::directory root("root");

filesystem::directory* docs =
    static_cast<filesystem::directory*>(
        root.add_child(
            std::make_unique<filesystem::directory>("docs")
        )
    );

docs->add_child(
    std::make_unique<filesystem::file>(
        "report.txt",
        "Q3 report"
    )
);

// Symlink на файл внутри docs/
root.add_child(
    std::make_unique<filesystem::symlink>(
        "latest",
        "docs/report.txt"
    )
);

// Поиск по имени
filesystem::fsobject* found = root.find("report.txt");

found->get_path();
// "/root/docs/report.txt"

// Перемещение между директориями
filesystem::directory* backup =
    static_cast<filesystem::directory*>(
        root.add_child(
            std::make_unique<filesystem::directory>("backup")
        )
    );

docs->move("report.txt", *backup);

// Глубокое копирование всей структуры
std::unique_ptr<filesystem::fsobject> cloned = root.clone();
```
