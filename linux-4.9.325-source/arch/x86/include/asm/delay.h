#ifndef _ASM_X86_DELAY_H
#define _ASM_X86_DELAY_H

#include <asm-generic/delay.h>

#ifndef CONFIG_COOPERATIVE
void use_tsc_delay(void);
#endif
void use_mwaitx_delay(void);

#endif /* _ASM_X86_DELAY_H */
