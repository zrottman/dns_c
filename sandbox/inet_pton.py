'''
Toy Python implementation of C's inet_pton()
'''

def inet_pton(ip):
    '''
    Convert IPv4 IP to 32-bit integer
    '''
    # split `ip` at dot, convert each int to 8-bit bytestring, and concatenate
    binstr = "0b" + "".join(["{:08b}".format(int(int8)) for int8 in ip.split('.')])
    # convert 32-bit bytestring to int
    int32 = int(binstr, 2)

    return binstr, int32

def display_pton(ip="8.8.8.8"):
    '''
    Pretty-print results of inet_pton()
    '''
    binstr, int32 = inet_pton(ip)
    print("{:<15} -> {} ({})".format(ip, binstr, int32))


if __name__ == '__main__':

    display_pton("8.8.8.8")
    display_pton("31.13.70.36")
    display_pton("172.217.14.78")
