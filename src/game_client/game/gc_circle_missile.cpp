
#include "gc_include.h"
#include "gc_circle_missile.h"
#include "game_assistant.h"
#include "gc_bomb_missile.h"

const float EXIST_TIME=100.0;//存活时间

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
	mCurrentMatrix.RotateLX(PI/2);//特定的代码把飞环躺平
	SetTargetPosition(vrTargetPosition);
}

void CCircleMissile::SetTargetPosition(RtgVertex3& vrTargetPosition)
{
	//距离
	float Distance=sqrt(DistanceSquare(mCurrentMatrix._30,mCurrentMatrix._31,mCurrentMatrix._32,
		vrTargetPosition.x,vrTargetPosition.y,vrTargetPosition.z));


	mCenter.x=(mCurrentMatrix._30+vrTargetPosition.x)/2;					
	mCenter.y=(mCurrentMatrix._31+vrTargetPosition.y)/2;					
	mCenter.z=(mCurrentMatrix._32+vrTargetPosition.z)/2;					

	mR=Distance/2;							//半径
	if(mR==0.0f)
	{
		return;
	}

	//圆周的长度
	Distance=Distance*PI;
	
	//计算起始角度
	float Temp1=acos((mCurrentMatrix._30-mCenter.x)/mR);
	float Temp2=asin((mCurrentMatrix._31-mCenter.y)/mR);

	if(Temp2>=0)//1或2象限
	{
		mStartDegree=Temp1*180/PI;
	}
	else//3或4象限
	{
		mStartDegree=(2*PI-Temp1)*180/PI;
	}
	
	if(mSpeed!=0.0f)
	{
		//移动过去的时间
		mTime=Distance/mSpeed;
		mPassMiddleTime=false;
		
		if(mTime!=0.0f)
		{
			//角速度
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
			//到目标的时间已经过了//直接设到目标点
			mCurrentMatrix._30=mTargetPosition.x;
			mCurrentMatrix._31=mTargetPosition.y;
			mCurrentMatrix._32=mTargetPosition.z;
			//到达了目标点
			Arrive();
		}
	}

	if(mTime<=0.0f)
	{
		DeleteSelf();
	}
	else 
	{//用圆的公式处理移动
		//得到轨迹
		//x'=x+A*cos(ang)
		//y'=y+B*sin(ang)
		//根据起始角度旋转
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
	//到达目的点的爆炸
	if(mTargetEffectName!="")
	{
		RT_NEW CBombMissile(mTargetPosition,(char*)mTargetEffectName.c_str());
	}
}

void CCircleMissile::SetTargetEffect(char vpEffectName[])				//到达目的地的特效
{
	mTargetEffectName=vpEffectName;
}

