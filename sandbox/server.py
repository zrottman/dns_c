import socket, os, typing
from dataclasses import dataclass

@dataclass
class Response:
    header: bytes
    content_type: bytes
    connection: bytes
    payload: bytes
    #content_length: bytes = b"Content-Length: "

    def __post_init__(self):
        """ Compute content_length """
        self.content_length = bytes("Content-Lenght: {}".format(str(len(self.payload) + 4)), "ISO-8859-1")

    def encode(self) -> bytes:
        """ Encode class attributes as ISO-8859-1 """
        response = [
                self.header,
                self.content_type,
                self.content_length,
                self.connection,
                b"",
                self.payload,
                b"",
                b""
                ]

        return b"\r\n".join(response)

def build_response(filename: str) -> bytes:

    try:
        with open(filename, "rb") as fp:
            data = bytearray(fp.read())
    except:
        # file not found, send 404
        response = Response(
                header=b"HTTP/1.1 404 Not Found", 
                content_type=b"Content-Type: text/plain",
                connection=b"Connection: close",
                payload=b"404 not found",
                )
        return response.encode()

    # get file ext
    _, ext = os.path.splitext(filename)

    content_type = {
            ".txt" : "text/plain",
            ".html": "text/html",
            ".pdf" : "application/pdf",
            ".jpeg": "image/jpeg",
            ".jpg" : "image/jpeg",
            ".gif" : "image/gif",
            }

    # create response object
    response = Response( 
            header=b"HTTP/1.1 200 OK",
            content_type=bytes("Content-Type: {}".format(content_type.get(ext, "application/octet-stream")), "ISO-8859-1"),
            connection=b"Connection: close",
            payload=data
            )

    return response.encode()


def get_request(new_sock: socket.socket) -> str:

    request_chunks = []

    while (bytes_received := new_sock.recv(1000)):
        received = bytes_received.decode("ISO-8859-1")
        print("request: {}\n".format(received))
        request_chunks.append(received)
        if received.endswith("\r\n\r\n"):
            break    

    return ''.join(request_chunks)


def parse_request(r: str) -> str:

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
