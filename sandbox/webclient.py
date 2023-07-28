import socket, argparse

# get CLI args
parser = argparse.ArgumentParser(
        prog='webclient',
        description='Receives data from host')

parser.add_argument('host')
parser.add_argument('-p', '--port', type=int)
args = vars(parser.parse_args())

host = args['host']
port = args['port'] if args['port'] else 80

# open socket
s = socket.socket()

# connect
s.connect((host, port))

print("Connecting to {}".format(socket.gethostbyname(host)))

# build request
request = "GET / HTTP/1.1\r\nHOST: {}\r\n\r\n".format(host)
request = request.encode("ISO-8859-1")
print(request)

# send request
s.sendall(request)

# receive response
d = s.recv(4096)
if len(d) == 0:
    print("nothing received")
else:
    print(d)


# close socket
s.close()
