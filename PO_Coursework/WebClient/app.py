import socket
import struct
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Server Configuration
CPP_HOST = '127.0.0.1'
CPP_PORT = 8080

# Protocol Constants (Must match Protocol.h in C++)
COMMAND_SEARCH = 0x01
RESPONSE_OK = 0x01
RESPONSE_EMPTY = 0x02
RESPONSE_ERROR = 0x03

def query_cpp_server(word):
    """
    Connects to the C++ server via TCP socket, sends the search query,
    and parses the binary response.
    
    Args:
        word (str): The keyword to search for.
        
    Returns:
        dict: A dictionary containing either a list of files or an error message.
    """
    try:
        # Create TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(2.0) # Timeout to prevent hanging
            s.connect((CPP_HOST, CPP_PORT))

            # --- Send Request ---
            word_bytes = word.encode('utf-8')
            # Pack: Command (1 byte) + Length (4 bytes)
            header = struct.pack('<Bi', COMMAND_SEARCH, len(word_bytes))
            
            s.sendall(header)
            s.sendall(word_bytes)

            # --- Receive Response ---
            
            # 1. Read Status (1 byte)
            status_data = s.recv(1)
            if not status_data:
                return {"error": "Connection closed by C++ server"}
            
            status = struct.unpack('<B', status_data)[0]

            if status == RESPONSE_EMPTY:
                return {"files": []}
            
            if status == RESPONSE_ERROR:
                return {"error": "Server returned an error status"}

            if status == RESPONSE_OK:
                # 2. Read File Count (4 bytes)
                count_data = s.recv(4)
                count = struct.unpack('<i', count_data)[0]
                
                found_files = []
                
                # 3. Read Filenames loop
                for _ in range(count):
                    # Read name length
                    len_data = s.recv(4)
                    name_len = struct.unpack('<i', len_data)[0]
                    
                    # Read name string
                    name_bytes = s.recv(name_len)
                    filename = name_bytes.decode('utf-8')
                    found_files.append(filename)
                
                return {"files": found_files}
            
            return {"error": "Unknown response status"}

    except ConnectionRefusedError:
        return {"error": "Could not connect to C++ server. Is it running?"}
    except Exception as e:
        return {"error": str(e)}

# --- Web Routes ---

@app.route('/')
def index():
    """Renders the main search page."""
    return render_template('index.html')

@app.route('/search')
def search():
    """API Endpoint to handle search requests from the frontend."""
    word = request.args.get('word', '')
    
    if not word:
        return jsonify({"error": "Empty query"})

    result = query_cpp_server(word)
    return jsonify(result)

if __name__ == '__main__':
    print(f"Starting Web Client on http://127.0.0.1:5000")
    app.run(debug=True, port=5000)