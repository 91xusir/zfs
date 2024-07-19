//
// Effect implementation file
//
#include "character2/actor.h"

RT_IMPLEMENT_DYNCREATE(CRT_EftInstEmitter, CRT_EffectInstance, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EftInstRibbon, CRT_EffectInstance, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EftInstBlur, CRT_EffectInstance, NULL, "")
RT_IMPLEMENT_DYNCREATE(CRT_EftInstProjector, CRT_EffectInstance, NULL, "")

// ----------------------------------------------------------------------------------
// Effect Emitter
// ----------------------------------------------------------------------------------
CRT_EftInstEmitter::CRT_EftInstEmitter()
{
	m_pActive = NULL;
	m_pDead   = NULL;
	m_actor   = NULL;
	m_parentMat = NULL;

	m_destMat = NULL;
	m_lastFrame = 0;                             
	m_modelClone = NULL;
	m_delta = 0;
}

CRT_EftInstEmitter::~CRT_EftInstEmitter()
{
    SParticle *p,*t;
	p = m_pActive;
	while(p)
	{
		t = p;
		p = t->next;
		DEL_ONE(t);
	}

	p = m_pDead;
	while(p)
	{
		t = p;
		p = t->next;
		DEL_ONE(t);
	}
}
void CRT_EftInstEmitter::InstanceInit() 
{
	CRT_EffectInstance::InstanceInit();
	m_eftEmitter = (CRT_EffectEmitter*)m_data;
}

void CRT_EftInstEmitter::InstanceExit() 
{
	m_renderDev->DeleteShader(&m_ro);
	if(m_modelClone)
	{
		ACTOR()->DeleteInstance(m_modelClone);
		m_modelClone = NULL;
	}
}

void CRT_EftInstEmitter::OnDataChange(DWORD p1, DWORD p2)
{
	m_emiSpeed = m_eftEmitter->m_emiSpeed;
	m_squirtNum = 0;
	m_bEnable = true;

	m_ro.iNumStage = 1;
    m_ro.Shaders[0].SetTexture(m_renderDev->m_Texture.CreateTexture(m_eftEmitter->m_texture.c_str(),RTGTBT_AUTO,true,1));
	m_ro.bTwoSide = true;
	m_ro.bColorKey = false;
	m_ro.bEnableBlendFunc = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.dwTextureFactor = 0xFFFFFFFF;
	m_ro.Shaders[0].bUseTextureMatrix = false;
	m_ro.bEnableBlendFunc = true;
	m_ro.Shaders[0].eColorOp   = RTGTOP_MODULATE;
	m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
	m_ro.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;
	m_ro.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
	m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
	switch(m_eftEmitter->m_alphaMode)
	{
		case ALPHA_BLEND:
			m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
			m_ro.eBlendDstFunc = RTGBLEND_INVSRCALPHA;
			m_ro.Shaders[0].eAlphaOp   = RTGTOP_MODULATE;
			m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;
			m_ro.Shaders[0].eAlphaArg2 = RTGTA_TEXTURE;
			break;

		case ALPHA_ADD:
			m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
			m_ro.eBlendDstFunc = RTGBLEND_ONE;
			break;

		case ALPHA_SUB:
			m_ro.eBlendSrcFunc = RTGBLEND_SRCALPHA;
			m_ro.eBlendDstFunc = RTGBLEND_ONE;
			break;

		case ALPHA_MODULATE:
			m_ro.eBlendSrcFunc = RTGBLEND_ZERO;
			m_ro.eBlendDstFunc = RTGBLEND_SRCCOLOR;
			break;
	}

	switch(m_eftEmitter->m_pathMode)
	{
		case PARTICLE_PATH_MODE_NORMAL:
			m_parentMat = m_actor->GetSlotMatrix(m_eftEmitter->m_parent.c_str());
			break;
		case PARTICLE_PATH_MODE_DEST:
			m_destMat = m_actor->GetSlotMatrix(m_eftEmitter->m_dest.c_str());
			break;
	}

	switch(m_eftEmitter->m_geometryMode)
	{
		case PARTICLE_GEOMETRY_BOARD:
			break;
		case PARTICLE_GEOMETRY_MODEL:
			break;
		case PARTICLE_GEOMETRY_MODEL_CLONE:
			if(m_modelClone)
			{
				ACTOR()->DeleteInstance(m_modelClone);
				m_modelClone = NULL;
			}
			m_modelClone = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT, 
				m_eftEmitter->m_model.c_str());
			if(m_modelClone)
			{
				m_modelClone->Create(m_renderDev);
				m_modelClone->PlayPose(m_eftEmitter->m_modelPose.c_str(), m_eftEmitter->m_bModelLoop);
			}
			break;
	}
}

bool CRT_EftInstEmitter::RequestTick()
{ 
	return true; 
}

bool CRT_EftInstEmitter::RequestUseFrame() 
{ 
	return m_eftEmitter->m_keyVec.Size() != 0;
}

bool CRT_EftInstEmitter::Init(RtgDevice *dev,CRT_ActorInstance *actor)
{
	m_renderDev = dev;
	m_actor = actor;
	m_delta = 0;
	OnDataChange(0,0);

	return true;
}

void CRT_EftInstEmitter::UseFrame(unsigned long frame ,unsigned long framestart)
{
	SEmitterKey k;
	if(m_eftEmitter->m_keyVec.GetKeyFrame(frame,k))
	{
		m_emiSpeed  = k.emiSpeed;
		m_bEnable   = k.bEnable;
	}

	// get squirt key	
	m_squirtNum = 0;
	if(m_eftEmitter->m_genMode == PARTICLE_GEN_MODE_SQUIRT && frame>m_lastFrame)
	{
		SEmitterKey *k = NULL;
		for(int i= Max((int)m_lastFrame,(int)framestart); i<frame; i++)	
		{
			k = m_eftEmitter->m_keyVec.GetKeyFrame(i);
			if(k) break;
		}
		if(k)
			m_squirtNum = k->emiSpeed;
	}
	m_lastFrame = frame;
}

void CRT_EftInstEmitter::Tick(unsigned long delta)
{
	// update key frame info
	
	m_delta += delta;

	RtgMatrix12 m;
	m.Unit();
	switch(m_eftEmitter->m_pathMode)
	{
		case PARTICLE_PATH_MODE_NORMAL:
			if(m_parentMat)
			{
				if(!m_eftEmitter->m_bUseParentCoor)
				{
					m = (*m_parentMat) * m_actor->m_finalMat;
				}
			}
			else
			{
			    if(!m_eftEmitter->m_bUseParentCoor)
					m = m_actor->m_finalMat;
			}
			break;

		case PARTICLE_PATH_MODE_DEST:
			if(m_destMat)
				m = (*m_destMat) * m_actor->m_finalMat;
			else 
				m = m_actor->m_finalMat;
			break;
	}
	
	// generate RT_NEW particle
	SParticle *t = NULL;
	int num = 0;
    if(!m_bEnable) m_delta = 0;
	if(m_eftEmitter->m_genMode == PARTICLE_GEN_MODE_SQUIRT)
	{
		num = m_squirtNum;
	}
	else 
	{
		num = m_delta/1000.0f*m_emiSpeed;
	}	

	if(num > 100) num = 100;  // max to 100
	if(num > 0 && m_bEnable)
	{
		m_delta = 0;
	    while(num--)
		{
			t = NewParticle();
			t->size  = m_eftEmitter->m_sizeMin + (m_eftEmitter->m_sizeMax-m_eftEmitter->m_sizeMin)*(rand()%100/100.0f);
			t->pos.x = m._30 + m_eftEmitter->m_emiSizeMin.x + (m_eftEmitter->m_emiSizeMax.x-m_eftEmitter->m_emiSizeMin.x)*(rand()%100/100.0f);
			t->pos.y = m._31 + m_eftEmitter->m_emiSizeMin.y + (m_eftEmitter->m_emiSizeMax.y-m_eftEmitter->m_emiSizeMin.y)*(rand()%100/100.0f);
			t->pos.z = m._32 + m_eftEmitter->m_emiSizeMin.z + (m_eftEmitter->m_emiSizeMax.z-m_eftEmitter->m_emiSizeMin.z)*(rand()%100/100.0f);

			switch(m_eftEmitter->m_geometryMode)
			{
				case PARTICLE_GEOMETRY_BOARD:
					break;
				case PARTICLE_GEOMETRY_MODEL:
					t->model = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT,m_eftEmitter->m_model.c_str());
					if(t->model)
					{
						t->model->Create(m_renderDev);
						t->model->PlayPose( m_eftEmitter->m_modelPose.c_str(), m_eftEmitter->m_bModelLoop);
						// force to use frame,update other info,like visible...
						SRT_Pose *p = t->model->GetCurrentPose();
						if(p)
							t->model->RealUseFrame(p->startFrm);
					}
					break;
				case PARTICLE_GEOMETRY_MODEL_CLONE:
					break;
			}

			switch(m_eftEmitter->m_pathMode)
			{
				case PARTICLE_PATH_MODE_NORMAL:
					t->speed.x = m_eftEmitter->m_speedMin.x + (m_eftEmitter->m_speedMax.x-m_eftEmitter->m_speedMin.x)*(rand()%100/100.0f);
					t->speed.y = m_eftEmitter->m_speedMin.y + (m_eftEmitter->m_speedMax.y-m_eftEmitter->m_speedMin.y)*(rand()%100/100.0f);
					t->speed.z = m_eftEmitter->m_speedMin.z + (m_eftEmitter->m_speedMax.z-m_eftEmitter->m_speedMin.z)*(rand()%100/100.0f);

					t->acc.x   = m_eftEmitter->m_accMin.x + (m_eftEmitter->m_accMax.x-m_eftEmitter->m_accMin.x)*(rand()%100/100.0f);
					t->acc.y   = m_eftEmitter->m_accMin.y + (m_eftEmitter->m_accMax.y-m_eftEmitter->m_accMin.y)*(rand()%100/100.0f);
					t->acc.z   = m_eftEmitter->m_accMin.z + (m_eftEmitter->m_accMax.z-m_eftEmitter->m_accMin.z)*(rand()%100/100.0f);
					if(m_eftEmitter->m_bUseParentDir)
						rtgV3MultiplyM9(t->speed.m_v,t->speed.m_v,m.m_m);
					break;

				case PARTICLE_PATH_MODE_DEST:
					t->acc.Zero();
					t->speed.x = (m._30-t->pos.x)/(m_eftEmitter->m_destTime/1000.0f);
					t->speed.y = (m._31-t->pos.y)/(m_eftEmitter->m_destTime/1000.0f);
					t->speed.z = (m._32-t->pos.z)/(m_eftEmitter->m_destTime/1000.0f);
					if(m_eftEmitter->m_destSpeedRand != 0)
					{
						float b = 1-(rand()%m_eftEmitter->m_destSpeedRand)/100.0f;
						t->speed.x *= b; 
						t->speed.y *= b;
						t->speed.z *= b;
					}
					break;
			}
			if(m_eftEmitter->m_bTexSub)
			{
				if(m_eftEmitter->m_bTexRand)
					t->texIdx = (rand()%m_eftEmitter->m_texUSub*m_eftEmitter->m_texVSub) + 1;
			}
			t->next = m_pActive;
			m_pActive = t;
		}
	}
    
	
	// update active particle
	SParticle *p = m_pActive,*prev = NULL;
	while(p)
	{
		// delete life over particle
		if( (p->life >= m_eftEmitter->m_parLife) || 
			(p->model && m_eftEmitter->m_autoDeleteModelPar && p->model->GetCurrentPose()==NULL)) 
		{
			if(prev)
				prev->next = p->next;
			else
				m_pActive = p->next;
			t = p;
			p = p->next;
			DelParticle(t);
			continue;
		}
		p->life += delta;
		p->speed += delta/1000.0f * p->acc;
		p->pos += delta/1000.0f * p->speed;
		p->dirSpeed = p->speed;

		for(int i=0; i<m_eftEmitter->m_forceVec.size(); i++)
		{
			Force(m_eftEmitter->m_forceVec[i], p, delta);
		}

		if(p->life <= m_eftEmitter->m_midVal*m_eftEmitter->m_parLife)
		{
			float b = p->life/(m_eftEmitter->m_midVal*m_eftEmitter->m_parLife); 
			p->opaque = m_eftEmitter->m_opaqueBeg + b*(m_eftEmitter->m_opaqueMid - m_eftEmitter->m_opaqueBeg);
			p->color  = m_eftEmitter->m_colorBeg  + b*(m_eftEmitter->m_colorMid  - m_eftEmitter->m_colorBeg);
			p->scale  = m_eftEmitter->m_scaleBeg  + b*(m_eftEmitter->m_scaleMid  - m_eftEmitter->m_scaleBeg);
			p->rotate = m_eftEmitter->m_rotateBeg + b*(m_eftEmitter->m_rotateMid - m_eftEmitter->m_rotateBeg);
			if(m_eftEmitter->m_bTexSub)
			{
				if(m_eftEmitter->m_bTexRand)
	                p->texIdx = rand()%(m_eftEmitter->m_texUSub*m_eftEmitter->m_texVSub) + 1;	// ???
				else
					p->texIdx = m_eftEmitter->m_texBeg + b*(m_eftEmitter->m_texMid - m_eftEmitter->m_texBeg);
			}
		}
		else
		{
			float b = (p->life-m_eftEmitter->m_midVal*m_eftEmitter->m_parLife)/((1-m_eftEmitter->m_midVal)*m_eftEmitter->m_parLife); 
			p->opaque = m_eftEmitter->m_opaqueMid + b*(m_eftEmitter->m_opaqueEnd - m_eftEmitter->m_opaqueMid);
			p->color  = m_eftEmitter->m_colorMid  + b*(m_eftEmitter->m_colorEnd  - m_eftEmitter->m_colorMid);
			p->scale  = m_eftEmitter->m_scaleMid  + b*(m_eftEmitter->m_scaleEnd  - m_eftEmitter->m_scaleMid);
			p->rotate = m_eftEmitter->m_rotateMid + b*(m_eftEmitter->m_rotateEnd - m_eftEmitter->m_rotateMid);
			if(m_eftEmitter->m_bTexSub)
			{
				if(m_eftEmitter->m_bTexRand)
					p->texIdx = (rand()%m_eftEmitter->m_texUSub*m_eftEmitter->m_texVSub) + 1; // ???
				else
					p->texIdx = m_eftEmitter->m_texMid + b*(m_eftEmitter->m_texEnd - m_eftEmitter->m_texMid);
			}
		}
		prev = p;
		p = p->next;
	}
}

void CRT_EftInstEmitter::RenderModel(SParticle *p, RtgMatrix16 &world)
{
	CRT_ActorInstance *act = NULL;
	switch(m_eftEmitter->m_geometryMode)
	{
		case PARTICLE_GEOMETRY_BOARD:
			break;
		case PARTICLE_GEOMETRY_MODEL:
			act = p->model;
			break;
		case PARTICLE_GEOMETRY_MODEL_CLONE:
			act = m_modelClone;
			break;
	}

	if(act)
	{
		act->SetVisible(p->opaque);
		// update world matrix
		world.Get4X3Matrix(&act->m_worldMat._00);
		// set finial matrix
		world.Get4X3Matrix(&act->m_finalMat._00);
		act->Render(m_renderDev,RTGRM_ALL,true);
	}
}

void CRT_EftInstEmitter::BeginRenderBoard(DWORD &light, DWORD &op)
{

}

void CRT_EftInstEmitter::Render(RtgDevice *dev,RTGRenderMask mask)
{
	RtgMatrix16 world,old;
	dev->GetMatrix(RTGTS_WORLD,&old);
    
	SParticle *p = m_pActive;
	RtgVertexVCT v[4];
	short idxMod[6] = {3,1,0,0,2,3};
	short idx[6];

	RtgVertex3 offset;
	DWORD sVB,sIB;
	BYTE *vb,*ib;
	unsigned int i;

	// don't use lighting on particle
	DWORD Light,op;
	if(m_eftEmitter->m_geometryMode == PARTICLE_GEOMETRY_BOARD)
	{
		Light = dev->GetRenderState(RTGRS_LIGHTING);
		dev->SetRenderState(RTGRS_LIGHTING,FALSE);
		dev->SetShader(&m_ro);
		dev->SetVertexFormat(RTG_VCT);
		
		if(m_eftEmitter->m_alphaMode == ALPHA_ADD)
			SetFogWithAdd(dev);
		else if(m_eftEmitter->m_alphaMode == ALPHA_SUB)
		{
			SetFogWithAdd(dev);
			dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_REVSUBTRACT);
		}

		op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
		dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
		dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
		if(m_eftEmitter->m_bAlwaysFront)
			dev->SetRenderState(RTGRS_Z_TEST,FALSE);
	}

	int batchNum = 100,cnt = 0;
	while(p)
	{
		world.Unit();
		world.Scale(p->scale.x,p->scale.y,p->scale.z);
		world.RotateLX(p->rotate.x);
		world.RotateLY(p->rotate.y);
		world.RotateLZ(p->rotate.z);

		RtgMatrix16 tmp;
		switch(m_eftEmitter->m_boardMode)
		{
			case PARTICLE_BILLBOARD_FULL:
				world = world * dev->m_pCamera->m_matBillboard;
				break;
			case PARTICLE_BILLBOARD_X:
				world.RotateY(DegreeToRadian(90));
				break;
			case PARTICLE_BILLBOARD_Y:
				world.RotateX(DegreeToRadian(90));
				break;
			case PARTICLE_BILLBOARD_Z:
				break;
			case PARTICLE_BILLBOARD_NONE:
				break;
			case PARTICLE_BILLBOARD_SPEEDX:
				tmp.Unit();
				// p->dirSpeed.Normalize();
				tmp.FaceTo(p->dirSpeed,RtgVertex3(1,0,0));
				world = world * tmp;
				break;
			case PARTICLE_BILLBOARD_SPEEDY:
				tmp.Unit();
				tmp.FaceTo(p->dirSpeed,RtgVertex3(0,1,0));
				world = world * tmp;
				break;
			case PARTICLE_BILLBOARD_SPEEDZ:
				tmp.Unit();
				tmp.FaceTo(p->dirSpeed,RtgVertex3(0,0,1));
				world = world * tmp;
				break;
		}

		world._30 = p->pos.x;
		world._31 = p->pos.y;
		world._32 = p->pos.z;

		switch(m_eftEmitter->m_pathMode)
		{
			case PARTICLE_PATH_MODE_NORMAL:
				if(m_eftEmitter->m_bUseParentCoor)
				{
                    RtgMatrix12 m;
					if(m_parentMat)
					{					
					    m = (*m_parentMat)*(m_actor->m_finalMat);								
					}
					else
					{
						m = m_actor->m_finalMat;
					}
					if( m_eftEmitter->m_boardMode == PARTICLE_BILLBOARD_FULL)
					{
						//只将位置信息传入变换矩阵处理
						//缩放有待优化
						RtgVertex3 vv(world._30,world._31,world._32);
						RtgVertex3 vv1(world._30 + 1,world._31,world._32);
						rtgV3MultiplyM12(vv,vv,m.m_m);
						rtgV3MultiplyM12(vv1,vv1,m.m_m);
						float scalef = vv1.Distance(vv);
	             
						RtgMatrix16 scale;
						scale.Unit();
						scale.Scale(scalef,scalef,scalef);
						world = scale*world;
						world._30 = vv.x;
						world._31 = vv.y;
						world._32 = vv.z;
					}
					else
					{										
						tmp.Unit();
						tmp.Set4X3Matrix(m);
						world = world * tmp;
					}
				}
				break;
		}
		if(m_eftEmitter->m_geometryMode != PARTICLE_GEOMETRY_BOARD)
		{
			RenderModel(p,world);
			p = p->next;
			continue;
		}

		if(cnt == 0)
		{
			vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,batchNum*4*sizeof(RtgVertexVCT),
				FALSE,sizeof(RtgVertexVCT));
			ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, batchNum*6*sizeof(short),
				FALSE, sizeof(short));
		}

		// prepare 4 vertex
		v[0].c = RtgVectorToColor(p->color,p->opaque);
		if(m_eftEmitter->m_bTexSub) 
		{
			int ui = p->texIdx % m_eftEmitter->m_texUSub;
			int vi = p->texIdx / m_eftEmitter->m_texVSub;
			v[0].t.x = ((float)ui) / m_eftEmitter->m_texUSub;
#ifdef FLIP_V
			v[0].t.y = ((float)vi) / m_eftEmitter->m_texVSub;
#else
			v[0].t.y = 1-((float)vi) / m_eftEmitter->m_texVSub;
#endif
		}
		else
		{
			v[0].t.x = 0.01;
#ifdef FLIP_V
			v[0].t.y = 0.01;
#else
			v[0].t.y = 0.99;
#endif
		}
		offset.Set(-p->size.x,p->size.y,0);
		if(m_eftEmitter->m_boardCenter == PARTICLE_BOARDCENTER_X0Y1)
		{
			offset.y += (p->size.y);
		}
		v[0].v = offset*world;

		v[1].c = v[0].c;
		if(m_eftEmitter->m_bTexSub)
		{
			int ui = p->texIdx % m_eftEmitter->m_texUSub + 1;
			int vi = p->texIdx / m_eftEmitter->m_texVSub;
			v[1].t.x = ((float)ui) / m_eftEmitter->m_texUSub;
#ifdef FLIP_V
			v[1].t.y = ((float)vi) / m_eftEmitter->m_texVSub;
#else
			v[1].t.y = 1-((float)vi) / m_eftEmitter->m_texVSub;
#endif
		}
		else
		{
			v[1].t.x = 0.99;
#ifdef FLIP_V
			v[1].t.y = 0.01;
#else
			v[1].t.y = 0.99;
#endif
		}
		offset.Set(p->size.x,p->size.y,0);
		if(m_eftEmitter->m_boardCenter == PARTICLE_BOARDCENTER_X0Y1)
		{
			offset.y += (p->size.y);
		}
		v[1].v = offset*world;

		v[2].c = v[0].c;
		if(m_eftEmitter->m_bTexSub)
		{
			int ui = p->texIdx % m_eftEmitter->m_texUSub;
			int vi = p->texIdx / m_eftEmitter->m_texVSub + 1;
			v[2].t.x = ((float)ui) / m_eftEmitter->m_texUSub;
#ifdef FLIP_V
			v[2].t.y = ((float)vi) / m_eftEmitter->m_texVSub;
#else
			v[2].t.y = 1-((float)vi) / m_eftEmitter->m_texVSub;
#endif
		}
		else
		{
			v[2].t.x = 0.01;
#ifdef FLIP_V
			v[2].t.y = 0.99;
#else
			v[2].t.y = 0.01;
#endif
		}
		offset.Set(-p->size.x,-p->size.y,0);
		if(m_eftEmitter->m_boardCenter == PARTICLE_BOARDCENTER_X0Y1)
		{
			offset.y += (p->size.y);
		}
		v[2].v = offset*world;

		v[3].c = v[0].c;
		if(m_eftEmitter->m_bTexSub)
		{
			int ui = p->texIdx % m_eftEmitter->m_texUSub + 1;
			int vi = p->texIdx / m_eftEmitter->m_texVSub + 1;
			v[3].t.x = ((float)ui) / m_eftEmitter->m_texUSub;
#ifdef FLIP_V
			v[3].t.y = ((float)vi) / m_eftEmitter->m_texVSub;
#else
			v[3].t.y = 1-((float)vi) / m_eftEmitter->m_texVSub;
#endif
		}
		else
		{
			v[3].t.x = 0.99;
#ifdef FLIP_V
			v[3].t.y = 0.99;
#else
			v[3].t.y = 0.01;
#endif
		}
		offset.Set(p->size.x,-p->size.y,0);
		if(m_eftEmitter->m_boardCenter == PARTICLE_BOARDCENTER_X0Y1)
		{
			offset.y += (p->size.y);
		}
		v[3].v = offset*world;

		for(i=0; i<4; i++)
		{
			memcpy(vb,&v[i],sizeof(RtgVertexVCT));
			vb += sizeof(RtgVertexVCT);
		}

		// prepare index
		for(i=0; i<6; i++)
			idx[i] = idxMod[i] + cnt*4;
		memcpy(ib,idx,6*sizeof(short));
		ib += 6*sizeof(short);
		cnt++;
		if(cnt == batchNum)
		{
			world.Unit();
			dev->m_pVB->EndUpload(sVB);
			dev->m_pVB->Bind(0, 0);
			dev->m_pIB->EndUpload(sIB);
			dev->m_pIB->Bind(0, 0);
			dev->SetMatrix(RTGTS_WORLD,&world);
			PFM()->actorRenderTri += cnt*2;
			PFM()->effectRenderTri += cnt*2;
			PFM()->particleRenderTri += cnt*2;
			dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,cnt*4,0,cnt*2);
			cnt = 0;
		}
		p = p->next;
	}

	if(cnt != 0)
	{
		world.Unit();
		dev->m_pVB->EndUpload(sVB);
		dev->m_pVB->Bind(0, 0);
		dev->m_pIB->EndUpload(sIB);
		dev->m_pIB->Bind(0, 0);
		dev->SetMatrix(RTGTS_WORLD,&world);
		PFM()->actorRenderTri += cnt*2;
		PFM()->effectRenderTri += cnt*2;
		PFM()->particleRenderTri += cnt*2;
		dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,cnt*4,0,cnt*2);
	}

	if(m_eftEmitter->m_geometryMode == PARTICLE_GEOMETRY_BOARD)
	{
		dev->RestoreShader(&m_ro);
		dev->SetRenderState(RTGRS_LIGHTING,Light);
		dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
		dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
		if(m_eftEmitter->m_bAlwaysFront)
			dev->SetRenderState(RTGRS_Z_TEST,TRUE);
		if(m_eftEmitter->m_alphaMode == ALPHA_ADD)
			RestoreFog(dev);
		else if(m_eftEmitter->m_alphaMode == ALPHA_SUB)
		{
			RestoreFog(dev);
			dev->SetRenderState(RTGRS_BLENDOP,RTGBOP_ADD);
		}
	}
}

void CRT_EftInstEmitter::DelParticle(SParticle *p)
{
	if(p->model)
		ACTOR()->DeleteInstance(p->model);
	p->model = NULL;
	p->next = m_pDead;
	m_pDead = p;
}

CRT_EftInstEmitter::SParticle *CRT_EftInstEmitter::NewParticle()
{
	SParticle *p;
	if(m_pDead)
	{
		p = m_pDead;
		m_pDead = m_pDead->next;
	}
	else
	{
		p = RT_NEW SParticle;
	}
	
	p->Init();
	return p;
}

void CRT_EftInstEmitter::Force(CRT_ForceField *ff, SParticle *p, long delta)
{
	switch(ff->m_ffType)
	{
		case FORCE_FIELD_VORTEX:
			CRT_FFVortex *vor = (CRT_FFVortex*)ff;
			float speed;
			speed = vor->m_speed;
			float r;
			r = vor->m_radius;

			RtgVertex3 dir;
			RtgVertex3 tmpp;
			tmpp = p->pos;
			tmpp.z = 0;
			dir = (RtgVertex3(0,0,0)-tmpp).Cross(RtgVertex3(0,0,-1));
			dir.Normalize();
			dir.x = -dir.x;
			dir.y = -dir.y;

			RtgVertex3 spd;
			spd = dir * speed;
			p->dirSpeed += spd;
			p->pos += delta/1000.0f*spd;

			dir = (RtgVertex3(0,0,0)-tmpp);
			dir.Normalize();
			dir *= (speed*speed/r);
			dir *= vor->m_centripetal;
			spd = delta/1000.0f*dir;
			p->dirSpeed += spd;
			p->pos += delta/1000.0f*spd;		
			break;
	}
}

// ------------------------------------------------------------------------------------
// Effect Ribbon
// ------------------------------------------------------------------------------------
CRT_EftInstRibbon::CRT_EftInstRibbon()
{
	m_lastFrame	= 0;
}

CRT_EftInstRibbon::~CRT_EftInstRibbon()
{

}

void CRT_EftInstRibbon::InstanceInit() 
{
	CRT_EffectInstance::InstanceInit();
	m_eftRibbon = (CRT_EffectRibbon*)m_data;
}

void CRT_EftInstRibbon::InstanceExit() 
{
	m_renderDev->DeleteShader(&m_ro);
	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		DelPiece(*it);
	}
	m_list.clear();
}

void CRT_EftInstRibbon::OnDataChange(DWORD p1, DWORD p2)
{
	m_ro.iNumStage = 1;
	
	if(m_eftRibbon->m_tex1.length() > 0)
		m_texId1 = m_renderDev->m_Texture.CreateTexture(m_eftRibbon->m_tex1.c_str(),RTGTBT_AUTO);
	else
		m_texId1 = 0;

	if(m_eftRibbon->m_tex2.length() > 0)
		m_texId2 = m_renderDev->m_Texture.CreateTexture(m_eftRibbon->m_tex2.c_str(),RTGTBT_AUTO);
	else
		m_texId2 = 0;

	m_ro.bTwoSide  = true;
	m_ro.bColorKey = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.dwTextureFactor = 0xFFFFFFFF;
	m_ro.Shaders[0].bUseTextureMatrix = false;
	m_ro.Shaders[0].eColorOp   = RTGTOP_MODULATE;
	m_ro.Shaders[0].eColorArg1 = RTGTA_TEXTURE;
	m_ro.Shaders[0].eColorArg2 = RTGTA_DIFFUSE;
	m_ro.Shaders[0].eAlphaOp   = RTGTOP_SELECTARG1;
	m_ro.Shaders[0].eAlphaArg1 = RTGTA_DIFFUSE;

	// get bone
	if(!m_actor) return;
	for(int i=0; i<m_actor->m_actor->m_boneVec.size(); i++)
	{
		SRT_Bone *tmp = &m_actor->m_actor->m_boneVec[i];
		if(tmp->name == m_eftRibbon->m_head.c_str())
		{
			m_headBone = tmp;
		}
		else if(tmp->name == m_eftRibbon->m_tail.c_str())
		{
			m_tailBone = tmp;
		}
	}
}

void CRT_EftInstRibbon::GenTri(int frame,SPiece *p)
{
	if(!m_actor) return;
	SRT_Bone *head=NULL,*tail=NULL;
	int find = 0;
	for(int i=0; i<m_actor->m_actor->m_boneVec.size() && find<2; i++)
	{
		SRT_Bone *tmp = &m_actor->m_actor->m_boneVec[i];
		if(tmp->name == m_eftRibbon->m_head.c_str())
		{
			head = tmp;
			find++;
		}
		else if(tmp->name == m_eftRibbon->m_tail.c_str())
		{
			tail = tmp;
			find++;
		}
	}
}

void CRT_EftInstRibbon::UseFrame(unsigned long frame,unsigned long framestart)
{
	if(frame == m_lastFrame) return;
	SRibbonKey *k = NULL;
	if(frame > m_lastFrame)
	{
		for(int i=m_lastFrame; i<frame; i++)	
		{
			k = m_eftRibbon->m_keyList.GetKeyFrame(i);
			if(k) break;
		}
	}
	
	if(k)
	{
		if(frame < k->endFrame)
		{
			// generate a RT_NEW ribbon
			SPiece *p   = NewPiece();
			p->keyFrame = *k;
			p->begFrame = frame;
			p->endFrame = k->endFrame;
			p->colorBeg = k->colorBeg;
			p->colorMid = k->colorMid;
			p->colorEnd = k->colorEnd;
			p->mid      = k->mid;
			p->visible = k->visible;
			// p->alphaMode = k->m_alphaMode;
			m_list.push_back(p);
		}
	}

	// update all piece here, not in tick
	list<SPiece*>::iterator it;
	if(frame < m_lastFrame)
	{
		list<SPiece*>::iterator it;
		for(it=m_list.begin(); it!=m_list.end(); it++)
		{
			DelPiece(*it);
		}
		m_list.clear();
	}

	if(frame > m_lastFrame && frame < m_lastFrame + 100)
	{
		for(int i=m_lastFrame; i<frame; i++)
		{
			for(it=m_list.begin(); it!=m_list.end();)
			{
				SPiece *p = *it;
				if(p->endFrame < i)
				{
					it = m_list.erase(it);
					DelPiece(p);
					continue;
				}

				// update color
				float rate = ((float)frame-p->begFrame)/(p->endFrame-p->begFrame);
				if(rate < p->mid)
                    p->color = p->colorBeg + rate/p->mid*(p->colorMid-p->colorBeg);
				else
					p->color = p->colorMid + (rate-p->mid)/(1-p->mid)*(p->colorEnd-p->colorMid);

				if(m_eftRibbon->m_bTexSub)
				{
					if(m_eftRibbon->m_bTexRand)
						p->subTex = (rand()%m_eftRibbon->m_texUSub*m_eftRibbon->m_texVSub) + 1;
					else
						p->subTex = (frame-p->begFrame)/m_eftRibbon->m_texSpeed;
				}
				it++;
			}
		}
	}
	m_lastFrame = frame;
}

bool CRT_EftInstRibbon::Init(RtgDevice *dev,CRT_ActorInstance *actor)
{
	m_renderDev = dev;
	m_actor = actor;

	OnDataChange(0,0);
	return true;
}

void CRT_EftInstRibbon::Tick(unsigned long delta)
{
	
}

void CRT_EftInstRibbon::Render(RtgDevice *dev,RTGRenderMask mask)
{
	if(m_list.empty()) return;
	if(!(m_headBone && m_tailBone)) return;

	RtgVertexVCT v;
	short idxMod[6] = {0,3,2, 0,1,3};

	RtgVertex3 offset;
	DWORD sVB;
	BYTE *vb;

	// don't use lighting on ribbon
	DWORD Light = dev->GetRenderState(RTGRS_LIGHTING);
	dev->SetRenderState(RTGRS_LIGHTING,FALSE);
	dev->SetVertexFormat(RTG_VCT);

	DWORD op;
	op = dev->GetRenderState(RTGRS_DIFFUSEMATERIALSOURCE);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,RTGMCS_COLOR1);
	dev->SetRenderState(RTGRS_Z_WRITE,FALSE);
	
	if(!m_eftRibbon->m_bZTest)
		dev->SetRenderState(RTGRS_Z_TEST,FALSE);

	list<SPiece*>::iterator it;
	for(it=m_list.begin(); it!=m_list.end(); it++)
	{
		SPiece *p = *it;
		
		// get visible vertex, > beginFrame && < fadeIn+life+fadeOut
		int delta = (p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut)*30/1000 + 1;
		if(delta > m_lastFrame-p->begFrame)
			delta = m_lastFrame-p->begFrame;

		int verNum = 0;
		for(int j=m_lastFrame-1; j>=m_lastFrame-delta; j--)
		{
			verNum += (m_headBone->ribbonFrame[j].segNum+1);
		}
		verNum *= 2;
		
		if(verNum == 0) continue;
		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,verNum*sizeof(RtgVertexVCT),
			FALSE,sizeof(RtgVertexVCT));

		// make vertex buffer
		for(int j=m_lastFrame-1; j>=m_lastFrame-delta; j--)
		{
			int seg = m_headBone->ribbonFrame[j].segNum;
			for(int k=seg; k>=0; k--)
			{
				// position
				m_headBone->ribbonFrame[j].pos[k].GetValue(v.v);
				
				// visilbe
				float validTime = (m_lastFrame-j)*1000.0f/30 - k*(1000.0f/30/(seg+1));

				// if global fadeout,get global visible
				float gvis = 1.0f;

				// if(j>p->keyFrame.fadeOutFrame) gvis = 0;
				if( j>p->keyFrame.fadeOutFrame && p->endFrame-p->keyFrame.fadeOutFrame>0)
				{
					gvis = 1 - ((1.0f*j-p->keyFrame.fadeOutFrame)*(seg+1)+k)/((p->endFrame - p->keyFrame.fadeOutFrame)*(seg+1));			
				}
                
				float visible = 0;	
				if(validTime < 0)
				{
					visible = 0;
				}
				else if(validTime < p->keyFrame.fadeIn)
				{
					visible = validTime/p->keyFrame.fadeIn;
				}
				else if(validTime < p->keyFrame.fadeIn + p->keyFrame.life)
				{
					visible = 1;
				}
				else if(validTime < p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut)
				{
					visible = 1 - ((validTime - p->keyFrame.fadeIn - p->keyFrame.life)/p->keyFrame.fadeOut);
				}
				visible *= gvis;
				v.c = RtgVectorToColor(p->color,visible*p->visible);
			    
				// texture uv
				v.t.x = 0.01;
				v.t.y = ( (m_lastFrame-j)*1000.0f/30 - k*(1000.f/30/(seg+1))) 
					/ (p->keyFrame.fadeIn + p->keyFrame.life + p->keyFrame.fadeOut);
				if(v.t.y < 0) v.t.y = 0;
				if(v.t.y > 1) v.t.y = 1;
				if(m_eftRibbon->m_bTexSub)
				{
					int ui = p->subTex % m_eftRibbon->m_texUSub;
					int vi = p->subTex / m_eftRibbon->m_texVSub;
					v.t.x = ((float)ui) / m_eftRibbon->m_texUSub;
					v.t.y = ((float)vi) / m_eftRibbon->m_texVSub + v.t.y / m_eftRibbon->m_texVSub;
				}

#ifdef FLIP_V
				v.t.y = 1-v.t.y;
#endif
				// copy to buffer
				memcpy(vb,&v,sizeof(RtgVertexVCT));
				vb += sizeof(RtgVertexVCT);
				// LOG3("uv = %f,%f,visible = %f\n",v.t.x,v.t.y,visible);

				// another vertex
				m_tailBone->ribbonFrame[j].pos[k].GetValue(v.v);
				v.t.x = 0.99f;
				if(m_eftRibbon->m_bTexSub)
				{
					int ui = p->subTex % m_eftRibbon->m_texUSub + 1;
					v.t.x = ((float)ui) / m_eftRibbon->m_texUSub;
				}
				memcpy(vb,&v,sizeof(RtgVertexVCT));
				vb += sizeof(RtgVertexVCT);
				// LOG3("uv = %f,%f,visible = %f\n",v.t.x,v.t.y,visible);
			}
		}

		RtgMatrix16 world;
		world.Unit();
		world.Set4X3Matrix(m_actor->m_finalMat.m_m);

		dev->m_pVB->EndUpload(sVB);
		dev->m_pVB->Bind(0, 0);
		dev->SetMatrix(RTGTS_WORLD,&world);

		if(m_eftRibbon->m_alphaMode1 != ALPHA_NULL)
			m_ro.bEnableBlendFunc = true;
		else
			m_ro.bEnableBlendFunc = false;
		SetupAlphaMode(m_eftRibbon->m_alphaMode1, m_ro, dev, 1.0);
		m_ro.Shaders[0].SetTexture(m_texId1);
		dev->SetShader(&m_ro);

		PFM()->actorRenderTri += verNum-2;
		PFM()->effectRenderTri += verNum-2;
		PFM()->ribbonRenderTri += verNum-2;
		dev->DrawPrimitiveVB(RTG_TRIANGLE_STRIP,0,verNum,verNum-2);
		RestoreAlphaMode(m_eftRibbon->m_alphaMode1, m_ro, dev, 1.0);
		if(m_texId2)
		{
			if(m_eftRibbon->m_alphaMode2 != ALPHA_NULL)
				m_ro.bEnableBlendFunc = true;
			else
				m_ro.bEnableBlendFunc = false;
			SetupAlphaMode(m_eftRibbon->m_alphaMode2, m_ro, dev, 1.0);
			m_ro.Shaders[0].SetTexture(m_texId2);
			dev->SetShader(&m_ro);
			PFM()->actorRenderTri  += verNum-2;
			PFM()->effectRenderTri += verNum-2;
			PFM()->ribbonRenderTri += verNum-2;
			dev->DrawPrimitiveVB(RTG_TRIANGLE_STRIP,0,verNum,verNum-2);
			RestoreAlphaMode(m_eftRibbon->m_alphaMode2, m_ro, dev, 1.0);
		}
	}	

	if(!m_eftRibbon->m_bZTest)
		dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	dev->RestoreShader(&m_ro);
	dev->SetRenderState(RTGRS_LIGHTING,Light);
	dev->SetRenderState(RTGRS_DIFFUSEMATERIALSOURCE,op);
	dev->SetRenderState(RTGRS_Z_WRITE,TRUE);
}

// ------------------------------------------------------------------------------------
// Effect Blur
// ------------------------------------------------------------------------------------
CRT_EftInstBlur::CRT_EftInstBlur()
{
	m_delta = 0;
	m_actor = NULL;
	m_blurAct = NULL;
	m_bEnable = true;
	m_frameDelta = 0;
}

CRT_EftInstBlur::~CRT_EftInstBlur()
{

}

void CRT_EftInstBlur::InstanceInit() 
{
	CRT_EffectInstance::InstanceInit();
	m_eftBlur = (CRT_EffectBlur*)m_data;
}

void CRT_EftInstBlur::InstanceExit() 
{
	if(m_blurAct)
	{
		ACTOR()->DeleteInstance(m_blurAct);
		m_blurAct = NULL;
	}
}

void CRT_EftInstBlur::OnDataChange(DWORD p1, DWORD p2)
{
	if(m_blurAct)
	{
		ACTOR()->DeleteInstance(m_blurAct);
		m_blurAct = NULL;
	}
	if(m_eftBlur->m_blurModel.size() > 0)
	{
		m_blurAct = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT,m_eftBlur->m_blurModel.c_str());
		if(m_blurAct)
		   m_blurAct->Create(m_renderDev);
	}
}

bool CRT_EftInstBlur::Init(RtgDevice *dev,CRT_ActorInstance *actor)
{
    m_actor = actor;
	m_renderDev = dev;
	OnDataChange(0,0);
	
	return true;
}

void CRT_EftInstBlur::Tick(unsigned long delta)
{
	m_frameDelta += delta;
	list<SFrame>::iterator it;
	for(it=m_frameList.begin(); it!=m_frameList.end(); )
	{
		SFrame *tmp = &(*it);
		tmp->time += delta;
		if(tmp->time >= m_eftBlur->m_life)
		{
			it=m_frameList.erase(it);
			continue;
		}
		it++;
	}

	if(!m_actor || !m_bEnable) return;
	// check if update frame
	if(m_frameDelta>m_eftBlur->m_blurInterval)
	{
		m_frameDelta = 0;
		SFrame tmp;
		tmp.matrix.Set4X3Matrix(m_actor->m_finalMat);
		tmp.frame = m_actor->GetCurFrame();
		tmp.time = 0;
		m_frameList.push_back(tmp);
	}
}

void CRT_EftInstBlur::UseFrame(unsigned long frame,unsigned long framestart)
{
	SBlurKey key;
	if(!m_eftBlur->m_keyVec.GetKeyFrame(frame,key))
		return;
	Update(key);
}

void CRT_EftInstBlur::Update(SBlurKey &key)
{
	m_bEnable = key.bEnable;
}

void CRT_EftInstBlur::Render(RtgDevice *dev,RTGRenderMask mask)
{
	if(!m_bEnable) return;
	if(m_eftBlur->m_bAlwaysFront)
		dev->SetRenderState(RTGRS_Z_TEST,FALSE);

	if(!m_blurAct)
	{
		// backup actor info
		float oldVis = m_actor->GetVisible();
		// RtgMatrix12 oldMat = *(m_actor->m_finalMat);

		list<SFrame>::iterator it;
		for(it=m_frameList.begin(); it!=m_frameList.end(); it++)
		{
			SFrame *tmp = &(*it);
			m_actor->SetVisible(m_eftBlur->m_visibleBeg+(float)tmp->time/m_eftBlur->m_life*
				(m_eftBlur->m_visibleEnd-m_eftBlur->m_visibleBeg));
			tmp->matrix.Get4X3Matrix(&m_actor->m_finalMat._00);
			m_actor->Render(dev,RTGRM_ALL,false);
		}

		m_actor->SetVisible(oldVis);
		// *(m_actor->GetWorldMatrix()) = oldMat;

		if(m_eftBlur->m_bAlwaysFront)
			dev->SetRenderState(RTGRS_Z_TEST,TRUE);
	}
	else
	{
		list<SFrame>::iterator it;
		for(it=m_frameList.begin(); it!=m_frameList.end(); it++)
		{
			SFrame *tmp = &(*it);
			m_blurAct->SetVisible(m_eftBlur->m_visibleBeg+(float)tmp->time/m_eftBlur->m_life*
				(m_eftBlur->m_visibleEnd-m_eftBlur->m_visibleBeg));
			tmp->matrix.Get4X3Matrix(&m_blurAct->m_finalMat._00);
			m_blurAct->RealUseFrame(tmp->frame);
			m_blurAct->Render(dev,RTGRM_ALL,false);
		}
	}
}

// ------------------------------------------------------------------------------------
// Effect Projector
// ------------------------------------------------------------------------------------
CRT_EftInstProjector::CRT_EftInstProjector()
{
	m_delta = 0;
	m_actor = NULL;
	m_stageProject = -1;
	m_stageGeo = -1;
	m_stageFadeOut = -1;
}

CRT_EftInstProjector::~CRT_EftInstProjector()
{

}

void CRT_EftInstProjector::InstanceInit() 
{
	CRT_EffectInstance::InstanceInit();
	m_eftProjector = (CRT_EffectProjector*)m_data;
}

void CRT_EftInstProjector::InstanceExit() 
{

}

// project tex
// fade out tex
// geometry tex
// *blend color and visible
void CRT_EftInstProjector::OnDataChange(DWORD p1, DWORD p2)
{
	m_stageProject = -1;
	m_stageGeo = -1;
	m_stageFadeOut = -1;

	m_camera.SetProjParams(DegreeToRadian(m_eftProjector->m_fov), m_eftProjector->m_aspect, 
		m_eftProjector->m_near, m_eftProjector->m_far);
	
	RtgMatrix16 view,tmp;
	view.Unit();
	view.Translate(m_eftProjector->m_offsetPos.x,m_eftProjector->m_offsetPos.y,m_eftProjector->m_offsetPos.z);
	
	tmp.Unit();
	tmp.RotateLX(m_eftProjector->m_offsetRotate.x);
	tmp.RotateLY(m_eftProjector->m_offsetRotate.y);
	tmp.RotateLZ(m_eftProjector->m_offsetRotate.z);
	view = tmp*view;

	m_matrix = m_actor->GetSlotMatrix(m_eftProjector->m_parent.c_str());
	if(m_matrix)
	{
		tmp.Set4X3Matrix(&m_matrix->_00);
		view = view * tmp;
		tmp.Set4X3Matrix(&m_actor->m_finalMat._00);
		view = view * tmp;
	}
	
	m_camera.SetViewMatrix(view.Inverse());
	
	// m_matrix = m_actor->GetSlotMatrix(m_eftProjector->m_parent.c_str());
	m_ro.bTwoSide = true;
	m_ro.bColorKey = false;
	m_ro.bEnableBlendFunc = false;
	m_ro.bEnableAlphaTest = false;
	m_ro.dwTextureFactor = 0xFFFFFFFF;

	m_ro.iNumStage = 0;
	if(m_eftProjector->m_tex.length() > 0)
	{
		m_stageProject = m_ro.iNumStage;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(m_renderDev->m_Texture.CreateTexture(m_eftProjector->m_tex.c_str(),RTGTBT_AUTO,true,1));
		m_ro.Shaders[m_ro.iNumStage].eColorOp   = RTGTOP_MODULATE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TFACTOR;
		m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TFACTOR;
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = true;
		m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;
		m_ro.Shaders[m_ro.iNumStage].dwTexTransformFlag = RTGTTF_COUNT4|RTGTTF_PROJECTED;
		m_ro.iNumStage++;
	}
	
	if(m_eftProjector->m_texFadeOut.length() > 0)
	{
		m_stageFadeOut = m_ro.iNumStage;
		m_ro.Shaders[m_ro.iNumStage].SetTexture(m_renderDev->m_Texture.CreateTexture(m_eftProjector->m_texFadeOut.c_str(),RTGTBT_AUTO,true,1));
		m_ro.Shaders[m_ro.iNumStage].eColorOp   = RTGTOP_MODULATE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_CURRENT;
		m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_TEXTURE;
		m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TFACTOR;
		m_ro.Shaders[m_ro.iNumStage].bUseTextureMatrix = true;
		m_ro.Shaders[m_ro.iNumStage].eTexCoordIndex = RTGTC_CAMERASPACEPOSITION;
		m_ro.Shaders[m_ro.iNumStage].dwTexTransformFlag = RTGTTF_COUNT4|RTGTTF_PROJECTED;
		m_ro.iNumStage++;
	}

	if(m_eftProjector->m_bCullBackFaces)
	{
		m_ro.Shaders[m_ro.iNumStage].eColorOp   = RTGTOP_MODULATE;
		m_ro.Shaders[m_ro.iNumStage].eColorArg1 = RTGTA_CURRENT;
		m_ro.Shaders[m_ro.iNumStage].eColorArg2 = RTGTA_DIFFUSE;
		m_ro.Shaders[m_ro.iNumStage].eAlphaOp   = RTGTOP_SELECTARG1;
		m_ro.Shaders[m_ro.iNumStage].eAlphaArg1 = RTGTA_TFACTOR;
		m_ro.iNumStage++;
	}
	m_stageGeo = m_ro.iNumStage;
}

bool CRT_EftInstProjector::Init(RtgDevice *dev,CRT_ActorInstance *actor)
{
	m_actor = actor;
	m_renderDev = dev;
	OnDataChange(0,0);

	return true;
}

void CRT_EftInstProjector::Tick(unsigned long delta)
{
	RtgMatrix16 view,tmp;
	view.Unit();
	view.Translate(m_eftProjector->m_offsetPos.x,m_eftProjector->m_offsetPos.y,m_eftProjector->m_offsetPos.z);

	tmp.Unit();
	tmp.RotateLX(m_eftProjector->m_offsetRotate.x);
	tmp.RotateLY(m_eftProjector->m_offsetRotate.y);
	tmp.RotateLZ(m_eftProjector->m_offsetRotate.z);
	view = tmp*view;

	// m_matrix = m_actor->GetSlotMatrix(m_eftProjector->m_parent.c_str());
	if(m_matrix)
	{
		tmp.Set4X3Matrix(&m_matrix->_00);
		view = view * tmp;
		tmp.Set4X3Matrix(&m_actor->m_finalMat._00);
		view = view * tmp;
	}
	m_camera.SetViewMatrix(view.Inverse());

	m_verVec.clear();
	m_idxVec.clear();
	
	RtgVertexVNT v3;
	float seg = 100;
	v3.v.Set(-seg,seg,15);	m_verVec.push_back(v3);
	v3.v.Set(0,seg,15);		m_verVec.push_back(v3);
	v3.v.Set(seg,seg,15);	m_verVec.push_back(v3);

	v3.v.Set(-seg,0,5);		m_verVec.push_back(v3);
	v3.v.Set(0,0,5);		m_verVec.push_back(v3);
	v3.v.Set(seg,0,15);		m_verVec.push_back(v3);

	v3.v.Set(-seg,-seg,25);	m_verVec.push_back(v3);
	v3.v.Set(0,-seg,5);		m_verVec.push_back(v3);
	v3.v.Set(seg,-seg,25);	m_verVec.push_back(v3);

	short idx[24] = {0,3,4, 0,4,1, 1,4,5, 1,5,2, 3,6,7, 3,7,4, 4,7,8, 4,8,5};
	for(int i=0; i<24; i++)
		m_idxVec.push_back(idx[i]);

	m_delta += delta;
}

void CRT_EftInstProjector::UseFrame(unsigned long frame,unsigned long framestart)
{
	SProjectorKey key;
	if(!m_eftProjector->m_keyVec.GetKeyFrame(frame,key))
		return;
	Update(key);
}

void CRT_EftInstProjector::Update(SProjectorKey &key)
{
	m_key = key;
}

void CRT_EftInstProjector::Render(RtgDevice *dev,RTGRenderMask mask)
{
	RtgMatrix16 uv,fm;
	uv.Unit();
	uv = uv * m_camera.GetViewMatrix();
	uv = uv * m_camera.GetProjMatrix();
	fm.Unit();
	fm = fm * m_camera.GetViewMatrix();
	fm = fm * m_camera.GetProjMatrix();

	RtgMatrix16 viewport;
	viewport.Unit();
	viewport._00 = 0.5;
#ifdef FLIP_V
	viewport._11 = -0.5;
#else 
	viewport._11 = 0.5;
#endif
	viewport._22 = 0.5;
	viewport._30 = 0.5;
	viewport._31 = 0.5;
	viewport._32 = 0.5;
	
	RtgMatrix16 tex;
	tex.Unit();
	tex.Translate(m_key.stage.translate.x,m_key.stage.translate.y,m_key.stage.translate.z);
	tex.RotateLZ(m_key.stage.rotate.x);
	tex.Scale(m_key.stage.scale.x,m_key.stage.scale.y,m_key.stage.scale.z);
	
	uv = dev->m_pCamera->GetViewMatrix().Inverse() * uv * tex * viewport;
	fm = dev->m_pCamera->GetViewMatrix().Inverse() * fm * viewport;
	if(m_stageProject != -1)
		m_ro.Shaders[m_stageProject].mMatrix16 = uv;
	if(m_stageFadeOut != -1)	
		m_ro.Shaders[m_stageFadeOut].mMatrix16 = fm;

	// clip terrian and actor
	UploadVB(dev);
	UploadIB(dev);

	m_ro.dwTextureFactor = RtgVectorToColor(m_key.color, m_key.ro.visible);
	if(m_eftProjector->m_bTexClampMode)
		dev->SetTextureAddress(RTGTADD_CLAMP);

	//
	// draw terrain
	//
	RtgMatrix16 m;
	m.Unit();
	dev->SetMatrix(RTGTS_WORLD, &m);
	SetupAlphaMode(m_eftProjector->m_fo.alphaMode, m_ro, m_renderDev, 1.0);
	dev->SetShader(&m_ro);
	PFM()->actorRenderTri   += m_idxVec.size()/3;
	PFM()->effectRenderTri  += m_idxVec.size()/3;
	PFM()->projectRenderTri += m_idxVec.size()/3;
	dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,m_verVec.size(),0,m_idxVec.size()/3);

	//
	// draw actor
	//
	m.Set4X3Matrix(&m_actor->m_finalMat._00);
	dev->SetMatrix(RTGTS_WORLD, &m);
	
	if(m_eftProjector->m_bCullBackFaces)
	{
		RtgLight light;
		light.eMode = RTG_LIGHT_DIRECTION;
		light.vAmbient.Zero();
		light.vSpecular.Zero();
		light.vDiffuse.Set(1,1,1);
		m_camera.GetViewMatrix().GetAxis(2,light.vDirection);
		light.vDirection = RtgVertex3(0,0,0) - light.vDirection;
		light.vDirection.Normalize();
		// RtCoreLog().Info("%f,%f,%f\n", light.vDirection.x, light.vDirection.y, light.vDirection.z);
		light.vPosition.x = m_camera.GetViewMatrix()._30;
		light.vPosition.y = m_camera.GetViewMatrix()._31;
		light.vPosition.z = m_camera.GetViewMatrix()._32;
		light.fRange = 10000.f;
		dev->SetLight(0,&light);
		// dev->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING,RtgVectorToColor(RtgVertex3(0,0,0),1));
		dev->SetMaterial(RtgVertex4(1,1,1,0), RtgVertex4(0,0,0,0), RtgVertex4(0,0,0,0),
			RtgVertex4(0,0,0,0), 0);
	}
	
	if(m_eftProjector->m_bProjectSelf)
		RenderProjectActor(dev,m_actor);

	dev->RestoreShader(&m_ro);
	// recover render option
	RestoreAlphaMode(m_eftProjector->m_fo.alphaMode, m_ro, dev, 1.0);

	// recover
	if(m_eftProjector->m_bTexClampMode)
		dev->SetTextureAddress(RTGTADD_WRAP);

	if(ACTOR()->m_bRenderProjector)
        DrawHelperObject(dev);
}

void CRT_EftInstProjector::RenderProjectActor(RtgDevice *dev, CRT_ActorInstance *actor)
{
	for(int i=0; i<actor->m_skinVec.size(); i++)
	{
		vector<SRT_StaticVtx> &verVec = actor->m_skinVec[i]->m_verVec;

		DWORD sVB;
		BYTE *vb;
		vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,verVec.size()*sizeof(RtgVertexVNT),
			FALSE,sizeof(RtgVertexVNT));
		for(int k=0; k<verVec.size(); k++)
		{
			memcpy(vb,&verVec[k].pos,sizeof(RtgVertexVNT));
			vb += sizeof(RtgVertexVNT);
		}
		dev->SetVertexFormat(RTG_VNT);
		dev->m_pVB->EndUpload(sVB);
		dev->m_pVB->Bind(0, 0);

		for(int j=0; j<actor->m_skinVec[i]->m_skin->m_rsVec.size(); j++)
		{
			DWORD sIB;
			BYTE *ib;
			vector<short> &idxVec = m_actor->m_skinVec[i]->m_skin->m_rsVec[j].idxVec;

			ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, idxVec.size()*sizeof(short),
				FALSE, sizeof(short));
			memcpy(ib, &idxVec[0],idxVec.size()*sizeof(short));
			dev->m_pIB->EndUpload(sIB);
			dev->m_pIB->Bind(0, 0);

			PFM()->actorRenderTri   += idxVec.size()/3;
			PFM()->effectRenderTri  += idxVec.size()/3;
			PFM()->projectRenderTri += idxVec.size()/3;
			dev->DrawIndexPrimitiveVB(RTG_TRIANGLES,0,verVec.size(),0,idxVec.size()/3);
		}
	}
}

void CRT_EftInstProjector::DrawHelperObject(RtgDevice *dev)
{
	//
	// draw projector self
	//
	RtgMatrix16 m;
	m.Unit();
	dev->SetMatrix(RTGTS_WORLD, &m);

	// dev->SetRenderState(RTGRS_Z_TEST, FALSE);
	dev->SetRenderState(RTGRS_LIGHTING, FALSE);
	DWORD color = RtgVectorToColor(RtgVertex3(0.5,0.5,1),1);

	RtgMatrix16 cam = m_camera.GetViewMatrix().Inverse();
	dev->DrawBox(RtgVertex3(5,5,5), cam, &color);
	dev->DrawAxis(cam, 10);

	color = RtgVectorToColor(RtgVertex3(0,1,0),1);
	RtgVertex3 vfar[4],vnear[4];

	cam.Unit();
	cam._32 = -m_eftProjector->m_far;
	cam = cam * m_camera.GetViewMatrix().Inverse();

	float r;
	r = m_eftProjector->m_far*tan(DegreeToRadian(m_eftProjector->m_fov/2));
	dev->DrawBox(RtgVertex3(r,r/m_eftProjector->m_aspect,0), cam, &color);

	vfar[0].Set(-r,r/m_eftProjector->m_aspect,0);
	vfar[1].Set(r,r/m_eftProjector->m_aspect,0);
	vfar[2].Set(-r,-r/m_eftProjector->m_aspect,0);
	vfar[3].Set(r,-r/m_eftProjector->m_aspect,0);
	for(int i=0; i<4; i++)
		vfar[i] = vfar[i]*cam;

	cam.Unit();
	cam._32 = -m_eftProjector->m_near;
	cam = cam * m_camera.GetViewMatrix().Inverse();
	r = m_eftProjector->m_near*tan(DegreeToRadian(m_eftProjector->m_fov/2));
	dev->DrawBox(RtgVertex3(r,r/m_eftProjector->m_aspect,0), cam, &color);

	vnear[0].Set(-r,r/m_eftProjector->m_aspect,0);
	vnear[1].Set(r,r/m_eftProjector->m_aspect,0);
	vnear[2].Set(-r,-r/m_eftProjector->m_aspect,0);
	vnear[3].Set(r,-r/m_eftProjector->m_aspect,0);
	for(int i=0; i<4; i++)
		vnear[i] = vnear[i]*cam;

	for(int i=0; i<4; i++)
		dev->DrawLine(vnear[i],vfar[i],&color);

	// dev->SetRenderState(RTGRS_Z_TEST, TRUE);
	dev->SetRenderState(RTGRS_LIGHTING, TRUE);
}

bool CRT_EftInstProjector::UploadVB(RtgDevice *dev)
{
	DWORD sVB;
	BYTE *vb;

	vb = (BYTE*)dev->m_pVB->BeginUpload(sVB,m_verVec.size()*sizeof(RtgVertexVNT),
		FALSE,sizeof(RtgVertexVNT));
	for(int k=0; k<m_verVec.size(); k++)
	{
		memcpy(vb,&m_verVec[k],sizeof(RtgVertexVNT));
		vb += sizeof(RtgVertexVNT);
	}
	dev->SetVertexFormat(RTG_VNT);

	dev->m_pVB->EndUpload(sVB);
	dev->m_pVB->Bind(0, 0);
	return true;
}

bool CRT_EftInstProjector::UploadIB(RtgDevice *dev)
{
	DWORD sIB;
	BYTE *ib;

	ib = (BYTE*)dev->m_pIB->BeginUpload(sIB, m_idxVec.size()*sizeof(short),
		FALSE, sizeof(short));
	memcpy(ib, &m_idxVec[0],m_idxVec.size()*sizeof(short));
	dev->m_pIB->EndUpload(sIB);
	dev->m_pIB->Bind(0, 0);
	return true;
}
