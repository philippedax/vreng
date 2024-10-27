#!/bin/python
#
# vrum.py
#
# Vreng Reflector Unicast Multicast
#
# Samuel Tardieu - Mar 2003
#

MTU = 1500

verbose = 1
uport_offset = 1000
maddr_base = '224.255.0.0'
mport_base = 52656
mport_incr = 10
ttl = 127
timeout = 60


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

def launchrtp (world, maddr, mport, uport = None):
    thread.start_new_thread (launch, (world, "rtp", maddr, mport, uport))
    thread.start_new_thread (launch, (world, "rtcp", maddr, mport+1, uport))

def launchudp (world, maddr, mport, uport = None):
    thread.start_new_thread (launch, (world, "udp", maddr, mport, uport))

# Launch repeaters

launchrtp ("Manager",      '224.255.0.0',  52656)
launchrtp ("Rendezvous",   '224.255.0.1',  52666)
launchrtp ("students",     '224.255.0.2',  52676)
launchrtp ("eric",         '224.255.0.3',  52686)
launchrtp ("infres",       '224.255.0.4',  52696)
launchrtp ("checker",      '224.255.0.5',  52706)
launchrtp ("tarots",       '224.255.0.6',  52716)
launchrtp ("telecomparis", '224.255.0.7',  52726)
launchrtp ("stadium",      '224.255.0.8',  52736)
launchrtp ("pyramid",      '224.255.0.9',  52746)
launchrtp ("maze",         '224.255.0.10', 52756)
launchrtp ("dax",          '224.255.0.11', 52766)
launchrtp ("danzart",      '224.255.0.12', 52776)
launchrtp ("felon",        '224.255.0.13', 52786)
launchrtp ("mouret",       '224.255.0.14', 52796)
launchrtp ("mythology",    '224.255.0.15', 52806)
launchrtp ("classroom",    '224.255.0.16', 52816)
launchrtp ("house",        '224.255.0.17', 52826)
launchrtp ("labyrinthe",   '224.255.0.18', 52836)
launchrtp ("temple",       '224.255.0.19', 52846)
launchrtp ("cubes",        '224.255.0.20', 52856)
launchrtp ("forest",       '224.255.0.21', 52866)
launchrtp ("tests",        '224.255.0.22', 52876)
launchrtp ("beach",        '224.255.0.23', 52886)
launchrtp ("movies",       '224.255.0.24', 52896)
launchrtp ("meeting",      '224.255.0.25', 52906)
launchrtp ("desktops",     '224.255.0.26', 52916)
launchrtp ("entree",       '224.255.0.27', 52926)
launchrtp ("fan",          '224.255.0.28', 52936)
launchrtp ("ring",         '224.255.0.29', 52946)
launchrtp ("board",        '224.255.0.30', 52956)
launchrtp ("guide",        '224.255.0.31', 52966)
launchrtp ("romain",       '224.255.0.32', 52976)
launchrtp ("land",         '224.255.0.33', 52986)
launchrtp ("factory",      '224.255.0.34', 52996)
launchrtp ("forest2",      '224.255.0.35', 53006)
launchrtp ("enstnet",      '224.255.0.36', 53016)
launchrtp ("mondetest",    '224.255.0.37', 53026)
launchrtp ("campus",       '224.255.0.38', 53036)

launchudp ("android-man",  '224.255.0.25', 52918)
launchudp ("android-girl", '224.255.0.25', 52920)
launchudp ("android-kang", '224.255.0.25', 52922)
launchudp ("android-dog",  '224.255.0.25', 52924)
launchudp ("android-wolf", '224.255.0.25', 52926)

# Wait forever

select.select ([], [], [])
