import socket

response = [
        "HTTP/1.1 200 OK",
        "Content-Type: text/plain",
        "Content-Length: 10",
        "Connection: close",
        "",
        "Hello!",
        ""
        ]

response = "\r\n".join(response)

port = 28333

# open a socket
sock = socket.socket()

# bind to (address, port) -- localHost = "" ?
sock.bind(('', port))

# listen
sock.listen()

# TODO: exit loop on key press
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

sock.close()
