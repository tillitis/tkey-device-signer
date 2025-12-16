// SPDX-FileCopyrightText: 2022 Tillitis AB <tillitis.se>
// SPDX-License-Identifier: BSD-2-Clause

#include <stdint.h>
#include <string.h>
#include <tkey/debug.h>
#include <tkey/tk1_mem.h>

#include "app_proto.h"
#include "platform.h"

// Send reply frame with response status Not OK (NOK==1), shortest length
void appreply_nok(struct frame_header hdr)
{
	uint8_t buf = 0;
	write_frame(STATUS_BAD, hdr.id, hdr.endpoint, &buf, sizeof(buf));
}

// Send app reply with response code, and LEN_X-1 bytes from buf
void appreply(struct frame_header hdr, enum appcmd rspcode, void *buf)
{
	size_t nbytes = 0;  // Number of bytes in a reply frame
			    // (including rspcode).
	uint8_t frame[128]; // longest response

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

	frame[0] = rspcode;
	memcpy(&frame[1], buf, nbytes - 1);
	write_frame(STATUS_OK, hdr.id, hdr.endpoint, frame, nbytes);
}

// read_command takes a frame header and a command to fill in after
// parsing. It returns 0 on success.
int read_command(struct frame_header *hdr, uint8_t *cmd)
{
	memset(hdr, 0, sizeof(struct frame_header));
	memset(cmd, 0, CMDLEN_MAXBYTES);

	if (read_frame(cmd, CMDLEN_MAXBYTES, hdr)) {
		return -1;
	}

	// Well-behaved apps are supposed to check for a client
	// attempting to probe for firmware. In that case destination
	// is firmware and we just reply NOK, discarding all bytes
	// already read.
	if (hdr->endpoint == DST_FW) {
		appreply_nok(*hdr);
		debug_puts("Responded NOK to message meant for fw\n");
		cmd[0] = CMD_FW_PROBE;

		return 0;
	}

	// Is it for us? If not, return error after having discarded
	// all bytes.
	if (hdr->endpoint != DST_SW) {
		debug_puts("Message not meant for app. endpoint was 0x");
		debug_puthex(hdr->endpoint);
		debug_lf();

		return -1;
	}

	return 0;
}
