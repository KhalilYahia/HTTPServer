# HTTPServer (C)

Простой HTTP-сервер, написанный на языке **C**, который обслуживает статические файлы из папки `public` на порту **8080**.

---

## Возможности
- Написан на C (стандарт **C99**)
- Сборка с помощью **CMake**
- Обслуживание статических файлов

---

## Требования
- **CMake** версии 3.16 или новее
- Компилятор C:
  - Windows: MSVC (Visual Studio Build Tools) или MinGW
  - Linux: GCC
- (Рекомендуется) **Visual Studio Code** и расширения:
  - **C/C++** (Microsoft)
  - **CMake Tools** (Microsoft)

---

## Структура проекта
- `src/` — исходные файлы
- `include/` — заголовочные файлы
- `public/` — файлы, доступные через HTTP-сервер
- `CMakeLists.txt` — конфигурация сборки

---

## Сборка проекта

В корневой папке проекта выполните:

```bash
cmake -S . -B build
cmake --build build
```

---

## Запуск сервера

### Запуск через VS Code (PowerShell) — Windows

1. Откройте проект в VS Code.
2. Откройте терминал: **Terminal → New Terminal** и выберите **PowerShell**.
3. Убедитесь, что вы находитесь в корневой папке проекта.
4. Запустите сервер:

```powershell
.\build\Debug\HTTPServer.exe
```

> Если проект собран в режиме **Release**, используйте:

```powershell
.\build\Release\HTTPServer.exe
```

---

### Linux

Запустите сервер:

```bash
./build/HTTPServer
```

---


## Проверка работы

После запуска сервера откройте браузер и перейдите по адресу:

- Главная страница: `http://localhost:8080/`
- Пример файла: `http://localhost:8080/file.txt`

Порт по умолчанию: `8080`  
Корневая папка сервера: `public`


