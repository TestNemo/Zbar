/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#include <config.h>
#include <stdlib.h>     /* malloc, calloc, free */
#include <stdio.h>      /* snprintf */
#include <string.h>     /* memset, strlen */

#include <zbar.h>
#include "decoder.h"

#if defined(DEBUG_DECODER) || defined(DEBUG_EAN) ||             \
    defined(DEBUG_CODE39) || defined(DEBUG_I25) ||              \
    defined(DEBUG_CODE128) || defined(DEBUG_QR_FINDER) ||       \
    (defined(DEBUG_PDF417) && (DEBUG_PDF417 >= 4))
# define DEBUG_LEVEL 1
#endif
#include "debug.h"

#if 1
/* return current element color */
char get_color (const zbar_decoder_t *dcode)
{
    return(dcode->idx & 1);
}

/* retrieve i-th previous element width */
 unsigned get_width (const zbar_decoder_t *dcode,
                                  unsigned char offset)
{
    return(dcode->w[(dcode->idx - offset) & (DECODE_WINDOW - 1)]);
}

/* retrieve bar+space pair width starting at offset i */
unsigned pair_width (const zbar_decoder_t *dcode,
                                   unsigned char offset)
{
    return(get_width(dcode, offset) + get_width(dcode, offset + 1));
}

/* calculate total character width "s"
 *   - start of character identified by context sensitive offset
 *     (<= DECODE_WINDOW - n)
 *   - size of character is n elements
 */
unsigned calc_s (const zbar_decoder_t *dcode,
                               unsigned char offset,
                               unsigned char n)
{
    /* FIXME check that this gets unrolled for constant n */
    unsigned s = 0;
    while(n--)
        s += get_width(dcode, offset++);
    return(s);
}

/* fixed character width decode assist
 * bar+space width are compared as a fraction of the reference dimension "x"
 *   - +/- 1/2 x tolerance
 *   - measured total character width (s) compared to symbology baseline (n)
 *     (n = 7 for EAN/UPC, 11 for Code 128)
 *   - bar+space *pair width* "e" is used to factor out bad "exposures"
 *     ("blooming" or "swelling" of dark or light areas)
 *     => using like-edge measurements avoids these issues
 *   - n should be > 3
 */
int decode_e (unsigned e,
                            unsigned s,
                            unsigned n)
{
    /* result is encoded number of units - 2
     * (for use as zero based index)
     * or -1 if invalid
     */
    unsigned char E = ((e * n * 2 + 1) / s - 3) / 2;
    return((E >= n - 3) ? -1 : E);
}

/* acquire shared state lock */
char get_lock (zbar_decoder_t *dcode,
                             zbar_symbol_type_t req)
{
    if(dcode->lock)
        return(1);
    dcode->lock = req;
    return(0);
}

/* ensure output buffer has sufficient allocation for request */
char size_buf (zbar_decoder_t *dcode,
                             unsigned len)
{
    unsigned char *buf;
    if(len < dcode->buf_alloc)
        /* FIXME size reduction heuristic? */
        return(0);
    if(len > BUFFER_MAX)
        return(1);
    if(len < dcode->buf_alloc + BUFFER_INCR) {
        len = dcode->buf_alloc + BUFFER_INCR;
        if(len > BUFFER_MAX)
            len = BUFFER_MAX;
    }
    buf = realloc(dcode->buf, len);
    if(!buf)
        return(1);
    dcode->buf = buf;
    dcode->buf_alloc = len;
    return(0);
}
#endif


zbar_decoder_t *zbar_decoder_create ()
{
    zbar_decoder_t *dcode = calloc(1, sizeof(zbar_decoder_t));
    dcode->buf_alloc = BUFFER_MIN;
    dcode->buf = malloc(dcode->buf_alloc);

    /* initialize default configs */
#ifdef ENABLE_EAN
    dcode->ean.enable = 1;
    dcode->ean.ean13_config = ((1 << ZBAR_CFG_ENABLE) |
                               (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.ean8_config = ((1 << ZBAR_CFG_ENABLE) |
                              (1 << ZBAR_CFG_EMIT_CHECK));
    dcode->ean.upca_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.upce_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn10_config = 1 << ZBAR_CFG_EMIT_CHECK;
    dcode->ean.isbn13_config = 1 << ZBAR_CFG_EMIT_CHECK;
#endif
#ifdef ENABLE_I25
    dcode->i25.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->i25, ZBAR_CFG_MIN_LEN) = 6;
#endif
#ifdef ENABLE_CODE39
    dcode->code39.config = 1 << ZBAR_CFG_ENABLE;
    CFG(dcode->code39, ZBAR_CFG_MIN_LEN) = 1;
#endif
#ifdef ENABLE_CODE128
    dcode->code128.config = 1 << ZBAR_CFG_ENABLE;
#endif
#ifdef ENABLE_PDF417
    dcode->pdf417.config = 1 << ZBAR_CFG_ENABLE;
#endif
#ifdef ENABLE_QRCODE
    dcode->qrf.config = 1 << ZBAR_CFG_ENABLE;
#endif

    zbar_decoder_reset(dcode);
    return(dcode);
}

void zbar_decoder_destroy (zbar_decoder_t *dcode)
{
    if(dcode->buf)
        free(dcode->buf);
    free(dcode);
}

void zbar_decoder_reset (zbar_decoder_t *dcode)
{
    memset(dcode, 0, (long)&dcode->buf_alloc - (long)dcode);
/*#ifdef ENABLE_EAN
    ean_reset(&dcode->ean);
#endif
#ifdef ENABLE_I25
    i25_reset(&dcode->i25);
#endif
#ifdef ENABLE_CODE39
    code39_reset(&dcode->code39);
#endif

#ifdef ENABLE_CODE128
    code128_reset(&dcode->code128);
#endif
#ifdef ENABLE_PDF417
    pdf417_reset(&dcode->pdf417);
#endif*/
#ifdef ENABLE_QRCODE
    qr_finder_reset(&dcode->qrf);
#endif
}

void zbar_decoder_new_scan (zbar_decoder_t *dcode)
{
    /* soft reset decoder */
    memset(dcode->w, 0, sizeof(dcode->w));
    dcode->lock = 0;
    dcode->idx = 0;
/*#ifdef ENABLE_EAN
    ean_new_scan(&dcode->ean);
#endif
#ifdef ENABLE_I25
    i25_reset(&dcode->i25);
#endif
#ifdef ENABLE_CODE39
    code39_reset(&dcode->code39);
#endif

#ifdef ENABLE_CODE128
    code128_reset(&dcode->code128);
#endif
#ifdef ENABLE_PDF417
    pdf417_reset(&dcode->pdf417);
#endif*/
#ifdef ENABLE_QRCODE
    qr_finder_reset(&dcode->qrf);
#endif
}


zbar_color_t zbar_decoder_get_color (const zbar_decoder_t *dcode)
{
    return(get_color(dcode));
}

const char *zbar_decoder_get_data (const zbar_decoder_t *dcode)
{
    return((char*)dcode->buf);
}

unsigned int zbar_decoder_get_data_length (const zbar_decoder_t *dcode)
{
    return(dcode->buflen);
}

zbar_decoder_handler_t *
zbar_decoder_set_handler (zbar_decoder_t *dcode,
                          zbar_decoder_handler_t handler)
{
    zbar_decoder_handler_t *result = dcode->handler;
    dcode->handler = handler;
    return(result);
}

void zbar_decoder_set_userdata (zbar_decoder_t *dcode,
                                void *userdata)
{
    dcode->userdata = userdata;
}

void *zbar_decoder_get_userdata (const zbar_decoder_t *dcode)
{
    return(dcode->userdata);
}

zbar_symbol_type_t zbar_decoder_get_type (const zbar_decoder_t *dcode)
{
    return(dcode->type);
}

zbar_symbol_type_t zbar_decode_width (zbar_decoder_t *dcode,
                                      unsigned w)
{
    zbar_symbol_type_t sym;
    dcode->w[dcode->idx & (DECODE_WINDOW - 1)] = w;
    dprintf(1, "    decode[%x]: w=%d (%g)\n", dcode->idx, w, (w / 32.));

    /* each decoder processes width stream in parallel */
    sym = dcode->type = ZBAR_NONE;
/*
#ifdef ENABLE_EAN
    if((dcode->ean.enable) &&
       (sym = _zbar_decode_ean(dcode)))
        dcode->type = sym;
#endif
#ifdef ENABLE_CODE39
    if(TEST_CFG(dcode->code39.config, ZBAR_CFG_ENABLE) &&
       (sym = _zbar_decode_code39(dcode)) > ZBAR_PARTIAL)
        dcode->type = sym;
#endif
#ifdef ENABLE_CODE128
    if(TEST_CFG(dcode->code128.config, ZBAR_CFG_ENABLE) &&
       (sym = _zbar_decode_code128(dcode)) > ZBAR_PARTIAL)
        dcode->type = sym;
#endif
#ifdef ENABLE_I25
    if(TEST_CFG(dcode->i25.config, ZBAR_CFG_ENABLE) &&
       (sym = _zbar_decode_i25(dcode)) > ZBAR_PARTIAL)
        dcode->type = sym;
#endif
#ifdef ENABLE_PDF417
    if(TEST_CFG(dcode->pdf417.config, ZBAR_CFG_ENABLE) &&
       (sym = _zbar_decode_pdf417(dcode)) > ZBAR_PARTIAL)
        dcode->type = sym;
#endif*/
#ifdef ENABLE_QRCODE
    if(TEST_CFG(dcode->qrf.config, ZBAR_CFG_ENABLE) &&
       (sym = _zbar_find_qr(dcode)) > ZBAR_PARTIAL)
        dcode->type = sym;
#endif

    dcode->idx++;
    if(dcode->type) {
        if(dcode->handler)
            dcode->handler(dcode);
        if(dcode->lock && dcode->type > ZBAR_PARTIAL)
            dcode->lock = 0;
    }
    return(dcode->type);
}

int decoder_set_config_bool (zbar_decoder_t *dcode,
                                           zbar_symbol_type_t sym,
                                           zbar_config_t cfg,
                                           int val)
{
    unsigned *config = NULL;
    switch(sym) {
#ifdef ENABLE_EAN
    case ZBAR_EAN13:
        config = &dcode->ean.ean13_config;
        break;

    case ZBAR_EAN8:
        config = &dcode->ean.ean8_config;
        break;

    case ZBAR_UPCA:
        config = &dcode->ean.upca_config;
        break;

    case ZBAR_UPCE:
        config = &dcode->ean.upce_config;
        break;

    case ZBAR_ISBN10:
        config = &dcode->ean.isbn10_config;
        break;

    case ZBAR_ISBN13:
        config = &dcode->ean.isbn13_config;
        break;
#endif

#ifdef ENABLE_I25
    case ZBAR_I25:
        config = &dcode->i25.config;
        break;
#endif

#ifdef ENABLE_CODE39
    case ZBAR_CODE39:
        config = &dcode->code39.config;
        break;
#endif

#ifdef ENABLE_CODE128
    case ZBAR_CODE128:
        config = &dcode->code128.config;
        break;
#endif

#ifdef ENABLE_PDF417
    case ZBAR_PDF417:
        config = &dcode->pdf417.config;
        break;
#endif

#ifdef ENABLE_QRCODE
    case ZBAR_QRCODE:
        config = &dcode->qrf.config;
        break;
#endif

    /* FIXME handle addons */

    default:
        return(1);
    }
    if(!config || cfg >= ZBAR_CFG_NUM)
        return(1);

    if(!val)
        *config &= ~(1 << cfg);
    else if(val == 1)
        *config |= (1 << cfg);
    else
        return(1);

#ifdef ENABLE_EAN
    dcode->ean.enable = TEST_CFG(dcode->ean.ean13_config |
                                 dcode->ean.ean8_config |
                                 dcode->ean.upca_config |
                                 dcode->ean.upce_config |
                                 dcode->ean.isbn10_config |
                                 dcode->ean.isbn13_config,
                                 ZBAR_CFG_ENABLE);
#endif

    return(0);
}

int decoder_set_config_int (zbar_decoder_t *dcode,
                                          zbar_symbol_type_t sym,
                                          zbar_config_t cfg,
                                          int val)
{
    switch(sym) {

#ifdef ENABLE_I25
    case ZBAR_I25:
        CFG(dcode->i25, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODE39
    case ZBAR_CODE39:
        CFG(dcode->code39, cfg) = val;
        break;
#endif
#ifdef ENABLE_CODE128
    case ZBAR_CODE128:
        CFG(dcode->code128, cfg) = val;
        break;
#endif
#ifdef ENABLE_PDF417
    case ZBAR_PDF417:
        CFG(dcode->pdf417, cfg) = val;
        break;
#endif

    default:
        return(1);
    }
    return(0);
}

int zbar_decoder_set_config (zbar_decoder_t *dcode,
                             zbar_symbol_type_t sym,
                             zbar_config_t cfg,
                             int val)
{
    if(sym == ZBAR_NONE) {
        zbar_decoder_set_config(dcode, ZBAR_EAN13, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_EAN8, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_UPCA, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_UPCE, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_ISBN10, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_ISBN13, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_I25, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_CODE39, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_CODE128, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_PDF417, cfg, val);
        zbar_decoder_set_config(dcode, ZBAR_QRCODE, cfg, val);
        return(0);
    }

    if(cfg >= 0 && cfg < ZBAR_CFG_NUM)
        return(decoder_set_config_bool(dcode, sym, cfg, val));
    else if(cfg >= ZBAR_CFG_MIN_LEN && cfg <= ZBAR_CFG_MAX_LEN)
        return(decoder_set_config_int(dcode, sym, cfg, val));
    else
        return(1);
}


static char *decoder_dump = NULL;
static unsigned decoder_dumplen = 0;

const char *_zbar_decoder_buf_dump (unsigned char *buf,
                                    unsigned int buflen)
{
    char *p;
    int i;
    int dumplen = (buflen * 3) + 12;
    if(!decoder_dump || dumplen > decoder_dumplen) {
        if(decoder_dump)
            free(decoder_dump);
        decoder_dump = malloc(dumplen);
        decoder_dumplen = dumplen;
    }
    p = decoder_dump +
        snprintf(decoder_dump, 12, "buf[%04x]=",
                 (buflen > 0xffff) ? 0xffff : buflen);
    for(i = 0; i < buflen; i++)
        p += snprintf(p, 4, "%s%02x", (i) ? " " : "",  buf[i]);
    return(decoder_dump);
}
