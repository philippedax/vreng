//-------------------------------------------------------------------
//	File:		CObject.cpp
//	Created:	01/12/99 0:24:AM
//	Author:		Allard Jeremie
//	Comments:	Create and manage Objects.
//-------------------------------------------------------------------
#include "stdafx.h"
//#include "Texture.h"
#include "CObject.h"
#include <stdio.h>

//#define HMsgLWO(s) Test(HMsg(s))
#define HMsgLWO(s) 

#define HFRegType(type) \
INLINE bool HFRead(FILE* f,type *p,int n=1) \
{ \
	return (int)fread(p,sizeof(type),n,f)==n; \
}

HFRegType(int)
HFRegType(char)
HFRegType(Byte)
HFRegType(Word)
HFRegType(DWord)
HFRegType(float)
HFRegType(double)

INLINE int HFPos(FILE* f)
{
	return ftell(f);
}

#define HFD_DEBUT SEEK_SET
#define HFD_COURANT SEEK_CUR
#define HFD_FIN SEEK_END
INLINE int HFDepl(FILE *f,int n,int p=HFD_COURANT)
{
	return fseek(f,n,p);
}

CObject::CObject()
: nbf(0),nbp(0),fc(NULL),pt(NULL)
{}

CObject::~CObject()
{
	Free();
}

void CObject::Free()
{
	if (fc) { delete[] fc; fc=NULL; }
	if (pt) { delete[] pt; pt=NULL; }
	nbf=0; nbp=0;
}

bool HFReadLWOStr(FILE *h,char *s,int *l)
{ //lecture d'une chaine de caractères
	for (;;)
	{
		Test(HFRead(h,s,2)); if (l!=NULL) *l-=2;
		if (s[0]==0 || s[1]==0) break;
		s+=2;
	}
	return r_ok;
}

bool CObject::LoadLWO(char* szFileName)
{
	FILE *h;
	TestP(h=fopen(szFileName,"rb"));
	int length,l;
	//DWord length,l;
	union
	{
		DWord id;
		char sid[5];
	};
	sid[4]=0;
	Word w,nw;
	int i,j;
	float f;
	//char *sname;
	int sp;
	//H3DSurface *ss;
	//int ns=0; // nombre de surfaces
	//char *sname[32]; // maximum de 32 surfaces
	char buf[256];
	//int snum;
	//LWOMapping *tsurf=NULL;
	//LWOMapping mapbak;
	//Bool usemap;
	//int curtxt;
	//for (j=0;j<32;j++) psurf[j]=NULL;
	Test(HFRead(h,&id));
	if (id!='MROF') return HErrorN(HText("Bad RIFF FORM id:%x",id));
	Test(HFRead(h,&length));
	length=dswap(length);
	Test(HFRead(h,&id));
	length-=sizeof(id);
	if (id!='BOWL') return HErrorN(HText("Bad lightwave id:%x",id));
	//nbp=nbf=0; o->p=NULL; o->f=NULL;
	Free();
	CSurface *ss=NULL;
	while (length)
	{
		Test(HFRead(h,&id));            length-=sizeof(id);
		Test(HFRead(h,&l)); l=dswap(l); length-=sizeof(l);
		if (l>length) return HErrorN(HText("Bad chunk size: end after EOF (id=%s),diff=%d",sid,l-length));
		HMsgLWO(HText("chunk: %x(%s) size=%d",id,sid,l));
		length-=l;
		switch(id)
		{
			case 'SFRS' : // SRFS
				if (nbs) return HErrorN("2 SRFS chunks");
				sp=HFPos(h);
				while (l)
				{
					Test(HFReadLWOStr(h,buf,&l));
					if (l<0) return HErrorN("Bad chunk size");
					++nbs;
				}
				HFDepl(h,sp,HFD_DEBUT);
				if (nbs)
				{
					sf=new CSurface[nbs];
					for (j=0;j<nbs;++j)
					{
						Test(HFReadLWOStr(h,sf[j].name,NULL));
						HMsgLWO(HText("new material:%s",sf[j].name));
					}
				}
				break;
			case 'FRUS' : // SURF
			{
				if (!nbs) return HErrorN("no surface");
				Test(HFReadLWOStr(h,buf,&l));
				if (l<0) return HErrorN("Bad chunk size");
				for (j=0;j<nbs;++j)
					if (!stricmp(buf,sf[j].name)) break;
				if (j==nbs) return HErrorN(HText("unknown LW surface:%s",buf));
				ss=&sf[j];
				ss->flag=0;
				//ss->diff=0;
				//ss->spec=0;
				//ss->t=H3DSURF_FLAT;
				/*tsurf[snum].map=None;
				tsurf[snum].axe=Axe0;
				tsurf[snum].ctr.x=tsurf[snum].ctr.y=tsurf[snum].ctr.z=0.0;
				tsurf[snum].size.x=tsurf[snum].size.y=tsurf[snum].size.z=1.0;
				tsurf[snum].warpx=False;
				tsurf[snum].warpy=False;
				curtxt=0;*/
				int ll;
				CColor4f colr(0.5f,0.5f,0.5f);
				float lumi=0;
				float diff=1.0f;
				float spec=0;
				int glos=64;
				float refl=0;
				float trans=0;
				Byte byte;
				while (l)
				{
					Test(HFRead(h,&id));              l-=sizeof(id);
					Test(HFRead(h,&w )); ll=wswap(w); l-=sizeof(w );
					//Test(HMsg(HText("subchunk %s, size %d",sid,ll)));
					if (ll>l) return HErrorN(HText("Bad chunk size: end after EOF (id=%s),diff=%d %x",sid,ll-l,ll));
					l-=ll;
					switch(id)
					{
						case 'GALF' : //FLAG
							if (ll!=2) return HErrorN("invalid FLAG subchunk size");
							Test(HFRead(h,&w )); w=wswap(w); ll-=sizeof(w );
							//if (w&4) ss->flag&=~CMaterial::Smooth; //ss->flag|=CMaterial::Smooth; //&=~CMaterial::Flat;
							if (w&4) ss->flag|=CMaterial::Smooth; //&=~CMaterial::Flat;
							//else     ss->flag|=CMaterial::Flat;
							if (w&256) ss->flag|=CMaterial::Double;
							if (w&512) ss->flag|=CMaterial::Blend|CMaterial::Additive;
							break;
						case 'RLOC' : // COLR
							if (ll!=4) return HErrorN("invalid COLR subchunk size");
							Test(HFRead(h,&byte)); colr.r=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte)); colr.g=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte)); colr.b=byte*(1.0f/255.0f);
							Test(HFRead(h,&byte));
							break;
						case 'IMUL' : // LUMI
							if (ll!=2) return HErrorN("invalid LUMI subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							lumi=w*(1.0f/255.0f);
							break;
						case 'FFID' : // DIFF
							if (ll!=2) return HErrorN("invalid DIFF subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							diff=w*(1.0f/255.0f);
							break;
						case 'CEPS' : // SPEC
							if (ll!=2) return HErrorN("invalid SPEC subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							spec=w*(1.0f/255.0f);
							break;
						case 'SOLG' : // GLOS
							if (ll!=2) return HErrorN("invalid GLOS subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							glos=w;
							break;
						case 'LFER' : // REFL
							if (ll!=2) return HErrorN("invalid REFL subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							refl=w*(1.0f/255.0f);
							break;
						case 'NART' : // TRAN
							if (ll!=2) return HErrorN("invalid TRAN subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							trans=0.5f; //w*(1.0f/255.0f);
							break;
/*						case 'XETC' : // CTEX (color)
						case 'XETD' : // DTEX (diffuse)
						case 'XETS' : // STEX (specular)
						case 'XETR' : // RTEX (reflection)
						case 'XETT' : // TTEX (transparency)
						case 'XETB' : // BTEX (bump)
							curtxt=id; usemap=False;
							Test(H3DReadLWO(h,&sname,&ll));
							if (ll) return HErrorN("invalid ?TEX subchunk size");
							     if (curtxt=='XETC') ss->t|=H3DSURF_TEXT;
							else if (curtxt=='XETB') ss->t|=H3DSURF_BUMP;
							else if (curtxt=='XETT') ss->t|=H3DSURF_TRANS;
							else break;
							mapbak=tsurf[snum]; usemap=True;
							if (!strcmp(sname,"Planar Image Map"))
								tsurf[snum].map=Plan;
							else if (!strcmp(sname,"Cylindrical Image Map"))
								tsurf[snum].map=Cyl;
							else if (!strcmp(sname,"Spherical Image Map"))
								tsurf[snum].map=Sphe;
							else return HErrorN("unknown LW mapping"); //tsurf[snum].map=None;
							if (mapbak.map!=None && tsurf[snum].map!=mapbak.map)
								return HErrorN("all textures must have the same mapping");
							break;
						case 'GMIT' : // TIMG
							Test(H3DReadLWO(h,&sname,&ll));
							if (ll) return HErrorN("invalid TIMG subchunk size");
							if      (curtxt=='XETC')
								TestP(ss->ctxt=H3DGetTexture(s,sname,H3DTEXT_COLOR|H3DTEXT_WORD,ss->l));
							else if (curtxt=='XETB')
								TestP(ss->btxt=H3DGetTexture(s,sname,H3DTEXT_BUMP));
//								TestP(ss->btxt=H3DGetTexture(s,sname,H3DTEXT_COLOR|H3DTEXT_WORD|H3DTEXT_TILED));
							else if (curtxt=='XETT')
								TestP(ss->ttxt=H3DGetTexture(s,sname,H3DTEXT_ALPHA|H3DTEXT_WORD));
							break;
						case 'GLFT' : // TFLG
							if (ll!=2) return HErrorN("invalid TFLG subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							if (usemap) //tsurf[snum].axe=w&7;
							{
								tsurf[snum].axe=(w&1) ? AxeX : ((w&2) ? AxeY : AxeZ);
								if (mapbak.map!=None && tsurf[snum].axe!=mapbak.axe)
									return HErrorN("all textures must have the same mapping");
							}
							break;
						case 'ZIST' : // TSIZ
							if (ll!=12) return HErrorN("invalid TSIZ subchunk size");
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].size.x=f;
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].size.y=f;
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].size.z=f;
							if (usemap && mapbak.map!=None &&
							    (  tsurf[snum].size.x!=mapbak.size.x
							    || tsurf[snum].size.y!=mapbak.size.y
							    || tsurf[snum].size.z!=mapbak.size.z
							    ))
								return HErrorN("all textures must have the same mapping");
							break;
						case 'RTCT' : // TCTR
							if (ll!=12) return HErrorN("invalid TCTR subchunk size");
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].ctr.x=f;
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].ctr.y=f;
							Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
							if (usemap) tsurf[snum].ctr.z=f;
							if (usemap && mapbak.map!=None &&
							    (  tsurf[snum].ctr.x!=mapbak.ctr.x
							    || tsurf[snum].ctr.y!=mapbak.ctr.y
							    || tsurf[snum].ctr.z!=mapbak.ctr.z
							    ))
								return HErrorN("all textures must have the same mapping");
							break;
						case 'PRWT' : // TWRP
							if (ll!=4) return HErrorN("invalid TWRP subchunk size");
							Test(HFRead(h,&w)); w=wswap(w);
							if (usemap) tsurf[snum].warpx=(w!=0);
							Test(HFRead(h,&w)); w=wswap(w);
							if (usemap) tsurf[snum].warpy=(w!=0);
							if (usemap && mapbak.map!=None &&
							    (  tsurf[snum].warpx!=mapbak.warpx
							    || tsurf[snum].warpy!=mapbak.warpy
							    ))
								return HErrorN("all textures must have the same mapping");
							break;*/
						default: //HMsg(HText("unkown SURF:%s",sid)); //cout << "unknown SURF:"<<(char*)&id<<" chunk\n";
							HFDepl(h,ll,HFD_COURANT);
					}
				}
				colr.a=1.0f-trans;
				if (trans) ss->flag|=CMaterial::Blend;
				ss->ambient.r=colr.r*lumi;
				ss->ambient.g=colr.g*lumi;
				ss->ambient.b=colr.b*lumi;
				ss->ambient.a=colr.a;
				ss->diffuse.r=colr.r*diff;
				ss->diffuse.g=colr.g*diff;
				ss->diffuse.b=colr.b*diff;
				ss->diffuse.a=colr.a;
				ss->specular.r=spec;
				ss->specular.g=spec;
				ss->specular.b=spec;
				ss->specular.a=colr.a;
				ss->emission.r=colr.r*refl;
				ss->emission.g=colr.g*refl;
				ss->emission.b=colr.b*refl;
				ss->emission.a=colr.a;
				ss->shininess=glos;
				if (diff || spec || refl) ss->flag|=CMaterial::Lighting;
				else ss->flag&=~CMaterial::Smooth;
/*				ss->c=H3DColor(ss->p.r,ss->p.g,ss->p.b,ss->l);
				if (ss->ctxt && ss->btxt &&
				    (ss->ctxt->nx!=ss->btxt->nx||ss->ctxt->ny!=ss->btxt->ny))
					return HErrorN("bump map must have the same dimensions as color map");
//				if ((ss->spec>0 && ss->spow>0) || ss->btxt)
				if ((ss->t&H3DSURF_ENVIR) || ss->btxt)
				{ // need light map
					ss->t=(ss->t|H3DSURF_ENVIR)&~(H3DSURF_FLAT|H3DSURF_GOUR);
					if ((ss->etxt=H3DLightTextureFind(ss->lum,ss->diff,ss->spec,ss->spow,s))==NULL)
					{
						TestP(HMAllocP(ss->etxt,H3DTexture));
						TDbg(H3DTextureInit(ss->etxt));
						Test(H3DLightTextureCreate(s,ss->lum,ss->diff,ss->spec,ss->spow,ss->etxt));
						TDbg(H3DTextureAdd(ss->etxt,s));
					}
				}
				else if (ss->lum==255 && ss->diff==0 && ss->spec==0)
					ss->t&=~(H3DSURF_FLAT|H3DSURF_GOUR);*/
				break;
			}
			case 'STNP' : // PNTS
				if (nbp) return HErrorN("2 PNTS chunks");
				if (l%(3*sizeof(float))) return HErrorN("bad PNTS chunk size");
				nbp=l/(3*sizeof(float));
				HMsgLWO(HText("nbp=%d",nbp));
				if (nbp)
				{
					//TestHMAllocN(o->p,H3DPoint,o->nbp);
					pt=new CVertex[nbp];
					for (j=0;j<nbp;j++)
					{
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.x=f;
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.y=f;
						Test(HFRead(h,&f)); *((DWord*)&f)=dswap(*((DWord*)&f));
						pt[j].p.z=f;
					}
				}
				break;
			case 'SLOP' : // POLS
			{
				if (nbf) return HErrorN("2 POLS chunks");
				sp=HFPos(h);
				j=0; nbf=0;
				while (j<l)
				{
					Test(HFRead(h,&w)); w=wswap(w); j+=2;
					HFDepl(h,2*w+2); j+=2*w+2;
					if (w>=3)
						nbf+=w-2;
				}
				if (j>l) return HErrorN(HText("bad POLS chunk:%d!=%d",l,j));
				HMsgLWO(HText("nbf=%d",nbf));
				HFDepl(h,sp,HFD_DEBUT);
				if (nbf)
				{
					fc=new CTriFace[nbf];
					for (j=0;j<nbf;j++)
					{
						Test(HFRead(h,&nw)); nw=wswap(nw); l-=4+nw*2;
						if (nw<=2) HFDepl(h,nw*2+2);
						else
						{
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[0]=&(pt[w]);
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[1]=&(pt[w]);
							Test(HFRead(h,&w)); w=wswap(w);
							if (w>=nbp)
								return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
							fc[j].v[2]=&(pt[w]);
							i=j;
							while (nw>3)
							{ ++j;--nw;
								fc[j].v[0]=fc[i].v[0]; fc[j].v[1]=fc[j-1].v[2];
								Test(HFRead(h,&w)); w=wswap(w);
								if (w>=nbp)
									return HErrorN(HText("LWO:point inexistant:%x (face %d)",w,j));
								fc[j].v[2]=&(pt[w]);
							}
							Test(HFRead(h,&w)); w=wswap(w); // lecture de la surface
							if (w==0 || w>nbs) return HErrorN(HText("LWO: surface inexistante:%x",w));
							sf[w-1].nbf+=1+j-i;
							while (i<=j) fc[i++].m=&sf[w-1]; //psurf[w-1];
						}
					}
					if (j!=nbf) return HErrorN(HText("bad poly number:%d instead of %d",j,nbf));
					if (nbs)
					{
						CTriFace *fb=fc;
						fc=new CTriFace[nbf];
						i=0;
						for (j=0;j<nbs;++j)
						{
							sf[j].fc=fc+i;
							i+=sf[j].nbf;
							sf[j].nbf=0;
						}
						if (i!=nbf) return HErrorN(HText("faces material num error: %d!=%d",i,nbf));
						CSurface *s;
						for (j=0;j<nbf;++j)
						{
							s=(CSurface*)fb[j].m;
							s->fc[s->nbf++]=fb[j];
							//fc[j]=fb[j];
						}
						delete[] fb;
					}
				}
				if (l) { HFDepl(h,l); l=0; }
				break;
			}
			default: //cout << "unknown "<<(char*)&id<<" chunk\n";
				HFDepl(h,l,HFD_COURANT);
		}
	}
//H3DRmsg("LWO:fichier lu\n");
	// ensuite il faut calculer les coords de mapping
/*	cout << "map:"; //HKGet();
	if (o->nbf)
	for (j=0;j<o->nbf;j++)
	{
		ss=o->f[j].s;
		for (snum=0;snum<ns;snum++)
			if (psurf[snum]==ss) break;
		if (snum==ns) return HErrorN(HText("internal error: poly %d/%d have an unknown surface",j,o->nbf));
		if (tsurf[snum].map!=None)
		{ H3DTexture *tt;
			if (tsurf[snum].map==Sphe)
				tsurf[snum].warpx=tsurf[snum].warpy=True;
			if (tsurf[snum].map==Cyl)
				tsurf[snum].warpx=True;
			if (tsurf[snum].warpx || tsurf[snum].warpy)
			{
				if (ss->ctxt) H3DTextureTile(ss->ctxt);
				if (ss->btxt) H3DTextureTile(ss->btxt);
				if (ss->ttxt) H3DTextureTile(ss->ttxt);
			}
			if (ss->ctxt) tt=ss->ctxt;
			else if (ss->btxt) tt=ss->btxt;
			else if (ss->ttxt) tt=ss->ttxt;
			H3DLWOCalcUV(o->f[j].p[0],&tsurf[snum],tt);
			H3DLWOCalcUV(o->f[j].p[1],&tsurf[snum],tt);
			H3DLWOCalcUV(o->f[j].p[2],&tsurf[snum],tt);
		}
	}
	if (tsurf) HMFree(tsurf);
	cout << "ok\n"; //HKGet();
//	// enfin on calcule les vecteurs normaux
*/
	fclose(h);

	if (nbf)
	{
		
		CVertex *pv=pt;
		for (j=0;j<nbp;++j,++pv)
		{
			pv->n.Zero();
		}
		CTriFace *pf=fc;
		for (j=0;j<nbf;++j,++pf)
		{
			pf->CalcNormal();
			pf->v[0]->n+=pf->n;
			pf->v[1]->n+=pf->n;
			pf->v[2]->n+=pf->n;
		}
		pv=pt;
		for (j=0;j<nbp;++j,++pv)
		{
			pv->n.Normalize();
		}
	}
	return r_ok;
}

void CVertex::Put(int pdata)
{
	if (pdata&PUT_NORMAL)
		glNormal3f(n.x,n.y,n.z);
	if (pdata&PUT_COORD)
		glVertex3f(p.x,p.y,p.z);
}

void CTriFace::Put(int pdata)
{
	//if (pdata&PUT_NORMAL)
		//glNormal3f(n.x,n.y,n.z);
	v[0]->Put(pdata);
	v[1]->Put(pdata);
	v[2]->Put(pdata);
}

void CTriFace::Aff(int pdata)
{
	glBegin(GL_TRIANGLES);
		Put(pdata);
	glEnd();
}

void CObject::Aff()
{
	int j;
	if (nbs)
	for (j=0;j<nbs;++j)
		sf[j].Aff(pt);
}

void CObject::Flip()
{
	for (int f=0;f<nbf;f++)
	{
		CVertex *v=fc[f].v[1];
		fc[f].v[1]=fc[f].v[2];
		fc[f].v[2]=v;
		fc[f].n.Scale(-1.0f);
	}
	for (int p=0;p<nbp;p++)
	{
		pt[p].n.Scale(-1.0f);
	}
}

void CObject::AffStrip()
{
	int j;
	if (nbs)
	for (j=0;j<nbs;++j)
		sf[j].AffStrip(pt);
}

int CMaterial::Put()
{
	int pdata=PUT_COORD;
	if (flag&Smooth)
		glEnable(GL_SMOOTH);
	else
		glDisable(GL_SMOOTH);
	if (flag&Double)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);
	if (flag&Blend)
	{
		if (flag&Additive)
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	else
		glDisable(GL_BLEND);
	if (flag&Lighting)
	{
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient.t);
		glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse.t);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular.t);
		glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,emission.t);
		glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,(flag&Double));
		glEnable(GL_LIGHTING);
		pdata|=PUT_NORMAL;
		//diffuse.Put();
	}
	else
	{
		glDisable(GL_LIGHTING);
		ambient.Put();
	}
	return pdata;
}

void CColor4f::Put()
{
	glColor4f(r,g,b,a);
}
#include <stddef.h>
void CSurface::Aff(CVertex *p0)
{
	int pdata=Put(); // configure material
	bool array=false;
	if (p0!=NULL)
	switch(pdata)
	{
		case PUT_COORD:
			glInterleavedArrays(GL_V3F,sizeof(CVertex),((Byte*)p0)+offsetof(CVertex,p));
			array=true;
			break;
		case PUT_COORD|PUT_NORMAL:
			glInterleavedArrays(GL_N3F_V3F,sizeof(CVertex),((Byte*)p0)+offsetof(CVertex,n));
			array=true;
			break;
	}
	int i;
	CTriFace *f;
	glBegin(GL_TRIANGLES);
	f=fc; i=nbf;
	if (array)
	{
		while (i--)
		{
			glArrayElement(f->v[0]-p0);
			glArrayElement(f->v[1]-p0);
			glArrayElement(f->v[2]-p0);
			++f;
		}
	}
	else
	{
		while (i--)
		{
			f->Put(pdata);
			++f;
		}
	}
	glEnd();
}

void CTriFace::CalcNormal()
{
	CPoint3D a=v[1]->p - v[0]->p;
	CPoint3D b=v[2]->p - v[0]->p;
	n.CrossProduct(a,b);
	n.Normalize();
}

Bool CObject::GetStrip(char *fname)
{
	if (nbs!=1) return HErrorN("only object with 1 material can use strip");
	Test(sf[0].GetStrip(fname,pt,nbp));
	return r_ok;
}

Bool CSurface::GetStrip(char *fname, CVertex *p0, int nbp)
{
	FILE *h;
	TestP(h=fopen(fname,"rt"));
	if (!h) return HErrorN(HText("can't open file %s",fname));
	CVertex **pbuf=new CVertex*[nbf*4];
	char buf[256]; char *s,*s0;
	int np,i;
	np=0;
	nbts=0;
	char c;
	while (!feof(h))
	{
		if (fgets(buf,256,h))
		{
			s=buf;
			while (*s==' ' || *s=='\t') ++s;
			switch(*(s++))
			{
				case 't':
					if (nbts)
						pbuf[np++]=NULL;
					++nbts;
				case 'q':
					for(;;)
					{
						while (*s==' ' || *s=='\t') ++s;
						if ((unsigned)(*s-'0')<10)
						{
							s0=s;
							while (*s!=' ' && *s!='\t' && *s!='\0') ++s;
							c=*s;
							*s=0;
							i=atoi(s0);
							if ((unsigned)(i-1)>=(unsigned)nbp) return HErrorN(HText("bad point num:%d",i));
							pbuf[np++]=p0+(i-1);
							*s=c;
						}
						else break;
					}
					break;
			}
		}
	}
	fclose(h);
	pbuf[np++]=NULL;
	//HMsg(HText("%d vertex, %d strip",np-nbts,nbts));
	if (ts) { delete[] ts; ts=NULL; }
	if (nbts)
	{
		ts=new CTriStrip[nbts];
		//CVertex **pv=new CVertex*[np];
		int t,p,j;
		p=0;
		for (t=0;t<nbts;++t)
		{
			i=0;
			while (pbuf[p+i]) ++i;
			ts[t].n=i;
			ts[t].v=new CVertex*[i+1];
			for (j=0;j<=i;++j)
				ts[t].v[j]=pbuf[p+j];
			p+=i+1;
		}
		if (p!=np) return HErrorN(HText("error: p(%d)!=np(%d)",p,np));
	}
	delete[] pbuf;
	return r_ok;
}

void CSurface::AffStrip(CVertex *p0)
{
	if (!nbts) { Aff(p0); return; }
	int pdata=Put(); // configure material
	bool array=false;
	if (p0!=NULL)
	switch(pdata)
	{
		case PUT_COORD:
			glInterleavedArrays(GL_V3F,sizeof(CVertex),p0+offsetof(CVertex,p));
			array=true;
			break;
		case PUT_COORD|PUT_NORMAL:
			glInterleavedArrays(GL_N3F_V3F,sizeof(CVertex),p0+offsetof(CVertex,n));
			array=true;
			break;
	}
	int s,i;
	CTriStrip *ps;
	CVertex **pv;
	s=nbts; ps=ts;
	if (array)
	{
		while (s--)
		{
			i=ps->n;
			pv=ps->v;
			glBegin(GL_TRIANGLE_STRIP);
				while (i--)
				{
					glArrayElement(*pv-p0);
					++pv;
				}
			glEnd();
			++ps;
		}
	}
	else
	{
		while (s--)
		{
			i=ps->n;
			pv=ps->v;
			glBegin(GL_TRIANGLE_STRIP);
				while (i--)
				{
					(*pv)->Put(pdata);
					++pv;
				}
			glEnd();
			++ps;
		}
	}
}

CSurface * CObject::FindSurface(char *name)
{
	CSurface *ps=sf;
	for (int s=0;s<nbs;s++,ps++)
	{
		if (!stricmp(ps->name,name))
			return ps;
	}
	return NULL;
}
