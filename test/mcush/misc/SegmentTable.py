# coding: utf8
__doc__ = '7-segment displayer definations'
__author__ = 'Peng Shulin <trees_peng@163.com>'
__license__ = 'MCUSH designed by Peng Shulin, all rights reserved.'

#  
#      a
#    -----
#  f|     | b
#   |  g  |
#    ----- 
#  e|     | c
#   |     |
#    ----- .h
#      d
#  
SA = 0x01
SB = 0x02
SC = 0x04
SD = 0x08
SE = 0x10
SF = 0x20
SG = 0x40
SH = 0x80

SEGMENT_TABLE_HEX = [
    # 48 0x30 '0' 
    SA + SB + SC + SD + SE + SF,
    # 49 0x31 '1' 
    SB + SC,
    # 50 0x32 '2' 
    SA + SB + SG + SE + SD,
    # 51 0x33 '3' 
    SA + SB + SG + SC + SD,
    # 52 0x34 '4' 
    SF + SG + SB + SC,
    # 53 0x35 '5' 
    SA + SF + SG + SC + SD,
    # 54 0x36 '6' 
    SA + SF + SE + SD + SC + SG,
    # 55 0x37 '7' 
    SA + SB + SC,
    # 56 0x38 '8' 
    SA + SB + SC + SD + SE + SF + SG,
    # 57 0x39 '9' 
    SG + SF + SA + SB + SC + SD,
    # 65 0x41 'A' 
    SA + SB + SC + SE + SF + SG,
    # 66 0x42 'B' 
    SF + SE + SD + SC + SG,
    # 67 0x43 'C' 
    SA + SF + SE + SD,
    # 68 0x44 'D' 
    SG + SE + SD + SC + SB,
    # 69 0x45 'E' 
    SA + SF + SG + SE + SD,
    # 70 0x46 'F' 
    SA + SF + SG + SE,
]

SEGMENT_TABLE_ASCII = [
    # 0 0x00 
    0,
    # 1 0x01 
    0,
    # 2 0x02 
    0,
    # 3 0x03 
    0,
    # 4 0x04 
    0,
    # 5 0x05 
    0,
    # 6 0x06 
    0,
    # 7 0x07 
    0,
    # 8 0x08 
    0,
    # 9 0x09 
    0,
    # 10 0x0a 
    0,
    # 11 0x0b 
    0,
    # 12 0x0c 
    0,
    # 13 0x0d 
    0,
    # 14 0x0e 
    0,
    # 15 0x0f 
    0,
    # 16 0x10 
    0,
    # 17 0x11 
    0,
    # 18 0x12 
    0,
    0,
    # 20 0x14 
    0,
    # 21 0x15 
    0,
    # 22 0x16 
    0,
    # 23 0x17 
    0,
    # 24 0x18 
    0,
    # 25 0x19 
    0,
    # 26 0x1a 
    0,
    # 27 0x1b 
    0,
    # 28 0x1c 
    0,
    # 29 0x1d 
    0,
    # 30 0x1e 
    0,
    # 31 0x1f 
    0,
    # 32 0x20 ' ' 
    0,
    # 33 0x21 '!' 
    0,
    # 34 0x22 '"' 
    0,
    # 35 0x23 '#' 
    0,
    # 36 0x24 '$' 
    0,
    # 37 0x25 '%' 
    0,
    # 38 0x26 '&' 
    0,
    # 39 0x27 ''' 
    SB,
    # 40 0x28 '(' 
    SA + SF + SE + SD,
    # 41 0x29 ')' 
    SA + SB + SC + SD,
    # 42 0x2a '*' 
    0,
    # 43 0x2b '+' 
    0,
    # 44 0x2c ',' 
    SC + SD,
    # 45 0x2d '-' 
    SG,
    # 46 0x2e '.' 
    SH,
    # 47 0x2f '/' 
    SB + SG + SE,
    # 48 0x30 '0' 
    SA + SB + SC + SD + SE + SF,
    # 49 0x31 '1' 
    SB + SC,
    # 50 0x32 '2' 
    SA + SB + SG + SE + SD,
    # 51 0x33 '3' 
    SA + SB + SG + SC + SD,
    # 52 0x34 '4' 
    SF + SG + SB + SC,
    # 53 0x35 '5' 
    SA + SF + SG + SC + SD,
    # 54 0x36 '6' 
    SA + SF + SE + SD + SC + SG,
    # 55 0x37 '7' 
    SA + SB + SC,
    # 56 0x38 '8' 
    SA + SB + SC + SD + SE + SF + SG,
    # 57 0x39 '9' 
    SG + SF + SA + SB + SC + SD,
    # 58 0x3a ':' 
    0,
    # 59 0x3b ';' 
    0,
    # 60 0x3c '<' 
    SD + SE + SG,
    # 61 0x3d '=' 
    SG + SD,
    # 62 0x3e '>' 
    SC + SD + SG,
    # 63 0x3f '?' 
    0,
    # 64 0x40 '@' 
    0,
    # 65 0x41 'A' 
    SA + SB + SC + SE + SF + SG,
    # 66 0x42 'B' 
    SF + SE + SD + SC + SG,
    # 67 0x43 'C' 
    SA + SF + SE + SD,
    # 68 0x44 'D' 
    SG + SE + SD + SC + SB,
    # 69 0x45 'E' 
    SA + SF + SG + SE + SD,
    # 70 0x46 'F' 
    SA + SF + SG + SE,
    # 71 0x47 'G' 
    SA + SF + SE + SD + SC,
    # 72 0x48 'H' 
    SF + SE + SG + SB + SC,
    # 73 0x49 'I' 
    SB + SC,
    # 74 0x4a 'J' 
    SB + SC + SD,
    # 75 0x4b 'K' 
    0,
    # 76 0x4c 'L' 
    SF + SE + SD,
    # 77 0x4d 'M' 
    0,
    # 78 0x4e 'N' 
    SE + SF + SA + SB + SC,
    # 79 0x4f 'O' 
    SA + SB + SC + SD + SE + SF,
    # 80 0x50 'P' 
    SA + SB + SG + SF + SE,
    # 81 0x51 'Q' 
    0,
    # 82 0x52 'R' 
    SE + SG,
    # 83 0x53 'S' 
    SA + SF + SG + SC + SD,
    # 84 0x54 'T' 
    SA + SE + SF,
    # 85 0x55 'U' 
    SB + SC + SD + SE + SF,
    # 86 0x56 'V' 
    SB + SC + SD + SE + SF,
    # 87 0x57 'W' 
    0,
    # 88 0x58 'X' 
    0,
    # 89 0x59 'Y' 
    SF + SG + SB + SC + SD,
    # 90 0x5a 'Z' 
    0,
    # 91 0x5b '[' 
    SA + SF + SE + SD,
    # 92 0x5c '\' 
    SF + SG + SC,
    # 93 0x5d ']' 
    SA + SB + SC + SD,
    # 94 0x5e '^' 
    SB + SA + SF,
    # 95 0x5f '_' 
    SD,
    # 96 0x60 '`' 
    SF,
    # 97 0x61 'a' 
    SA + SB + SC + SE + SF + SG,
    # 98 0x62 'b' 
    SF + SE + SD + SC + SG,
    # 99 0x63 'c' 
    SG + SE + SD,
    # 100 0x64 'd' 
    SG + SE + SD + SC + SB,
    # 101 0x65 'e' 
    SA + SF + SG + SE + SD,
    # 102 0x66 'f' 
    SA + SF + SG + SE,
    # 103 0x67 'g' 
    SA + SF + SE + SD + SC,
    # 104 0x68 'h' 
    SF + SE + SG + SC,
    # 105 0x69 'i' 
    SB + SC,
    # 106 0x6a 'j' 
    SB + SC + SD,
    # 107 0x6b 'k' 
    0,
    # 108 0x6c 'l' 
    SF + SE + SD,
    # 109 0x6d 'm' 
    0,
    # 110 0x6e 'n' 
    SC + SE + SG,
    # 111 0x6f 'o' 
    SG + SE + SD + SC,
    # 112 0x70 'p' 
    SA + SB + SG + SF + SE,
    # 113 0x71 'q' 
    SA + SB + SG + SF + SC,
    # 114 0x72 'r' 
    SE + SG,
    # 115 0x73 's' 
    SA + SF + SG + SC + SD,
    # 116 0x74 't' 
    SF + SG + SE + SD,
    # 117 0x75 'u' 
    SE + SD + SC,
    # 118 0x76 'v' 
    SE + SD + SC,
    # 119 0x77 'w' 
    0,
    # 120 0x78 'x' 
    0,
    # 121 0x79 'y' 
    SB + SG + SF + SC + SD,
    # 122 0x7a 'z' 
    0,
    # 123 0x7b '{' 
    SE + SF + SG,
    # 124 0x7c '|' 
    SF + SE,
    # 125 0x7d '}' 
    SB + SC + SG,
    # 126 0x7e '~' 
    SA,
    # 127 0x7f DEL 
    0,
]


