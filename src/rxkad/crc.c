/*
 * Copyright (c) 1997 - 2000 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

/* RCSID("$Heimdal: crc.c,v 1.9 2000/08/03 01:45:14 assar Exp $"); */

#include <afsconfig.h>
#if defined(UKERNEL)
#include "../afs/param.h"
#else
#include <afs/param.h>
#endif


#if defined(UKERNEL)
#include "../afs/sysincludes.h"
#include "../afs/afsincludes.h"
#include "../afs/stds.h"
#include "../rx/xdr.h"
#include "../rx/rx.h"
#include "../des/des.h"
#include "../afs/lifetimes.h"
#include "../afs/rxkad.h"
#else /* defined(UKERNEL) */
#include <afs/stds.h>
#include <sys/types.h>
#ifdef AFS_NT40_ENV
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <string.h>
#include <rx/xdr.h>
#include <rx/rx.h>
#include <des.h>
#include "lifetimes.h"
#include "rxkad.h"
#endif /* defined(UKERNEL) */

static u_long table[256];

#define CRC_GEN 0xEDB88320L

void
_rxkad_crc_init_table(void)
{
    static int flag = 0;
    unsigned long crc, poly;
    int i, j;

    if (flag)
	return;
    poly = CRC_GEN;
    for (i = 0; i < 256; i++) {
	crc = i;
	for (j = 8; j > 0; j--) {
	    if (crc & 1) {
		crc = (crc >> 1) ^ poly;
	    } else {
		crc >>= 1;
	    }
	}
	table[i] = crc;
    }
    flag = 1;
}

afs_uint32
_rxkad_crc_update(const char *p, size_t len, afs_uint32 res)
{
    while (len--)
	res = table[(res ^ *p++) & 0xFF] ^ (res >> 8);
    return res & 0xFFFFFFFF;
}
