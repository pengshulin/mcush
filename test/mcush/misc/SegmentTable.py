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


def generateSegmentTableHEX(sa=SA,sb=SB,sc=SC,sd=SD,se=SE,sf=SF,sg=SG,sh=SH):
    table = [
        # 48 0x30 '0' 
        sa + sb + sc + sd + se + sf,
        # 49 0x31 '1' 
        sb + sc,
        # 50 0x32 '2' 
        sa + sb + sg + se + sd,
        # 51 0x33 '3' 
        sa + sb + sg + sc + sd,
        # 52 0x34 '4' 
        sf + sg + sb + sc,
        # 53 0x35 '5' 
        sa + sf + sg + sc + sd,
        # 54 0x36 '6' 
        sa + sf + se + sd + sc + sg,
        # 55 0x37 '7' 
        sa + sb + sc,
        # 56 0x38 '8' 
        sa + sb + sc + sd + se + sf + sg,
        # 57 0x39 '9' 
        sg + sf + sa + sb + sc + sd,
        # 65 0x41 'A' 
        sa + sb + sc + se + sf + sg,
        # 66 0x42 'B' 
        sf + se + sd + sc + sg,
        # 67 0x43 'C' 
        sa + sf + se + sd,
        # 68 0x44 'D' 
        sg + se + sd + sc + sb,
        # 69 0x45 'E' 
        sa + sf + sg + se + sd,
        # 70 0x46 'F' 
        sa + sf + sg + se, ]
    return table


def generateSegmentTableASCII(sa=SA,sb=SB,sc=SC,sd=SD,se=SE,sf=SF,sg=SG,sh=SH):
    table = [
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
        sb,
        # 40 0x28 '(' 
        sa + sf + se + sd,
        # 41 0x29 ')' 
        sa + sb + sc + sd,
        # 42 0x2a '*' 
        0,
        # 43 0x2b '+' 
        0,
        # 44 0x2c ',' 
        sc + sd,
        # 45 0x2d '-' 
        sg,
        # 46 0x2e '.' 
        sh,
        # 47 0x2f '/' 
        sb + sg + se,
        # 48 0x30 '0' 
        sa + sb + sc + sd + se + sf,
        # 49 0x31 '1' 
        sb + sc,
        # 50 0x32 '2' 
        sa + sb + sg + se + sd,
        # 51 0x33 '3' 
        sa + sb + sg + sc + sd,
        # 52 0x34 '4' 
        sf + sg + sb + sc,
        # 53 0x35 '5' 
        sa + sf + sg + sc + sd,
        # 54 0x36 '6' 
        sa + sf + se + sd + sc + sg,
        # 55 0x37 '7' 
        sa + sb + sc,
        # 56 0x38 '8' 
        sa + sb + sc + sd + se + sf + sg,
        # 57 0x39 '9' 
        sg + sf + sa + sb + sc + sd,
        # 58 0x3a ':' 
        0,
        # 59 0x3b ';' 
        0,
        # 60 0x3c '<' 
        sd + se + sg,
        # 61 0x3d '=' 
        sg + sd,
        # 62 0x3e '>' 
        sc + sd + sg,
        # 63 0x3f '?' 
        0,
        # 64 0x40 '@' 
        0,
        # 65 0x41 'A' 
        sa + sb + sc + se + sf + sg,
        # 66 0x42 'B' 
        sf + se + sd + sc + sg,
        # 67 0x43 'C' 
        sa + sf + se + sd,
        # 68 0x44 'D' 
        sg + se + sd + sc + sb,
        # 69 0x45 'E' 
        sa + sf + sg + se + sd,
        # 70 0x46 'F' 
        sa + sf + sg + se,
        # 71 0x47 'G' 
        sa + sf + se + sd + sc,
        # 72 0x48 'H' 
        sf + se + sg + sb + sc,
        # 73 0x49 'I' 
        sb + sc,
        # 74 0x4a 'J' 
        sb + sc + sd,
        # 75 0x4b 'K' 
        0,
        # 76 0x4c 'L' 
        sf + se + sd,
        # 77 0x4d 'M' 
        0,
        # 78 0x4e 'N' 
        se + sf + sa + sb + sc,
        # 79 0x4f 'O' 
        sa + sb + sc + sd + se + sf,
        # 80 0x50 'P' 
        sa + sb + sg + sf + se,
        # 81 0x51 'Q' 
        0,
        # 82 0x52 'R' 
        se + sg,
        # 83 0x53 'S' 
        sa + sf + sg + sc + sd,
        # 84 0x54 'T' 
        sa + se + sf,
        # 85 0x55 'U' 
        sb + sc + sd + se + sf,
        # 86 0x56 'V' 
        sb + sc + sd + se + sf,
        # 87 0x57 'W' 
        0,
        # 88 0x58 'X' 
        0,
        # 89 0x59 'Y' 
        sf + sg + sb + sc + sd,
        # 90 0x5a 'Z' 
        0,
        # 91 0x5b '[' 
        sa + sf + se + sd,
        # 92 0x5c '\' 
        sf + sg + sc,
        # 93 0x5d ']' 
        sa + sb + sc + sd,
        # 94 0x5e '^' 
        sb + sa + sf,
        # 95 0x5f '_' 
        sd,
        # 96 0x60 '`' 
        sf,
        # 97 0x61 'a' 
        sa + sb + sc + se + sf + sg,
        # 98 0x62 'b' 
        sf + se + sd + sc + sg,
        # 99 0x63 'c' 
        sg + se + sd,
        # 100 0x64 'd' 
        sg + se + sd + sc + sb,
        # 101 0x65 'e' 
        sa + sf + sg + se + sd,
        # 102 0x66 'f' 
        sa + sf + sg + se,
        # 103 0x67 'g' 
        sa + sf + se + sd + sc,
        # 104 0x68 'h' 
        sf + se + sg + sc,
        # 105 0x69 'i' 
        sb + sc,
        # 106 0x6a 'j' 
        sb + sc + sd,
        # 107 0x6b 'k' 
        0,
        # 108 0x6c 'l' 
        sf + se + sd,
        # 109 0x6d 'm' 
        0,
        # 110 0x6e 'n' 
        sc + se + sg,
        # 111 0x6f 'o' 
        sg + se + sd + sc,
        # 112 0x70 'p' 
        sa + sb + sg + sf + se,
        # 113 0x71 'q' 
        sa + sb + sg + sf + sc,
        # 114 0x72 'r' 
        se + sg,
        # 115 0x73 's' 
        sa + sf + sg + sc + sd,
        # 116 0x74 't' 
        sf + sg + se + sd,
        # 117 0x75 'u' 
        se + sd + sc,
        # 118 0x76 'v' 
        se + sd + sc,
        # 119 0x77 'w' 
        0,
        # 120 0x78 'x' 
        0,
        # 121 0x79 'y' 
        sb + sg + sf + sc + sd,
        # 122 0x7a 'z' 
        0,
        # 123 0x7b '{' 
        se + sf + sg,
        # 124 0x7c '|' 
        sf + se,
        # 125 0x7d '}' 
        sb + sc + sg,
        # 126 0x7e '~' 
        sa,
        # 127 0x7f DEL 
        0, ]
    return table


SEGMENT_TABLE_HEX = generateSegmentTableHEX()
SEGMENT_TABLE_ASCII = generateSegmentTableASCII()

