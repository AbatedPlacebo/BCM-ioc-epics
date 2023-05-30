#ifndef CHK_H
#define CHK_H

#ifndef D_PREF
#define D_PREF
#endif

#define Dif(level) if(level<=debug_level)

#ifndef D_PRINTF
#define D_PRINTF printf
#include<stdio.h>
#endif

#define D(level,fmto) do{ \
	if(level<=debug_level) { \
		CHKCURPOS \
		D_PRINTF(D_PREF "%s:%i:",D_FILE_NAME(__FILE__),__LINE__); \
		D_DATATIME; \
		D_PRINTF fmto; \
	} \
}while(0)

#define Dcont(level,fmto) do{ \
	if(level<=debug_level) { \
		D_PRINTF fmto; \
	} \
}while(0)

#ifndef CHKCURPOS
#define CHKCURPOS
#endif
#ifndef D_DATATIME
#define D_DATATIME
#endif
#define CHK_STR(val) #val
#define CHK(val)     do{ if( (val)<0) {D(2,(       "%s\n",CHK_STR(val))); goto CHK_ERR;} }while(0)
#define CHKTRUE(val) do{ if(!(val)  ) {D(2,("FALSE: %s\n",CHK_STR(val))); goto CHK_ERR;} }while(0)
#define WARN(val)     do{ if( (val)<0) {D(2,("WARN: %s\n",CHK_STR(val)));              } }while(0)
#define WARNTRUE(val) do{ if(!(val)  ) {D(2,("WARN: %s\n",CHK_STR(val)));              } }while(0)
#ifndef D_FILE_NAME
static const char* D_FILE_NAME(const char* file)
{
	const char* name = file;
	for(name=file; *file!=0; file++)
		if((*file=='/' || *file=='\\') && file[1]!=0)
			name = file + 1;
	return name;
	(void)D_FILE_NAME;
}
#endif
#endif
