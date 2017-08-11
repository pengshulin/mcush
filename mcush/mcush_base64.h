/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __MCUSH_BASE64_H__
#define __MCUSH_BASE64_H__

/****************************************************************************
libb64: Base64 Encoding/Decoding Routines
======================================
Authors:
-------
Chris Venter    chris.venter@gmail.com  http://rocketpod.blogspot.com

Copyright-Only Dedication (based on United States law) 
or Public Domain Certification

The person or persons who have associated work with this document (the
"Dedicator" or "Certifier") hereby either (a) certifies that, to the best of
his knowledge, the work of authorship identified is in the public domain of the
country from which the work is published, or (b) hereby dedicates whatever
copyright the dedicators holds in the work of authorship identified below (the
"Work") to the public domain. A certifier, moreover, dedicates any copyright
interest he may have in the associated work, and for these purposes, is
described as a "dedicator" below.

A certifier has taken reasonable steps to verify the copyright status of this
work. Certifier recognizes that his good faith efforts may not shield him from
liability if in fact the work certified is not in the public domain.

Dedicator makes this dedication for the benefit of the public at large and to
the detriment of the Dedicator's heirs and successors. Dedicator intends this
dedication to be an overt act of relinquishment in perpetuity of all present
and future rights under copyright law, whether vested or contingent, in the
Work. Dedicator understands that such relinquishment of all rights includes
the relinquishment of all rights to enforce (by lawsuit or otherwise) those
copyrights in the Work.

Dedicator recognizes that, once placed in the public domain, the Work may be
freely reproduced, distributed, transmitted, used, modified, built upon, or
otherwise exploited by anyone for any purpose, commercial or non-commercial,
and in any way, including by methods that have not yet been invented or
conceived.
****************************************************************************/

/*
cdecode.h - c header for a base64 decoding algorithm
cencode.h - c header for a base64 encoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

typedef enum
{
    step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
    base64_decodestep step;
    char plainchar;
} base64_decodestate;

void base64_init_decodestate(base64_decodestate* state_in);

int base64_decode_value(char value_in);

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in);

typedef enum
{
    step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
    base64_encodestep step;
    char result;
    int stepcount;
} base64_encodestate;

void base64_init_encodestate(base64_encodestate* state_in);

char base64_encode_value(char value_in);

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in);

int base64_encode_blockend(char* code_out, base64_encodestate* state_in);


#endif
