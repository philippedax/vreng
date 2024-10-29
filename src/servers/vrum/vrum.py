#! /usr/bin/python
#
# vrum.py
#
# Vreng Reflector Unicast Multicast
#
# Samuel Tardieu - Mar 2003
#
verbose = 1
uport_offset = 1000
maddr_base = '224.255.0.0'
mport_base = 52656
mport_incr = 10
ttl = 127
timeout = 60
MTU = 1500

import multicast, select, socket, thread, time


def launch (world, proto, maddr, mport, uport = None):
    if not uport: uport = mport - uport_offset
    if verbose:
         print "%s(%s) vrum starting (%s/%d/%d), to=%d, uport=%d" % \
               (world, proto, maddr, mport, ttl, timeout, uport)

    ms = multicast.multicast (maddr, mport, ttl)
    ms.loop (0)
    us = socket.socket (socket.AF_INET, socket.SOCK_DGRAM)
    us.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    us.bind (('', uport))
    subscribers = {}

    while 1:
        ready, _, _ = select.select ([ms, us], [], [])
        if ms in ready:
            # if verbose: print "%s(%s): multicast packet" % (world, proto)
            pkt = ms.recv (MTU)
            for i in subscribers.keys():
                us.sendto (pkt, i)

        if us in ready:
            # if verbose: print "%s(%s): unicast packet" % (world, proto)
            pkt, sender = us.recvfrom (MTU)
            subscribers[sender] = time.time()
            ms.sendto (pkt, (maddr, mport))
            for i in subscribers.keys():
                if i != sender: us.sendto (pkt, i)

        for s in subscribers.keys():
            if time.time() - subscribers[s] > timeout:
                del subscribers[s]

        if verbose: print "%s(%s): %d unicast subscribers" % \
           (world, proto, len (subscribers.keys()))

def rtp (world, maddr, mport, uport = None):
    thread.start_new_thread (launch, (world, "rtp", maddr, mport, uport))
    thread.start_new_thread (launch, (world, "rtcp", maddr, mport+1, uport))

def udp (world, maddr, mport, uport = None):
    thread.start_new_thread (launch, (world, "udp", maddr, mport, uport))

# Launch repeaters
rtp ("Manager",      '224.255.0.0',  52656)
rtp ("Rendezvous",   '224.255.0.1',  52666)
rtp ("students",     '224.255.0.2',  52676)
rtp ("eric",         '224.255.0.3',  52686)
rtp ("infres",       '224.255.0.4',  52696)
rtp ("checker",      '224.255.0.5',  52706)
rtp ("tarots",       '224.255.0.6',  52716)
rtp ("telecomparis", '224.255.0.7',  52726)
rtp ("stadium",      '224.255.0.8',  52736)
rtp ("pyramid",      '224.255.0.9',  52746)
rtp ("maze",         '224.255.0.10', 52756)
rtp ("dax",          '224.255.0.11', 52766)
rtp ("danzart",      '224.255.0.12', 52776)
rtp ("felon",        '224.255.0.13', 52786)
rtp ("mouret",       '224.255.0.14', 52796)
rtp ("mythology",    '224.255.0.15', 52806)
rtp ("classroom",    '224.255.0.16', 52816)
rtp ("house",        '224.255.0.17', 52826)
rtp ("labyrinthe",   '224.255.0.18', 52836)
rtp ("temple",       '224.255.0.19', 52846)
rtp ("cubes",        '224.255.0.20', 52856)
rtp ("forest",       '224.255.0.21', 52866)
rtp ("tests",        '224.255.0.22', 52876)
rtp ("beach",        '224.255.0.23', 52886)
rtp ("movies",       '224.255.0.24', 52896)
rtp ("meeting",      '224.255.0.25', 52906)
rtp ("desktops",     '224.255.0.26', 52916)
rtp ("entree",       '224.255.0.27', 52926)
rtp ("fan",          '224.255.0.28', 52936)
rtp ("ring",         '224.255.0.29', 52946)
rtp ("board",        '224.255.0.30', 52956)
rtp ("guide",        '224.255.0.31', 52966)
rtp ("romain",       '224.255.0.32', 52976)
rtp ("land",         '224.255.0.33', 52986)
rtp ("factory",      '224.255.0.34', 52996)
rtp ("forest2",      '224.255.0.35', 53006)
#rtp ("enstnet",      '224.255.0.36', 53016)
#rtp ("mondetest",    '224.255.0.37', 53026)
#rtp ("campus",       '224.255.0.38', 53036)
#
#udp ("android-man",  '224.255.0.25', 52918)
#udp ("android-girl", '224.255.0.25', 52920)
#udp ("android-kang", '224.255.0.25', 52922)
#udp ("android-dog",  '224.255.0.25', 52924)
#udp ("android-wolf", '224.255.0.25', 52926)

# Wait forever
select.select ([], [], [])
