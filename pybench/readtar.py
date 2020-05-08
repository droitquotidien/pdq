#! /usr/bin/env python3
import tarfile
import sys

with tarfile.open(name=sys.argv[1], mode='r:gz') as tfile:
    for tarinfo in tfile:
        filepath = tarinfo.name
        print(filepath)

