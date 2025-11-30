import socket
import struct
import time
import random
from locust import User, task, between, events

# Server Configuration
CPP_HOST = '127.0.0.1'
CPP_PORT = 8080

# Protocol Constants
CMD_SEARCH = 0x01

# List of words for random search queries to avoid caching effects
SEARCH_WORDS = ["fox", "dog", "internet", "code", "java", "cpp", "python", "master", "1", "test"]

class CppServerUser(User):
    """
    Simulates a user interacting with the C++ server via raw TCP sockets.
    """
    
    # Wait time between tasks simulating realistic user behavior (0.5 to 2 seconds)
    wait_time = between(0.5, 2)

    def send_search_request(self, word):
        """
        Sends a binary search request to the C++ server and measures the response time.
        """
        start_time = time.time()
        
        try:
            # 1. Open a TCP socket
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(1.0)  # 1-second timeout
                s.connect((CPP_HOST, CPP_PORT))

                # 2. Send Request
                word_bytes = word.encode('utf-8')
                # Pack: Command (1 byte) + Length (4 bytes)
                header = struct.pack('<Bi', CMD_SEARCH, len(word_bytes))
                
                s.sendall(header)
                s.sendall(word_bytes)

                # 3. Receive Status
                # We only need the first byte to confirm the server is alive and responding.
                status_data = s.recv(1)
                
                if not status_data:
                    raise Exception("Connection closed unexpectedly")
                
                # Note: We do not read the full response payload (file list) 
                # to minimize client-side processing overhead during load testing.

            # Calculate execution time in milliseconds
            total_time = int((time.time() - start_time) * 1000)
            
            # Report SUCCESS to Locust
            events.request.fire(
                request_type="TCP",
                name="search",
                response_time=total_time,
                response_length=0,
                exception=None,
            )

        except Exception as e:
            # Calculate time even in case of failure
            total_time = int((time.time() - start_time) * 1000)
            
            # Report FAILURE to Locust
            events.request.fire(
                request_type="TCP",
                name="search",
                response_time=total_time,
                response_length=0,
                exception=e,
            )

    @task
    def search_task(self):
        """
        Main task: picks a random word and sends a search request.
        """
        word = random.choice(SEARCH_WORDS)
        self.send_search_request(word)