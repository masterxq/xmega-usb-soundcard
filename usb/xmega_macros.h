#ifndef __XMEGA_MACROS_H__
#define __XMEGA_MACROS_H__ 1

// Load and toggle
#ifdef __GNUC__
	#define LATR16(addr,msk) \
		__asm__ __volatile__ ( \
		"ldi r16, %1" "\n\t" \
		".dc.w 0x9307" "\n\t"\
		::"z" (addr), "M" (msk):"r16")
#else
	#define LATR16(addr,msk) __lat(msk,addr)
#endif


#define USB_EP_size_to_gc(x)  ((x <= 8   )?USB_EP_BUFSIZE_8_gc:\
                               (x <= 16  )?USB_EP_BUFSIZE_16_gc:\
                               (x <= 32  )?USB_EP_BUFSIZE_32_gc:\
                               (x <= 64  )?USB_EP_BUFSIZE_64_gc:\
                               (x <= 128 )?USB_EP_BUFSIZE_128_gc:\
                               (x <= 256 )?USB_EP_BUFSIZE_256_gc:\
                               (x <= 512 )?USB_EP_BUFSIZE_512_gc:\
                                           USB_EP_BUFSIZE_1023_gc)


#ifdef __GNUC__
#define LACR16(addr,msk) \
	__asm__ __volatile__ ( \
	"ldi r16, %1" "\n\t" \
	".dc.w 0x9306" "\n\t"\
	::"z" (addr), "M" (msk):"r16")
#else
	#define LACR16(addr,msk) __lac((unsigned char)msk,(unsigned char*)addr)
#endif

#endif
