import socket
import struct

# Константи протоколу
CMD_SEARCH = 0x01

def main():
    HOST = '127.0.0.1'
    PORT = 8080

    while True:
        word = input("\nEnter word to search (or 'exit'): ")
        if word == 'exit': break

        # Створюємо сокет
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            try:
                s.connect((HOST, PORT))
                
                # --- ВІДПРАВКА ---
                # Pack formats: B = unsigned char (1 byte), i = int (4 bytes)
                # encode перетворює рядок в байти
                word_bytes = word.encode('utf-8')
                
                # Відправляємо: [CMD] [LEN] [WORD]
                s.sendall(struct.pack('<Bi', CMD_SEARCH, len(word_bytes)))
                s.sendall(word_bytes)

                # --- ОТРИМАННЯ ---
                # 1. Статус (1 байт)
                data = s.recv(1)
                if not data: break
                status = struct.unpack('<B', data)[0]

                if status == 0x01: # RESP_OK
                    # 2. Кількість (4 байти)
                    count_data = s.recv(4)
                    count = struct.unpack('<i', count_data)[0]
                    print(f"Found in {count} files:")

                    # 3. Читаємо імена файлів
                    for _ in range(count):
                        # Довжина імені
                        name_len_data = s.recv(4)
                        name_len = struct.unpack('<i', name_len_data)[0]
                        
                        # Саме ім'я
                        name_bytes = s.recv(name_len)
                        print(f"- {name_bytes.decode('utf-8')}")
                
                elif status == 0x02: # RESP_EMPTY
                    print("Word not found.")
                else:
                    print("Server returned error.")

            except ConnectionRefusedError:
                print("Could not connect to server. Is it running?")
                break

if __name__ == "__main__":
    main()
