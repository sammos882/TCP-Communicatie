#include <stdio.h>

void LogTargetCStandard(void)
{
    printf("Target C standard: ");

#if defined(__STDC_VERSION__)

#if __STDC_VERSION__ >= 202311L
    printf("C23 or later");
#elif __STDC_VERSION__ >= 201710L
    printf("C17");
#elif __STDC_VERSION__ >= 201112L
    printf("C11");
#elif __STDC_VERSION__ >= 199901L
    printf("C99");
#else
    printf("C90/C89");
#endif
    printf(" (%ld)", (long)__STDC_VERSION__);
#else
#if defined(_MSC_VER)
    printf("MSVC C compiler ");
#endif
    printf("C standard (__STDC_VERSION__ not available)");
#endif
    printf("\n");
}
