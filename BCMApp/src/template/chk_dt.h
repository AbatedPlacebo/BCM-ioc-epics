#ifndef CHK_DATATIME_H
#define CHK_DATATIME_H

#if !(defined(GEN_DB) || defined(GEN_DBD) || defined(GEN_WIKI) || defined(GEN_PY))

#include <sys/time.h>
#include <time.h>

#ifndef D_PRINTF
#define D_PRINTF printf
#include <stdio.h>
#endif

#ifndef D_DATATIME
#define D_DATATIME D_DATATIME_F();

static void D_DATATIME_F()
{
	char buf[40];
	struct timeval tv;
	struct tm tvtm;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tvtm);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tvtm);
	D_PRINTF("%s:", buf);
	return;
	(void)D_DATATIME_F;
}

#endif
#endif

#endif
