#include "stdafx.h"
#include <stdio.h>

bool HErrorAff(char *s,char *file,int line)
{
	static char buf[1024];
	char *f;
	f=file;
	while (*f) ++f;
	while (f>file && f[-1]!='\\' && f[-1]!='/' && f>file) --f;
	sprintf(buf,"%s(%d)",f,line);
	MessageBox(NULL,s,buf,MB_ICONERROR|MB_OK);
	return r_err;
}

bool HMsgAff(char *s,char *file,int line)
{
	static char buf[1024];
	char *f;
	f=file;
	while (*f) ++f;
	while (f>file && f[-1]!='\\' && f[-1]!='/' && f>file) --f;
	sprintf(buf,"%s(%d)",f,line);
	int m=MessageBox(NULL,s,buf,MB_ICONWARNING|MB_OKCANCEL);
	//return r_err;
	return m==IDOK;
}

char HTextBuf[1024];
char *HText(char *s,...)
{
	va_list arglist;
	va_start( arglist, s );
	vsprintf( HTextBuf,s, arglist );
	va_end( arglist );
	return HTextBuf;
}
