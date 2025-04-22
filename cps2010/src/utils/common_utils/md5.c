#include <core.h>
#include <cpsdcps.h>
#include <dcps_func.h>



#undef in
#undef F1
#undef F2
#undef F3
#undef F4

typedef struct etri_md5_context
{
	uint32_t buf[4];
	uint32_t bits[2];
	uint8_t  in[64];
} etri_md5_context;

void etri_md5_init(etri_md5_context *pms);
void etri_md5_update(etri_md5_context *pms, const uint8_t *data, unsigned nbytes);
void etri_md5_final(etri_md5_context *pms, uint8_t digest[16]);

typedef struct etri_hmac_md5_context
{
    etri_md5_context  context;
    uint8_t	    k_opad[64];
} etri_hmac_md5_context;


#define byteReverse(buf, len)

static void MD5Transform(uint32_t buf[4], uint32_t const in[16]);


void etri_md5_init(etri_md5_context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
	memset(ctx->in, 0, 64);
}

void etri_md5_update(etri_md5_context *ctx, 
			    unsigned char const *buf, unsigned len)
{
    uint32_t t;


    t = ctx->bits[0];
    if ((ctx->bits[0] = t + ((uint32_t) len << 3)) < t)
	ctx->bits[1]++;
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;

    if (t) {
	unsigned char *p = (unsigned char *) ctx->in + t;

	t = 64 - t;
	if (len < t) {
	    memcpy(p, buf, len);
	    return;
	}
	memcpy(p, buf, t);
	byteReverse(ctx->in, 16);
	MD5Transform(ctx->buf, (uint32_t *) ctx->in);
	buf += t;
	len -= t;
    }

    while (len >= 64) {
	memcpy(ctx->in, buf, 64);
	byteReverse(ctx->in, 16);
	MD5Transform(ctx->buf, (uint32_t *) ctx->in);
	buf += 64;
	len -= 64;
    }

    memcpy(ctx->in, buf, len);
}


void etri_md5_final(etri_md5_context *ctx, unsigned char digest[16])
{
    unsigned count;
    unsigned char *p;

    count = (ctx->bits[0] >> 3) & 0x3F;

    p = ctx->in + count;
    *p++ = 0x80;

    count = 64 - 1 - count;

    if (count < 8) {
		memset(p, 0, count);
		byteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (uint32_t *) ctx->in);

		memset(ctx->in, 0, 56);
    } else {
		memset(p, 0, count - 8);
    }
    byteReverse(ctx->in, 14);

    ((uint32_t *) ctx->in)[14] = ctx->bits[0];
    ((uint32_t *) ctx->in)[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (uint32_t *) ctx->in);
    byteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
	memset(ctx, 0, sizeof(ctx));
}

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))


#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


static void MD5Transform(uint32_t buf[4], uint32_t const in[16])
{
    register uint32_t a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


void etri_hmac_md5_init(etri_hmac_md5_context *hctx, 
			      const uint8_t *key, unsigned key_len)
{
    uint8_t k_ipad[64];
    uint8_t tk[16];
    int i;

    if (key_len > 64) {
        etri_md5_context      tctx;

        etri_md5_init(&tctx);
        etri_md5_update(&tctx, key, key_len);
        etri_md5_final(&tctx, tk);

        key = tk;
        key_len = 16;
    }

	memset(k_ipad, 0, sizeof(k_ipad));
	memset(hctx->k_opad, 0, sizeof(hctx->k_opad));

    memcpy( k_ipad, key, key_len);
    memcpy( hctx->k_opad, key, key_len);

    for (i=0; i<64; i++) {
        k_ipad[i] ^= 0x36;
        hctx->k_opad[i] ^= 0x5c;
    }

    etri_md5_init(&hctx->context);
    etri_md5_update(&hctx->context, k_ipad, 64);

}


void etri_hmac_md5_update(etri_hmac_md5_context *hctx,
				 const uint8_t *input, 
				 unsigned input_len)
{
    etri_md5_update(&hctx->context, input, input_len);
}


void etri_hmac_md5_final(etri_hmac_md5_context *hctx,
				uint8_t digest[16])
{
    etri_md5_final(&hctx->context, digest);

    etri_md5_init(&hctx->context);
    etri_md5_update(&hctx->context, hctx->k_opad, 64);
    etri_md5_update(&hctx->context, digest, 16);
    etri_md5_final(&hctx->context, digest);
}

void etri_hmac_md5( const uint8_t *input, unsigned input_len, 
			  const uint8_t *key, unsigned key_len, 
			  uint8_t digest[16] )
{
    etri_hmac_md5_context ctx;

    etri_hmac_md5_init(&ctx, key, key_len);
    etri_hmac_md5_update(&ctx, input, input_len);
    etri_hmac_md5_final(&ctx, digest);
}

#define MD5STRLEN 32

#define MD5_APPEND(pms,buf,len)	etri_md5_update(pms, (const uint8_t*)buf, len)
#define etri_hex_digits	"0123456789abcdef"

void etri_val_to_hex_digit(unsigned value, char *p)
{
    *p++ = etri_hex_digits[ (value & 0xF0) >> 4 ];
    *p   = etri_hex_digits[ (value & 0x0F) ];
}

void digest2str(const unsigned char digest[], char *output)
{
    int i;
    for (i = 0; i<16; ++i) {
	etri_val_to_hex_digit(digest[i], output);
	output += 2;
    }
}

void hex2digit(unsigned value, char *p)
{
    static char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7',
			 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    *p++ = hex[ (value & 0xF0) >> 4 ];
    *p++ = hex[ (value & 0x0F) ];
}

void get_key_guid( message_t* p_message, uint8_t* p_data, int size )
{
    etri_md5_context pms;

	etri_md5_init(&pms);
	MD5_APPEND( &pms, p_data, size);
	etri_md5_final(&pms, p_message->key_guid);
}

