import socket
import struct
from flask import Flask, render_template, request, jsonify

# Створюємо додаток Flask
app = Flask(__name__)

# Налаштування підключення до C++ (Кухня)
CPP_HOST = '127.0.0.1'
CPP_PORT = 8080
CMD_SEARCH = 0x01

# --- Функція-клієнт (Та сама, що була в Client_PY, але тепер як функція) ---
def query_cpp_server(word):
    """
    Ця функція працює як офіціант:
    1. Біжить до C++.
    2. Віддає слово.
    3. Приносить список файлів.
    """
    try:
        # Відкриваємо "двері" (сокет) до C++ сервера
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(2.0) # Чекаємо максимум 2 секунди, щоб не зависнути
            s.connect((CPP_HOST, CPP_PORT))

            # --- ПІДГОТОВКА ЗАМОВЛЕННЯ ---
            word_bytes = word.encode('utf-8')
            # Пакуємо: Команда (1 байт) + Довжина (4 байти) + Слово
            header = struct.pack('<Bi', CMD_SEARCH, len(word_bytes))
            
            # Відправляємо
            s.sendall(header)
            s.sendall(word_bytes)

            # --- ОТРИМАННЯ СТРАВИ (Відповіді) ---
            
            # 1. Читаємо статус (OK, EMPTY або ERROR)
            status_data = s.recv(1)
            if not status_data:
                return {"error": "C++ сервер розірвав з'єднання"}
            
            status = struct.unpack('<B', status_data)[0]

            if status == 0x02: # RESP_EMPTY
                return {"files": []} # Повертаємо пустий список
            
            if status == 0x03: # RESP_ERROR
                return {"error": "C++ сервер повернув помилку"}

            if status == 0x01: # RESP_OK
                # 2. Читаємо кількість файлів
                count_data = s.recv(4)
                count = struct.unpack('<i', count_data)[0]
                
                found_files = []
                
                # 3. Читаємо назви файлів одну за одною
                for _ in range(count):
                    # Довжина назви
                    len_data = s.recv(4)
                    name_len = struct.unpack('<i', len_data)[0]
                    
                    # Сама назва
                    name_bytes = s.recv(name_len)
                    filename = name_bytes.decode('utf-8')
                    found_files.append(filename)
                
                return {"files": found_files}
            
            return {"error": "Невідомий статус від сервера"}

    except ConnectionRefusedError:
        return {"error": "Не можу підключитись до C++. Сервер запущено?"}
    except Exception as e:
        return {"error": str(e)}

# --- Маршрути веб-сайту ---

# Головна сторінка (http://localhost:5000/)
@app.route('/')
def index():
    # Ця функція просто показує наш HTML файл
    return render_template('index.html')

# API для пошуку (http://localhost:5000/search?word=fox)
@app.route('/search')
def search():
    # 1. Офіціант чує, що хоче клієнт (бере слово з адреси)
    word = request.args.get('word', '')
    
    if not word:
        return jsonify({"error": "Пустий запит"})

    # 2. Офіціант біжить на кухню (викликаємо функцію сокетів)
    result = query_cpp_server(word)

    # 3. Віддає результат у форматі JSON (зрозумілому для JavaScript)
    return jsonify(result)

# Запуск веб-сервера
if __name__ == '__main__':
    print("Запускаю веб-сайт на http://127.0.0.1:5000")
    app.run(debug=True, port=5000)