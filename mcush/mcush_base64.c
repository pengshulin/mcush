/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush_base64.h"
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


/****************************************************************************/
/*
cencoder.c - c source to a base64 encoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

//#include <b64/cencode.h>
//#include "cencode.h"

static const int CHARS_PER_LINE = 76;

void base64_init_encodestate(base64_encodestate* state_in)
{
    state_in->step = step_A;
    state_in->result = 0;
    state_in->stepcount = 0;
}

    
char base64_encode_value(char value_in)
{
    static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (value_in > 63)
        return '=';
    else
        return encoding[(int)value_in];
}

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
{
    const char* plainchar = plaintext_in;
    const char* const plaintextend = plaintext_in + length_in;
    char *codechar = code_out;
    char result;
    char fragment;
    
    result = state_in->result;
    
    switch (state_in->step)
    {
        while (1)
        {
    case step_A:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_A;
                return codechar - code_out;
            }
            fragment = *plainchar++;
            result = (fragment & 0x0fcu) >> 2;
            *codechar++ = base64_encode_value(result);
            result = (fragment & 0x003u) << 4;
    case step_B:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_B;
                return codechar - code_out;
            }
            fragment = *plainchar++;
            result |= (fragment & 0x0f0u) >> 4;
            *codechar++ = base64_encode_value(result);
            result = (fragment & 0x00fu) << 2;
    case step_C:
            if (plainchar == plaintextend)
            {
                state_in->result = result;
                state_in->step = step_C;
                return codechar - code_out;
            }
            fragment = *plainchar++;
            result |= (fragment & 0x0c0u) >> 6;
            *codechar++ = base64_encode_value(result);
            result  = (fragment & 0x03fu) >> 0;
            *codechar++ = base64_encode_value(result);
            
            ++(state_in->stepcount);
            if (state_in->stepcount == CHARS_PER_LINE/4)
            {
                *codechar++ = '\n';
                state_in->stepcount = 0;
            }
        }
    }
    /* control should not reach here */
    return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
{
    char* codechar = code_out;
    
    switch (state_in->step)
    {
    case step_B:
        *codechar++ = base64_encode_value(state_in->result);
        *codechar++ = '=';
        *codechar++ = '=';
        break;
    case step_C:
        *codechar++ = base64_encode_value(state_in->result);
        *codechar++ = '=';
        break;
    case step_A:
        break;
    }
    *codechar++ = '\n';
    
    return codechar - code_out;
}


/****************************************************************************/
/*
cdecoder.c - c source to a base64 decoding algorithm implementation

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

//#include <b64/cdecode.h>
//#include "cdecode.h"


int base64_decode_value(char value_in)
{
    static const signed char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
    static const char decoding_size = sizeof(decoding);
    if (value_in < 43)
        return -1;
    value_in -= 43;
    if (value_in > decoding_size)
        return -1;
    return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
    state_in->step = step_a;
    state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
    const char* codechar = code_in;
    char* plainchar = plaintext_out;
    int fragment;
    
    *plainchar = state_in->plainchar;
    
    switch (state_in->step)
    {
        while (1)
        {
    case step_a:
            do {
                if (codechar == code_in+length_in)
                {
                    state_in->step = step_a;
                    state_in->plainchar = *plainchar;
                    return plainchar - plaintext_out;
                }
                fragment = base64_decode_value(*codechar++);
            } while (fragment < 0);
            *plainchar    = (fragment & 0x03fu) << 2;
    case step_b:
            do {
                if (codechar == code_in+length_in)
                {
                    state_in->step = step_b;
                    state_in->plainchar = *plainchar;
                    return plainchar - plaintext_out;
                }
                fragment = base64_decode_value(*codechar++);
            } while (fragment < 0);
            *plainchar++ |= (fragment & 0x030u) >> 4;
            *plainchar    = (fragment & 0x00fu) << 4;
    case step_c:
            do {
                if (codechar == code_in+length_in)
                {
                    state_in->step = step_c;
                    state_in->plainchar = *plainchar;
                    return plainchar - plaintext_out;
                }
                fragment = base64_decode_value(*codechar++);
            } while (fragment < 0);
            *plainchar++ |= (fragment & 0x03cu) >> 2;
            *plainchar    = (fragment & 0x003u) << 6;
    case step_d:
            do {
                if (codechar == code_in+length_in)
                {
                    state_in->step = step_d;
                    state_in->plainchar = *plainchar;
                    return plainchar - plaintext_out;
                }
                fragment = base64_decode_value(*codechar++);
            } while (fragment < 0);
            *plainchar++   |= (fragment & 0x03f);
        }
    }
    /* control should not reach here */
    return plainchar - plaintext_out;
}





