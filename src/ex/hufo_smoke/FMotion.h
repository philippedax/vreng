#ifndef FMOTION_H
#define FMOTION_H
//#include "CPoint3D.h"

bool FMotionInit();
void FMotionQuit();
void FMotionAnimate(float dt);
//void GetFMotion(const SVector3D &p,SVector3D &v);
void FMotionWarp(SVector3D &p,float dt);
void FMotionWarp2(SVector3D &p,float dt);
void AffFMotion();
#endif