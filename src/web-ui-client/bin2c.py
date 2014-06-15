#!/usr/bin/env python

#
# Filename: bin2c.py
#
# Author: follower@rancidbacon.com // October 2008
#
# Take a binary file and output it as an array of bytes suitable for
# including in a C/C++ program.
#
# Example usage: ./bin2c.py somefile.swf > content.h
#

import sys

if __name__ == "__main__":
    try:
        filename = sys.argv[1]
        varname = sys.argv[2]
    except IndexError:
        print "Usage: %s <filename> <varname>" % sys.argv[0]
        raise SystemExit

    contentFile = open(filename, "rb");

    print '/* This is automatically generated file! Do not edit it!'
    print '*/'
    print '#pragma once'
    print "static const unsigned char %s[] = {" % (varname,),

    while True:
        byte = contentFile.read(1)

        if not byte:
            break

        print "0x%02x," % ord(byte),

    print "};"
