
#include "gc_include.h"
#include "gc_circle_missile.h"
#include "game_assistant.h"
#include "gc_bomb_missile.h"

const float EXIST_TIME=100.0;//���ʱ��

CCircleMissile::CCircleMissile(RtgVertex3& vrSourcePosition,RtgVertex3& vrTargetPosition,char vpGraphName[],
								 float vRatio,float vSelfRad,CMissileListener* vpListener,float vSpeed)
:CMissile(vrSourcePosition,vpGraphName,vpListener,EXIST_TIME),
mPassMiddleTime(false),
mRatio(vRatio),
mTime(0.0f),
mPassTime(0.0f),
mSpeed(vSpeed),
mTargetPosition(),
mCenter(),	
mStartDegree(0.0f),
mRad(0.0f),
mSelfRad(vSelfRad/180*PI),
mR(0.0f),
mTargetEffectName()			
{
	mCurrentMatrix.RotateLX(PI/2);//�ض��Ĵ���ѷɻ���ƽ
	SetTargetPosition(vrTargetPosition);
}

void CCircleMissile::SetTargetPosition(RtgVertex3& vrTargetPosition)
{
	//����
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));


	mCenter.x=(mCurrentMatrix._30+vrTargetPosition.x)/2;					
	mCenter.y=(mCurrentMatrix._31+vrTargetPosition.y)/2;					
	mCenter.z=(mCurrentMatrix._32+vrTargetPosition.z)/2;					

	mR=Distance/2;							//�뾶
	if(mR==0.0f)
	{
		return;
	}

	//Բ�ܵĳ���
	Distance=Distance*PI;
	
	//������ʼ�Ƕ�
	float Temp1=acos((mCurrentMatrix._30-mCenter.x)/mR);
	float Temp2=asin((mCurrentMatrix._31-mCenter.y)/mR);

	if(Temp2>=0)//1��2����
	{
		mStartDegree=Temp1*180/PI;
	}
	else//3��4����
	{
		mStartDegree=(2*PI-Temp1)*180/PI;
	}
	
	if(mSpeed!=0.0f)
	{
		//�ƶ���ȥ��ʱ��
		mTime=Distance/mSpeed;
		mPassMiddleTime=false;
		
		if(mTime!=0.0f)
		{
			//���ٶ�
			mRad=360/mTime;		
		}
	}
	mTargetPosition=vrTargetPosition;
}

void CCircleMissile::DealMove(float vSecond)
{
	mTime-=vSecond;
	mPassTime+=vSecond;

	if(!mPassMiddleTime)
	{
		if(mTime<mPassTime)
		{
			mPassMiddleTime=true;
			//��Ŀ���ʱ���Ѿ�����//ֱ���赽Ŀ���
			mCurrentMatrix._30=mTargetPosition.x;
			mCurrentMatrix._31=mTargetPosition.y;
			mCurrentMatrix._32=mTargetPosition.z;
			//������Ŀ���
			Arrive();
		}
	}

	if(mTime<=0.0f)
	{
		DeleteSelf();
	}
	else 
	{//��Բ�Ĺ�ʽ�����ƶ�
		//�õ��켣
		//x'=x+A*cos(ang)
		//y'=y+B*sin(ang)
		//������ʼ�Ƕ���ת
		//x'=x*cos(ang)-y*sin(ang)
		//y'=x*sin(ang)+y*cos(ang) 

		float SinStartDegree=QuickSin(mStartDegree);
		float CosStartDegree=QuickCos(mStartDegree);
		float X;
		float Y;
		float X1;
		float Y1;

		X=mR*QuickCos(360-mRad*mPassTime);
		Y=mRatio*mR*QuickSin(360-mRad*mPassTime);

		X1=X*CosStartDegree-Y*SinStartDegree;
		Y1=X*SinStartDegree+Y*CosStartDegree;

		mCurrentMatrix._30=mCenter.x+ X1;
		mCurrentMatrix._31=mCenter.y+ Y1;

		mCurrentMatrix.RotateLY(-mSelfRad*vSecond);



		//mCurrentMatrix._32=mCenter.z+ 
		//mCurrentMatrix._32=mCenter.x+ mR*sin(mRad*vSecond);
	}
}


void CCircleMissile::OnRun(float vSecond)
{
	DealMove(vSecond);
	UpdateGraph();
}

void CCircleMissile::OnArrive()
{
	//����Ŀ�ĵ�ı�ը
	if(mTargetEffectName!="")
	{
		RT_NEW CBombMissile(mTargetPosition,(char*)mTargetEffectName.c_str());
	}
}

void CCircleMissile::SetTargetEffect(char vpEffectName[])				//����Ŀ�ĵص���Ч
{
	mTargetEffectName=vpEffectName;
}

