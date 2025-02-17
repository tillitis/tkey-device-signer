// SPDX-FileCopyrightText: 2022 Tillitis AB <tillitis.se>
// SPDX-License-Identifier: BSD-2-Clause

#include <tkey/debug.h>

#include "app_proto.h"

// Send reply frame with response status Not OK (NOK==1), shortest length
void appreply_nok(struct frame_header hdr)
{
	uint8_t buf[2];

	buf[0] = genhdr(hdr.id, hdr.endpoint, 0x1, LEN_1);
	buf[1] = 0; // Not used, but smallest payload is 1 byte
	write(IO_CDC, buf, 2);
}

// Send app reply with frame header, response code, and LEN_X-1 bytes from buf
void appreply(struct frame_header hdr, enum appcmd rspcode, void *buf)
{
	size_t nbytes = 0; // Number of bytes in a reply frame
			   // (including rspcode).
	enum cmdlen len = LEN_1;
	uint8_t frame[1 + 128]; // Frame header + longest response

	switch (rspcode) {
	case RSP_GET_PUBKEY:
		len = LEN_128;
		nbytes = 128;
		break;

	case RSP_SET_SIZE:
		len = LEN_4;
		nbytes = 4;
		break;

	case RSP_LOAD_DATA:
		len = LEN_4;
		nbytes = 4;
		break;

	case RSP_GET_SIG:
		len = LEN_128;
		nbytes = 128;
		break;

	case RSP_GET_NAMEVERSION:
		len = LEN_32;
		nbytes = 32;
		break;

	case RSP_GET_FIRMWARE_HASH:
		len = LEN_128;
		nbytes = 128;
		break;

	default:
		debug_puts("appreply(): Unknown response code: ");
		debug_puthex(rspcode);
		debug_puts("\n");

		return;
	}

	// Frame Protocol Header
	frame[0] = genhdr(hdr.id, hdr.endpoint, 0x0, len);
	// App protocol header
	frame[1] = rspcode;

	// Copy payload after app protocol header
	memcpy(&frame[2], buf, nbytes - 1);

	write(IO_CDC, frame, 1 + nbytes);
}
