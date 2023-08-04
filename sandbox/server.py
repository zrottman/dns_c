import socket, os
from dataclasses import dataclass

@dataclass
class Response:
    header: str
    content_type: str
    content_length: str
    connection: str
    payload: str

    def encode(self) -> bytes:
        to_encode = [
                self.header,
                self.content_type,
                self.content_length,
                self.connection,
                "",
                self.payload,
                ""
                ]
        return "\r\n".join(to_encode).encode("ISO-8859-1")
    

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
        response = Response(
                header="HTTP/1.1 404 Not Found", 
                content_type="Content-Type: text/plain",
                content_length="Content-Length: 13",
                connection="Connection: close",
                payload="404 not found",
                )
        return response.encode()

    _, ext = os.path.splitext(filename)

    response = Response( 
            header="HTTP/1.1 200 OK",
            content_type="Content-Type: {}".format(content_type.get(ext, "DEFAULT")), # replace default val
            content_length="Content-Length: {}".format(datalength),
            connection="Connection: close",
            payload=data
            )

    return response.encode()


def get_request(new_sock):

    request_chunks = []

    while (bytes_received := new_sock.recv(1000)):
        received = bytes_received.decode("ISO-8859-1")
        print("request: {}\n".format(received))
        request_chunks.append(received)
        if received.endswith("\r\n\r\n"):
            break    

    return ''.join(request_chunks)


def parse_request(r):

    # split lines
    lines = r.split('\r\n')

    # get requested file
    path = lines[0].split(' ')[1]        # we only need path for now
    filename = os.path.split(path)[1]    # we only want the filename

    return filename


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

    # connect to client
    new_sock = sock.accept()[0]

    # receive request from client
    request = get_request(new_sock)

    # parse client request
    filename = parse_request(request)

    # build response
    encoded_response = build_response(filename)

    # send response to client
    new_sock.send(encoded_response)

    # close new_sock
    new_sock.close()

    # close sock
    sock.close()
