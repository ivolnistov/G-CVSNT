/* zlib.c --- interface to the zlib compression library
   Ian Lance Taylor <ian@cygnus.com>

   This file is part of GNU CVS.

   GNU CVS is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.  */

/* The routines in this file are the interface between the CVS
   client/server support and the zlib compression library.  */

#include "cvs.h"
#include "buffer.h"

#include "zlib.h"

/* OS/2 doesn't have EIO.  FIXME: this whole notion of turning
   a different error into EIO strikes me as pretty dubious.  */
#if !defined (EIO)
#define EIO EBADPOS
#endif

/* The compression interface is built upon the buffer data structure.
   We provide a buffer type which compresses or decompresses the data
   which passes through it.  An input buffer decompresses the data
   read from an underlying buffer, and an output buffer compresses the
   data before writing it to an underlying buffer.  */

/* This structure is the closure field of the buffer.  */

struct compress_buffer
{
    /* The underlying buffer.  */
    struct buffer *buf;
    /* The compression information.  */
    z_stream zstr;
};

static void compress_error (int, int, z_stream *, const char *);
static int compress_buffer_input (void *, char *, int, int, int *);
static int compress_buffer_output (void *, const char *, int, int *);
static int compress_buffer_flush (void *);
static int compress_buffer_block (void *, int);
static int compress_buffer_shutdown_input (void *);
static int compress_buffer_shutdown_output (void *);

/* Report an error from one of the zlib functions.  */

static void compress_error (int status, int zstatus, z_stream *zstr, const char *msg)
{
    int hold_errno;
    const char *zmsg;
    char buf[100];

    hold_errno = errno;

    zmsg = zstr->msg;
    if (zmsg == NULL)
    {
        sprintf (buf, "error %d", zstatus);
	zmsg = buf;
    }

    error (status,
	   zstatus == Z_ERRNO ? hold_errno : 0,
	   "%s: %s", msg, zmsg);
}

/* Create a compression buffer.  */

struct buffer * compress_buffer_initialize (struct buffer *buf, int input, int level, void (*memory)(struct buffer *))
{
    struct compress_buffer *n;
    int zstatus;

    n = (struct compress_buffer *) xmalloc (sizeof *n);
    memset (n, 0, sizeof *n);

    n->buf = buf;

    if (input)
	zstatus = inflateInit (&n->zstr);
    else
	zstatus = deflateInit (&n->zstr, level);
    if (zstatus != Z_OK)
	compress_error (1, zstatus, &n->zstr, "compression initialization");

    /* There may already be data buffered on BUF.  For an output
       buffer, this is OK, because these routines will just use the
       buffer routines to append data to the (uncompressed) data
       already on BUF.  An input buffer expects to handle a single
       buffer_data of buffered input to be uncompressed, so that is OK
       provided there is only one buffer.  At present that is all
       there ever will be; if this changes, compress_buffer_input must
       be modified to handle multiple input buffers.  */
    assert (! input || buf->data == NULL || buf->data->next == NULL);

    return buf_initialize (input ? compress_buffer_input : NULL,
			   input ? NULL : compress_buffer_output,
			   input ? NULL : compress_buffer_flush,
			   compress_buffer_block,
			   (input
			    ? compress_buffer_shutdown_input
			    : compress_buffer_shutdown_output),
			   memory,
			   n);
}

/* Input data from a compression buffer.  */

static int compress_buffer_input (void *closure, char *data, int need, int size, int *got)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;
    struct buffer_data *bd;

    if (cb->buf->input == NULL)
		abort ();

    /* We use a single buffer_data structure to buffer up data which
       the z_stream structure won't use yet.  We can safely store this
       on cb->buf->data, because we never call the buffer routines on
       cb->buf; we only call the buffer input routine, since that
       gives us the semantics we want.  As noted in
       compress_buffer_initialize, the buffer_data structure may
       already exist, and hold data which was already read and
       buffered before the decompression began.  */
    bd = cb->buf->data;
    if (bd == NULL)
    {
	bd = ((struct buffer_data *) xmalloc (sizeof (struct buffer_data)));
	if (bd == NULL)
	    return -2;
	bd->text = (char *) xmalloc (BUFFER_DATA_SIZE);
	if (bd->text == NULL)
	{
	    xfree (bd);
	    return -2;
	}
	bd->bufp = bd->text;
	bd->size = 0;
	cb->buf->data = bd;
    }

    cb->zstr.avail_out = size;
    cb->zstr.next_out = (Bytef *) data;

    while (1)
    {
	int zstatus, sofar, status, nread;

	/* First try to inflate any data we already have buffered up.
	   This is useful even if we don't have any buffered data,
	   because there may be data buffered inside the z_stream
	   structure.  */

	cb->zstr.avail_in = bd->size;
	cb->zstr.next_in = (Bytef *) bd->bufp;

	do
	{
	    zstatus = inflate (&cb->zstr, Z_NO_FLUSH);
	    if (zstatus == Z_STREAM_END)
		break;
	    if (zstatus != Z_OK && zstatus != Z_BUF_ERROR)
	    {
		compress_error (0, zstatus, &cb->zstr, "inflate");
		return EIO;
	    }
	} while (cb->zstr.avail_in > 0
		 && cb->zstr.avail_out > 0);

	bd->size = cb->zstr.avail_in;
	bd->bufp = (char *) cb->zstr.next_in;

	/* If we have obtained NEED bytes, then return, unless NEED is
           zero and we haven't obtained anything at all.  If NEED is
           zero, we will keep reading from the underlying buffer until
           we either can't read anything, or we have managed to
           inflate at least one byte.  */
	sofar = size - cb->zstr.avail_out;
	if (sofar > 0 && sofar >= need)
	    break;

	if (zstatus == Z_STREAM_END)
	    return -1;

	/* All our buffered data should have been processed at this
           point.  */
	assert (bd->size == 0);

	/* This will work well in the server, because this call will
	   do an unblocked read and fetch all the available data.  In
	   the client, this will read a single byte from the stdio
	   stream, which will cause us to call inflate once per byte.
	   It would be more efficient if we could make a call which
	   would fetch all the available bytes, and at least one byte.  */

	status = (*cb->buf->input) (cb->buf->closure, bd->text,
				    need > 0 ? 1 : 0,
				    BUFFER_DATA_SIZE, &nread);
	if (status != 0)
	    return status;

	/* If we didn't read anything, then presumably the buffer is
           in nonblocking mode, and we should just get out now with
           whatever we've inflated.  */
	if (nread == 0)
	{
	    assert (need == 0);
	    break;
	}

	bd->bufp = bd->text;
	bd->size = nread;
    }

    *got = size - cb->zstr.avail_out;

    return 0;
}

/* Output data to a compression buffer.  */

static int compress_buffer_output (void *closure, const char *data, int have, int *wrote)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;

    cb->zstr.avail_in = have;
    cb->zstr.next_in = (unsigned char *) data;

    while (cb->zstr.avail_in > 0)
    {
	char buffer[BUFFER_DATA_SIZE];
	int zstatus;

	cb->zstr.avail_out = BUFFER_DATA_SIZE;
	cb->zstr.next_out = (unsigned char *) buffer;

	zstatus = deflate (&cb->zstr, Z_NO_FLUSH);
	if (zstatus != Z_OK)
	{
	    compress_error (0, zstatus, &cb->zstr, "deflate");
	    return EIO;
	}

	if (cb->zstr.avail_out != BUFFER_DATA_SIZE)
	    buf_output (cb->buf, buffer,
			BUFFER_DATA_SIZE - cb->zstr.avail_out);
    }

    *wrote = have;

    /* We will only be here because buf_send_output was called on the
       compression buffer.  That means that we should now call
       buf_send_output on the underlying buffer.  */
    return buf_send_output (cb->buf);
}

/* Flush a compression buffer.  */

static int compress_buffer_flush (void *closure)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;

    cb->zstr.avail_in = 0;
    cb->zstr.next_in = NULL;

    while (1)
    {
	char buffer[BUFFER_DATA_SIZE];
	int zstatus;

	cb->zstr.avail_out = BUFFER_DATA_SIZE;
	cb->zstr.next_out = (unsigned char *) buffer;

	zstatus = deflate (&cb->zstr, Z_SYNC_FLUSH);

	/* The deflate function will return Z_BUF_ERROR if it can't do
           anything, which in this case means that all data has been
           flushed.  */
	if (zstatus == Z_BUF_ERROR)
	    break;

	if (zstatus != Z_OK)
	{
	    compress_error (0, zstatus, &cb->zstr, "deflate flush");
	    return EIO;
	}

	if (cb->zstr.avail_out != BUFFER_DATA_SIZE)
	    buf_output (cb->buf, buffer,
			BUFFER_DATA_SIZE - cb->zstr.avail_out);

	/* If the deflate function did not fill the output buffer,
           then all data has been flushed.  */
	if (cb->zstr.avail_out > 0)
	    break;
    }

    /* Now flush the underlying buffer.  Note that if the original
       call to buf_flush passed 1 for the BLOCK argument, then the
       buffer will already have been set into blocking mode, so we
       should always pass 0 here.  */
    return buf_flush (cb->buf, 0);
}

/* The block routine for a compression buffer.  */

static int compress_buffer_block (void *closure, int block)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;

    if (block)
	return set_block (cb->buf);
    else
	return set_nonblock (cb->buf);
}

/* Shut down an input buffer.  */

static int compress_buffer_shutdown_input (void *closure)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;
    int zstatus;

    zstatus = inflateEnd (&cb->zstr);
    if (zstatus != Z_OK)
    {
	compress_error (0, zstatus, &cb->zstr, "inflateEnd");
	return EIO;
    }

    int status = buf_shutdown (cb->buf);cb->buf = NULL;
    return status;
}

/* Shut down an output buffer.  */

static int compress_buffer_shutdown_output (void *closure)
{
    struct compress_buffer *cb = (struct compress_buffer *) closure;
    int zstatus, status;

    do
    {
	char buffer[BUFFER_DATA_SIZE];

	cb->zstr.avail_out = BUFFER_DATA_SIZE;
	cb->zstr.next_out = (unsigned char *) buffer;

	zstatus = deflate (&cb->zstr, Z_FINISH);
	if (zstatus != Z_OK && zstatus != Z_STREAM_END)
	{
	    compress_error (0, zstatus, &cb->zstr, "deflate finish");
	    return EIO;
	}

	if (cb->zstr.avail_out != BUFFER_DATA_SIZE)
	    buf_output (cb->buf, buffer,
			BUFFER_DATA_SIZE - cb->zstr.avail_out);
    } while (zstatus != Z_STREAM_END);

    zstatus = deflateEnd (&cb->zstr);
    if (zstatus != Z_OK)
    {
	compress_error (0, zstatus, &cb->zstr, "deflateEnd");
	return EIO;
    }

    status = buf_flush (cb->buf, 1);
    if (status != 0)
	return status;

    status = buf_shutdown (cb->buf);cb->buf = NULL;
    return status;
}




