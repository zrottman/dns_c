
import socket

response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 6\r\nConnection: close\r\n\r\nHello!\r\n\r\n"
port = 28333
#open a socket
sock = socket.socket()
#     (address, port), localHost = "" ?
sock.bind(('', port))
#to do: loop to accept new request
sock.listen()
new_sock, _ = sock.accept()
# bytes_recived = ""
while (bytes_recived := new_sock.recv(1000)):
    print("response: {}\n".format(bytes_recived.decode("ISO-8859-1")))
    if bytes_recived.decode("ISO-8859-1").endswith("\r\n\r\n"):
        break    
new_sock.send(response.encode("ISO-8859-1"))
new_sock.close()

