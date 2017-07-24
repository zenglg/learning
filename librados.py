#!/usr/bin/python

import rados, sys

with rados.Rados(conffile='/etc/ceph/ceph.conf') as cluster:
    print "\n\nI/O Context and Object Operations"
    print "====================================="
    with cluster.open_ioctx('data') as ioctx:
        print "\nWriting object 'hw' with contents 'Hello World!' to pool 'data'."
        ioctx.write("hw", "Hello World!")
        print "\nWriting XATTR 'lang' with value 'en_US' to object 'hw'"
        ioctx.set_xattr("hw", "lang", "en_US")

        print "\nWriting object 'bm' with contents 'Bonjour tout le monde!' to pool 'data'."
        ioctx.write("bm", "Bonjour tout le monde!")
        print "\nWriting XATTR 'lang' with value 'fr_FR' to object 'hw'"
        ioctx.set_xattr("hw", "lang", "fr_FR")

        print "\nContents of object 'hw'\n-----------------------------"
        print ioctx.read("hw")
        print "\n\nGetting XATTR 'lang' from object 'hw'"
        print ioctx.get_xattr("hw", "lang")

        print "\nContents of object 'bm'\n-----------------------------"
        print ioctx.read("bm")
