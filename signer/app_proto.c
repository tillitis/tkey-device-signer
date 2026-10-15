// SPDX-FileCopyrightText: 2022 Tillitis AB <tillitis.se>
// SPDX-License-Identifier: BSD-2-Clause

#include <string.h>
#include <tkey/debug.h>

#include "app_proto.h"

// Send reply frame with response status Not OK (NOK==1), shortest length
void appreply_nok(struct frame_header hdr)
{
	uint8_t buf[1] = {0}; // Not used, but smallest payload is 1 byte

	frame_write(FRAME_STATUS_NOK, hdr.id, hdr.f_domain, buf, 1);
}

// Send app reply with frame header, response code, and LEN_X-1 bytes from buf
void appreply(struct frame_header hdr, enum appcmd rspcode, void *buf)
{
	size_t nbytes = 0; // Number of bytes in a reply frame
			   // (including rspcode).
	uint8_t frame[128]; // Longest response

	switch (rspcode) {
	case RSP_GET_PUBKEY:
		nbytes = 128;
		break;

	case RSP_SET_SIZE:
		nbytes = 4;
		break;

	case RSP_LOAD_DATA:
		nbytes = 4;
		break;

	case RSP_GET_SIG:
		nbytes = 128;
		break;

	case RSP_GET_NAMEVERSION:
		nbytes = 32;
		break;

	case RSP_GET_FIRMWARE_HASH:
		nbytes = 128;
		break;

	default:
		debug_puts("appreply(): Unknown response code: ");
		debug_puthex(rspcode);
		debug_puts("\n");

		return;
	}

	// App protocol header
	frame[0] = rspcode;

	// Copy payload after app protocol header
	memcpy(&frame[1], buf, nbytes - 1);

	frame_write(FRAME_STATUS_OK, hdr.id, hdr.f_domain, frame, nbytes);
}
