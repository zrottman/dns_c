
import socket

response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 6\r\nConnection: close\r\n\r\nHello!\r\n\r\n"
port = 28333

# open a socket
sock = socket.socket()

# bind to (address, port) -- localHost = "" ?
sock.bind(('', port))

#to do: loop to accept new request

# listen
sock.listen()

# continue accepting connections
while (new_sock := sock.accept()[0]):

    # receive request
    while (bytes_received := new_sock.recv(1000)):
        print("request: {}\n".format(bytes_received.decode("ISO-8859-1")))
        if bytes_received.decode("ISO-8859-1").endswith("\r\n\r\n"):
            break    
        
    # send response
    new_sock.send(response.encode("ISO-8859-1"))

    # close new_sock
    new_sock.close()

