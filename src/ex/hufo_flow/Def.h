#ifndef __DEF_H
#define __DEF_H
typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned long DWord;
#define INLINE __inline

#define r_ok true
#define r_err false
#define Test(b) if (!(b)) return r_err; else
#define TestP(p) if ((p)==NULL) return r_err; else
bool HErrorAff(char *s,char *file,int line);
#define HErrorN(msg) HErrorAff(msg,__FILE__,__LINE__)
bool HMsgAff(char *s,char *file,int line);
#define HMsg(msg) HMsgAff(msg,__FILE__,__LINE__)
char *HText(char *s,...);
// warning C4035: function : no return value
#pragma warning(push)
#pragma warning(disable:4035)

INLINE int dswap(int i)
{
	__asm
	{
		mov eax,i;
		bswap eax;
	}
}

INLINE Word wswap(Word i)
{
	/*__asm
	{
		movzx eax,i;
		xchg ah,al;
	}*/
	return (i<<8)|(i>>8);
}

#pragma warning(pop)

#endif