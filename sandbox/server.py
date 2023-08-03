import socket, os


def build_response(filename):

    content_type = {
            "txt": "text/plain",
            "html": "text/html"
            }

    try:
        with open(filename) as fp:
            data = fp.read()
        datalength = len(data.encode("ISO-8859-1"))

    except:
        # file not found, send 404
        response = [
                "HTTP/1.1 404 Not Found", 
                "Content-Type: text/plain",
                "Content-Length: 13",
                "Connection: close",
                "",
                "404 not found",
                ""
                ]
        response = "\r\n".join(response)
        return response.encode("ISO-8859-1")

    _, ext = os.path.splitext(filename)

    response = [
            "HTTP/1.1 200 OK",
            "Content-Type: {}".format(content_type.get(ext, "DEFAULT")), # replace default val
            "Content-Length: {}".format(datalength),
            "Connection: close",
            "",
            data,
            ""
            ]

    response = "\r\n".join(response)

    return response.encode("ISO-8859-1")


if __name__ == "__main__":

    port = 28333

    # open a socket
    sock = socket.socket()

    # bind to (address, port) -- localHost = "" ?
    sock.bind(('', port))

    # listen
    sock.listen()

    # TODO: exit loop on key press
    # continue accepting connections
    '''
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
    '''

    new_sock = sock.accept()[0]

    request_chunks = []

    # receive request
    while (bytes_received := new_sock.recv(1000)):
        received = bytes_received.decode("ISO-8859-1")
        print("request: {}\n".format(received))
        request_chunks.append(received)
        if received.endswith("\r\n\r\n"):
            break    

    # consolidate request chunks and split lines
    lines = ''.join(request_chunks).split('\r\n')

    # get requested file
    path = lines[0].split(' ')[1]        # we only need path for now
    _, filename, _ = os.path.split(path) # we only want the filename

    # build response
    encoded_response = build_response(filename)

    # send response
    new_sock.send(encoded_response)

    # close new_sock
    new_sock.close()

    # close sock
    sock.close()


    print("Printing first line of request")
    print(lines[0])

    print("Requested path")
    print(filename)

