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

c = Counter()
text_num = re.compile("\d{2,4}-\d{1,4}")
text_num_dc = re.compile("\d{2,4}-\d{1,4}\s(DC|L|QPC|ORGA|AN|PDR)")
text_num_letter = re.compile("\d{2,4}-[A-Z]+-\d{1,4}")
art_num = re.compile("\*{0,2}(L|R|A|D|LO)?\*{0,2}\d{1,4}(-\d{1,3})?(-\d{1,3})?(-\d{1,3})?")
art_bis_num = re.compile(
    "\*{0,2}\d{1,4}(-\d{1,2})?(\s[A-Z]{1,3})?"
    "(\s(bis|ter|quater|quinquies|sexies|septies|octies|nonies"
    "|decies|undecies|duodecies|terdecies|quaterdecies|quindecies|sedecies|sexdecies|septdecies|octodecies|novodecies"
    "|vicies|unvicies|duovicies|tervicies|quatervicies|quinvicies|sexvicies|septvicies|octovicies|novovicies"
    "|tricies|untricies|duotricies|tertricies|quatertricies|quintricies|sextricies|septtricies"
    "|quinquedecies"
    ")(-\d)?)?"
    "(\s[A-Z]{1,2})?(\s(bis|ter|quater|quinquies|sexies|septies|octies|nonies|decies))?",
    re.IGNORECASE
)

with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        one_match = False
        m = text_num.fullmatch(line)
        if m is not None:
            one_match = True
            c["text_num"] += 1
        m = text_num_dc.fullmatch(line)
        if m is not None:
            one_match = True
            c["text_num_dc"] += 1
        m = text_num_letter.fullmatch(line)
        if m is not None:
            one_match = True
            c["text_num_letter"] += 1
        m = art_num.fullmatch(line)
        if m is not None:
            one_match = True
            c["art_num"] += 1
        m = art_bis_num.fullmatch(line)
        if m is not None:
            one_match = True
            c["art_bis_num"] += 1
        if not one_match:
            c["excluded"] += 1
            print(line, file=sys.stderr)
        c["total"] += 1

pprint(c)

