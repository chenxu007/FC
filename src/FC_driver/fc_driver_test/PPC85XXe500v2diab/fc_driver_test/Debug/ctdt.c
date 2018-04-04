/* ctors and dtors arrays -- to be used by runtime system */
/*   to call static constructors and destructors          */
/*                                                        */
/* NOTE: Use a C compiler to compile this file. If you    */
/*       are using GNU C++, be sure to use compile this   */
/*       file using a GNU compiler with the               */
/*       -fdollars-in-identifiers flag.                   */


#ifdef __DCC__
asm("     	.text");
asm("     	.section        .init$00,4,C");
asm("	.text");
asm("	.section        .fini$00,4,C");
asm("	.section	.frame_info$00,4,r");
asm("_frame_info_start:");
asm("	.section	.frame_info$99");
asm("_frame_info_end:");
#endif

#ifdef __GNUC__
#error Diab C++ code must be munched and linked using the Diab toolchain
#endif

extern unsigned long _frame_info_start[];
extern unsigned long _frame_info_end[];

extern void __frameinfo_add(void *, void *);
extern void __frameinfo_del(void *, void *);

static void _STI__15_ctors()
{
	__frameinfo_add(_frame_info_start, _frame_info_end);
}

static void _STD__15_ctors()
{
	__frameinfo_del(_frame_info_start, _frame_info_end);
}

char __dso_handle = 0;

void _STI___18_HwaFcDriverlib_cpp_5d8f6f29();

extern void (*_ctors[])();
void (*_ctors[])() =
    {
    _STI__15_ctors,
    _STI___18_HwaFcDriverlib_cpp_5d8f6f29,
    0
    };

extern void (*_dtors[])();
void (*_dtors[])() =
    {
    _STD__15_ctors,
    0
    };

/* build variables */
#ifdef __GNUC__
__asm("	.section \".wrs_build_vars\",\"a\"");
#endif
#ifdef __DCC__
__asm("	.section \".wrs_build_vars\",\"r\"");
#endif
__asm(" .ascii \"tag SMP 0\"");
__asm(" .byte 0");
__asm("	.ascii \"end\"");
__asm("	.byte 0");
__asm("	.previous");
