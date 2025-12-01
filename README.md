# Багатопотоковий веб-сервіс для швидкого повнотекстового пошуку по файлах

---

## Інструкція для збірки та запуску проекту

---

## Вимоги

- Windows 10/11  
- Visual Studio 2019 або 2022  
- C++17 або новіше  
- Python 3.8+ (для веб-клієнта)

---

## Як зібрати

### 1. Клонування репозиторію

```bash
git clone https://github.com/GeLo0000/KPI-PO_Coursework
cd your-repo-name
```

### 2. Налаштування Visual Studio

- Відкрийте .sln у Visual Studio.
- Увімкніть C++17:
`Project Properties → C/C++ → Language → C++ Language Standard → ISO C++17.`
- Додайте бібліотеку сокетів:
`Project Properties → Linker → Input → Additional Dependencies → ws2_32.lib`

### 3. Компіляція

- Оберіть Release або Debug.
- Виконайте Build Solution.

---

## Як запустити

### 1. Підготовка даних

У директорії з серверним(`WebServer`) .exe  має бути папка data/ зі звичайними .txt файлами.
- Примітка: Якщо ви запускаєте з Visual Studio, створіть папку data у корені проекту (поруч з .vcxproj).

### 2. Запуск сервера

Запустіть проєкт WebServer.

Очікуваний вивід:
```bash
System initialized.
[SCHEDULER] Started monitoring directory: data
Server started on port 8080. Waiting for connections...
```

### Крок 3. Запуск Клієнтів

Ви можете використовувати один із двох клієнтів:

#### Варіант А: Консольний C++ Клієнт

1. Запустіть проект Client (або запустіть .exe файл клієнта).
2. Введіть слово для пошуку у консолі.

#### Варіант Б: Веб-інтерфейс (Python + Flask)

1. Встановіть бібліотеку Flask (якщо не встановлена):
```bash
pip install flask
```

2. Перейдіть у папку з Python-клієнтом:
```bash
cd WebClient
```

3. Запустіть веб-сервер:
```bash
python app.py
```

4. Відкрийте браузер за адресою: http://127.0.0.1:5000

5. Введіть слово для пошуку.

---

## Як запустити тестування

Проект містить скрипти для навантажувального тестування за допомогою Locust.
1. Встановіть Locust: `pip install locust`.
2. Запустіть сервер C++.
3. Запустіть тест: `python -m locust`
4. Відкрийте http://localhost:8089 та задайте параметри навантаження (наприклад, 100 users, 10 spawn rate).
