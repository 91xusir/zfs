
#ifndef __MD5_H__
#define __MD5_H__

namespace rt2_core {
namespace rtc_hashing {

/*------------------------------------------------------------------------
-   Calculates a checksum for a block of data using the MD5 message-digest algorithm.
------------------------------------------------------------------------*/

/* MD5 context. */
typedef struct
{
	unsigned int	state[4];
    unsigned int	bits[2];
    unsigned char	in[64];
} MD5_CTX;

void MD5_Init( MD5_CTX *ctx );
void MD5_Update( MD5_CTX *ctx, unsigned char const *buf, unsigned int len );
void MD5_Final( MD5_CTX *ctx, unsigned char digest[16] );

} // namespace rtc_hashing
} // namespace rt2_core

#endif // __MD5_H__

/*------------------------------------------------------------------------
-   The End.
------------------------------------------------------------------------*/
