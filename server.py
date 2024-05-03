import socket
import json
import matplotlib.pyplot as plt
HOST = '192.168.0.107' # IP address
PORT = 7777 # Port to listen on (use ports > 1023)

raw_data = []
filter_data = []
counter = 0
sample_num = 320


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print("Starting server at: ", (HOST, PORT))
    conn, addr = s.accept()
    with conn:
        print("Connected at", addr)
        while True:
            if not(counter < sample_num*2):
                break
            data = conn.recv(1024).decode('utf-8')
            print("Received from socket server:", data)
            if (data.count('{') != 1):
                # Incomplete data are received.
                choose = 0
                buffer_data = data.split('}')
                while buffer_data[choose][0] != '{':
                    choose += 1
                data = buffer_data[choose] + '}'
        
            obj = json.loads(data)
            
            if counter < sample_num:
                raw_data.append(obj['x'])
            elif counter >= sample_num and counter < sample_num*2:
                filter_data.append(obj['x'])
            print(f"progressing... {100*counter/(sample_num*2)}%")
            counter += 1
        index = range(1,sample_num+1)
        print("progressing... 100%")
        plt.subplot(2,1,1)
        plt.title("x_acceleration_raw_data")
        plt.plot(index, raw_data)
        plt.subplot(2,1,2)
        plt.title("x_acceleration_filtered_data")
        plt.plot(index, filter_data)
        plt.show()    