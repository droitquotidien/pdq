# -*- coding: utf-8 -*-
"""
cut -d ';' -f 8 uris_legi | sort | uniq > nums_legi
python snums.py nums_legi 2> unmatched_legi
cut -d ';' -f 8 uris_jorf | sort | uniq > nums_jorf
python snums.py nums_jorf 2> unmatched_jorf
"""
import re
import sys
from collections import Counter
from pprint import pprint

links = Counter()
link_re = re.compile("warning:(?P<id>[^:]+): too much LIEN")

with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        one_match = False
        m = link_re.match(line)
        if m is not None:
            one_match = True
            links[m.group('id')] += 1

max_links = max(links.values())
pprint(max_links+12)

