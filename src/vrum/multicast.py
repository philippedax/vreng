#
# $Id: multicast.py,v 1.17 1995/12/19 17:12:38 tardieu Exp $
#
# Samuel Tardieu - 1995
#

"""This module implements a simple interface with multicast sockets.
The basic class is:

  multicast(group, port [,ttl [, emission_port]])

ttl is 1 by default
If emission_port is specified, you cannot listen anymore on this socket
which will be used to send only. The exception `send_only_error' will be
raised if you try to receive data from a write-only socket.

Operations on a multicast object are sendto, send, set_ttl and recvfrom.
set_ttl may be called only once (don't call it if you have chosen the
ttl at initialization time).

You can choose whether you want to receive a copy of your own packets by
calling methods loop() (you will receive them, the default) or noloop().

Complex objects can be used as a generic data transmission service
using the pickle module.
"""

import socket, string, struct

send_only_error = 'multicast.send_only_error'

class multicast:
    
    def __init__(self, *params):
	if type(params[0]) == type({}):
	    import sd
	    group, port, id, ttl = sd.extract(params[0], params[1])
	    eport = None
	else:
	    group, port, ttl, eport = params + (None,) * (4 - len(params))
	self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	if eport:
	    self.sendonly = 1
	else:
	    self.sendonly = None
	    eport = port
	self.s.bind(('', eport))
	self.ttl = ttl
	if self.ttl:
	    self.set_ttl(ttl)
	self.enter_group(group, port)

    def enter_group(self, group, port):
	self.group, self.port = group, port
	bytes = [string.atoi (x) for x in string.split (group, '.')]
	grpaddr = apply (struct.pack, ['BBBB'] + bytes)
	mreq = grpaddr + struct.pack('l', socket.INADDR_ANY)
	self.s.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    def sendto(self, what, to): return self.s.sendto(what, to)
    
    def recvfrom(self, len):
	if self.sendonly: raise send_only_error
        return self.s.recvfrom(len)

    def recv(self, len):
        if self.sendonly: raise send_only_error
        return self.s.recv (len)
    
    def loop(self, opt = 1):
	o = struct.pack('b', opt)
	self.s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, o)

    def noloop(self):
	self.loop(0)

    def fileno(self): return self.s.fileno()
    
    def __del__(self):
	self.s.close()

    def set_ttl(self, ttl = 1):
	self.ttl = struct.pack('b', ttl)
	self.s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, self.ttl)

    def send(self, data):
	self.sendto(data, (self.group, self.port))

class complex(multicast):

    def send(self, data):
	import pickle
	multicast.send(self, pickle.dumps(data))

    def recvfrom(self, size):
	d, sender = multicast.recvfrom(self, size)
	try:
	    import pickle
	    return pickle.loads(d), sender
	except:
	    return d, sender
