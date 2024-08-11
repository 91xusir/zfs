#include "stdafx.h"
#include "tool_character.h"
#include "MainFrm.h"
#include "EditorFrame.h"
#include <MMSystem.h>
#include "graph/rt_graph.h"
using namespace std;
#include "character/actor.h"
#include "audio/rt_audio.h"
#include "tool_characterDoc.h"
#include "tool_characterView.h"

bool g_bRenderShadow = false;
bool g_bRenderFrame = false;
bool g_bRenderCoor = true;
bool g_bRenderFloor = true;
bool g_bRenderInfo = true;
bool g_bRenderWire = false;
RtgVertex3 CameraEye,CameraLookAt;
RtgVertex3 CameraTrans,CameraRot;
RtgMatrix16 CameraMatrix,CamMatRot;
float CameraForward;


DWORD shadowFloat = 0;
class CEditScene : public RtsSceneUtil
{
public:
	CEditScene() {}
	virtual ~CEditScene() {}

	virtual RtgLight* CreateDynamicLight()
	{
		RtgLight *l = new RtgLight;
		m_lightList.push_back(l);
		return l;
	}

	virtual void UpdateDynamicLight(RtgLight* pLight) 
	{
	
	}
	
	virtual void DestroyDynamicLight(RtgLight* pLight)
	{
		vector<RtgLight*>::iterator it;
		for(it=m_lightList.begin(); it!=m_lightList.end(); it++)
		{
			if(*it == pLight)
				m_lightList.erase(it);
			return;
		}
	}

	vector<RtgLight*> m_lightList;
};

CEditScene g_editScene;

RT_IMPLEMENT_DYNCREATE(CEditorFrame, CRtgAppFrame, NULL, "")
CEditorFrame::CEditorFrame()
{
    m_iCameraMode = 0;
    m_iFillTerrain = 0;
    m_iCameraPos.Set(0.f, 0.f);

    m_bShowBoundBox = false;
	m_fogColor.Set(0,0,0);
	m_clearColor.Set(0.3,0.3,0.3);
	m_bFog = false;
	m_fogNear = 300;
	m_fogFar  = 1000;
	m_bShowBoundBox = false;
	m_actorScale.Set(1.0);
	m_bCameraLight = false;
}

CEditorFrame::~CEditorFrame()
{

}


bool CEditorFrame::OnFrameInit()
{
	rtSoundInit(NULL,(HWND)this->GetWndHandle());
	g_pSoundMgr->SetAudioPath("audio");

//	CreateEngineSystemChar();
	RtIni ini;
	ActorInit();
//	ACTOR()->Init(ini);

//	ACTOR()->m_bRenderProjector = true;

	// for client
	// begin (Modified by Wayne Wong 2010-11-05)
	//ActorRegisterPath("creature\\actor\\");
	//ActorRegisterPath("creature\\material\\");
	//ActorRegisterPath("creature\\effect\\");

	//ActorRegisterPath("scene\\actor\\");
	//ActorRegisterPath("scene\\material\\");
	//ActorRegisterPath("scene\\effect\\");

	RtcGetActorManager()->AddSearchPath("creature\\actor\\");
	RtcGetActorManager()->AddSearchPath("creature\\material\\");
	RtcGetActorManager()->AddSearchPath("creature\\effect\\");

	RtcGetActorManager()->AddSearchPath("scene\\actor\\");
	RtcGetActorManager()->AddSearchPath("scene\\material\\");
	RtcGetActorManager()->AddSearchPath("scene\\effect\\");
	// end (Modified by Wayne Wong 2010-11-05)

	// Texture Path
    RtGetRender()->GetTextMgr()->AddTextSearchPath("language/chinese/texture");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("creature/texture");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("scene/texture");
    RtGetRender()->GetTextMgr()->AddTextSearchPath("scene/texture/sm");


	// ActorSetScene(&g_editScene);
    
	// Camera
    RtGetRender()->m_pCamera->SetProjParams(DegreeToRadian(45.0f), 4.f/3.f, 5.0f, 5000.0f);
	CameraEye = RtgVertex3(0,-150,150);
	CameraLookAt = RtgVertex3(0,0,0);
	CameraTrans.Zero();
	CameraRot.Zero();
	CameraForward = 0;
	CamMatRot.Unit();


//	m_floor = (CRT_ActorInstance*)ACTOR()->CreateInstance(ACTOR_RES_TYPE_ACT, "_floor");

	// begin (Modified by Wayne Wong 2010-11-09)
	//m_floor = LoadActorInstance("sky_1");
	//m_floor->Create(RtGetRender());
	m_floor = RtcGetActorManager()->CreateActor("sky_1", true);
	m_floor->Create();
	m_floor->SetOffset(RtgVertex3(0,0,-1));

	/*
	RtgMatrix16 mat;
	RtgMatrix16 tmp;
	mat.Unit();
	mat.SetView(CameraEye,CameraLookAt,RtgVertex3(0.0f, 0.0f, 1.0f));

	tmp.Unit();
	tmp.RotateX(DegreeToRadian(CameraRot.x*0.5));
	tmp.RotateY(DegreeToRadian(CameraRot.y*0.5));
	tmp.RotateZ(DegreeToRadian(CameraRot.z*0.5));
	mat = tmp * mat;

	tmp.Unit();
	tmp.Translate(CameraTrans.x,CameraTrans.y,CameraTrans.z);
	mat = mat * tmp;
	*/

	// Render State & View check
	m_emi.Set(0.5,0.5,0.5);
	m_dirLight.Set(1.0,1.0,1.0);
	RtGetRender()->m_bEnableViewCheck = FALSE;
	// RtGetRender()->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING, 0xFF505050);
	// RtGetRender()->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING, 0xFF000000);

  	const float fFloorSize = 100.f;
	m_boundingBox = new RtgGeometry(RtGetRender());
	m_boundingBox->SetVertexFormat(RTG_VCT);

	m_shaderBox.iNumStage = 1;
	m_shaderBox.bTwoSide = true;
	m_shaderBox.dwTextureFactor = 0xffffff00;
	m_shaderBox.Shaders[0].eColorOp = RTGTOP_SELECTARG1;
	m_shaderBox.Shaders[0].eColorArg1 = RTGTA_TFACTOR;
    return true;
}

void CEditorFrame::OnFrameClose()
{
	// delete shadow render target
	// move to other cpp later
	//if(shadowRenderTarget)
	//	RtGetRender()->m_Texture.DeleteTexture(shadowRenderTarget);
	delete m_boundingBox;
	RtGetRender()->DeleteShader(&m_shaderBox);

	// Modified by Wayne Wong 2010-11-09
	//if (m_floor)
	//	delete m_floor;
	//	//ACTOR()->DeleteInstance(m_floor);
	if (m_floor)
		RtcGetActorManager()->ReleaseActor(m_floor);

	for(int i=0; i<g_actorList.size(); i++)
		delete g_actorList[i];

	ActorExit();
	//ACTOR()->Exit();
	rtAudioPreExit();
	rtAudioExit();
	// rtGraphExit();
	CRtgAppFrame::OnFrameClose();
}

void CEditorFrame::OnFrameMove(float fDifTime)
{
    // lyymark 简单限制帧率
    int millisecondsPerFrame = 1000 / 30;  // 30 FPS
    Sleep(millisecondsPerFrame);

	if(fDifTime > 0.1)
		fDifTime = 0.1;
	//TODO: Where to tick????
//	ACTOR()->Tick(fDifTime*1000);

	// Modified by Wayne Wong 2010-11-09
	//CRT_ActorInstance::Update(fDifTime * 1000);
	if (g_activeActor)
		g_activeActor->Tick(fDifTime * 1000);

	// reset camera
	RtgMatrix16 mat;
	RtgMatrix16 tmp;
	mat.Unit();
	mat.SetView(CameraEye,CameraLookAt,RtgVertex3(0.0f, 0.0f, 1.0f));

	mat = CamMatRot * mat; 
	tmp.Unit();
	tmp.Translate(CameraTrans.x,CameraTrans.y,CameraTrans.z);
	mat = mat * tmp;
	RtGetRender()->m_pCamera->SetViewMatrix(mat);

	static float time = 0.f;

	static int count = 0;

	time += fDifTime;

	if(time >= 180.f) 
	{
		time = 0.f;

		count ++;
		if(count >10)count = 0;

		if(!g_activeActor || !g_activeActor->m_data) return;

		CRT_Actor *cha = g_activeActor->m_data;

		char bak[255];
        char tmp[255];

		// save path
		GetCurrentDirectory(255,bak);

		CString file = bak;
        
		sprintf(tmp,"\\creature\\autoback\\autoback%d.act",count);

		file += tmp;

		RtArchive *ar = RtCoreFile().CreateFileWriter(NULL,file);
		if(!ar) return;
		ar->WriteObject(cha);
		ar->Close();
		delete ar;     
	}
		
        
}

bool CEditorFrame::OnErrorMessage(const char* szMsg, int iType)
{
    ((Ctool_characterView*)m_pView)->Pause();
    RtCoreLog().Info("RtGraph错误: %s.\n", szMsg);
    return false;
}

void CEditorFrame::OnMouseLDown(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseLDown(iButton, x, y);
}

void CEditorFrame::OnMouseLDrag(int iButton, int x, int y, int increaseX, int increaseY)
{

}

void CEditorFrame::OnMouseLUp(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseLUp(iButton, x, y);
}

void CEditorFrame::OnMouseRDown(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseRDown(iButton, x, y);
}

void CEditorFrame::OnMouseRDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
	// if(GetAsyncKeyState(VK_CONTROL)&0x8000)
	{
		if(GetAsyncKeyState(VK_MENU)&0x8000)
		{
			// rotate
			CamMatRot.RotateX(DegreeToRadian(increaseY));
			CamMatRot.RotateLZ(DegreeToRadian(increaseX));
		}
		else
		{
			// pan
			CameraTrans.x += increaseX;
			CameraTrans.y += -increaseY;
		}
	}
    // CRtgAppFrame::OnMouseRDrag(iButton, x, y, increaseX, increaseY);
}

void CEditorFrame::OnMouseRUp(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseRUp(iButton, x, y);
}

void CEditorFrame::OnMouseMDown(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseMDown(iButton, x, y);
}

void CEditorFrame::OnMouseMUp(int iButton, int x, int y)
{
    // CRtgAppFrame::OnMouseMUp(iButton, x, y);
}

void CEditorFrame::OnBrushMove(int iButton, int x, int y)
{

}

void CEditorFrame::OnMouseMDrag(int iButton, int x, int y, int increaseX, int increaseY)
{
	if(GetAsyncKeyState(VK_MENU)&0x8000)
	{
		// rotate
		// CameraRot.x += increaseY;
		// CameraRot.z += increaseX;
		CamMatRot.RotateX(DegreeToRadian(increaseY));
		CamMatRot.RotateLZ(DegreeToRadian(increaseX));
	}
	else
	{
		// pan
		CameraTrans.x += increaseX;
		CameraTrans.y += -increaseY;
	}
}

void CEditorFrame::OnMouseWheel(int iButton, long vDelta, int x, int y)
{
	// CameraForward += vDelta * (-15);
	CameraTrans.z += vDelta * 15;
}

void CEditorFrame::OnMouseMove(int iButton, int x, int y, int increaseX, int increaseY)
{
	
}

int theme = 0;
void CEditorFrame::OnKeyDown(int iChar, bool bAltDown)
{
	//CMainFrame *main;
	//CRT_Effect *effect;
    switch (iChar)
    {
		case VK_F8:
			CameraTrans.Zero();
			CameraRot.Zero();
			CameraForward = 0;
			CamMatRot.Unit();
			// RtGetRender()->FullscreenToggle();
			break;

		case VK_F7:
			if(g_activeActor)
			{
				// g_activeActor->m_matrix.Unit();

				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->Unit();
				g_activeActor->m_inst->SetWorldMatrix(RtgMatrix12::matIdentity);
			}
			break;
		
		case VK_UP:
			if(g_activeActor)
			{
				// g_activeActor->m_matrix._31 += 5;

				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->_31 += 5;
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp._31 += 5;
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_DOWN:
			if(g_activeActor)
			{
				// g_activeActor->m_matrix._31 -= 5;
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->_31 -= 5;
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp._31 -= 5;
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_LEFT:
			if(g_activeActor)
			{	
				// g_activeActor->m_matrix._30 -= 5;
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->_30 -= 5;
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp._30 -= 5;
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_RIGHT:
			if(g_activeActor)
			{
				// g_activeActor->m_matrix._30 += 5;
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->_30 += 5;
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp._31 += 5;
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_F1:
			if(g_activeActor)
			{
				// g_activeActor->m_matrix.RotateLZ(0.1);
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->RotateLX(0.1);
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp.RotateLX(0.1);
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_F2:
			if(g_activeActor)
			{
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->RotateLZ(0.1);
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp.RotateLZ(0.1);
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_F3:
			if(g_activeActor)
			{
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->RotateLZ(-0.1);
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp.RotateLZ(-0.1);
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_F4:
			if(g_activeActor)
			{
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->TranslateY(5.0f);
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp.TranslateY(5.0f);
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;

		case VK_F5:
			if(g_activeActor)
			{
				// Modified by Wayne Wong 2010-11-09
				//g_activeActor->m_inst->GetWorldMatrix()->TranslateY(-5.0f);
				RtgMatrix12 matTemp = *g_activeActor->m_inst->GetWorldMatrix();
				matTemp.TranslateY(-5.0f);
				g_activeActor->m_inst->SetWorldMatrix(matTemp);
			}
			break;
		case VK_F9:
/*			ACTOR()->Output();*/
			break;
		//case VK_F9:
		//	CameraTrans.Zero();
		//	CameraRot.Zero();
		//	CameraForward = 0;
		//	CamMatRot.Unit();
		//	// g_pDevice->FullscreenToggle();
		//	break;

		/*case VK_F5:
			break;//copy effect
		
			if(!g_activeActor) break;
			main = (CMainFrame*)AfxGetMainWnd();
			effect = main->GetSelectedEffect();
			if(!effect) break;
	
			CRT_Effect *tmp;
			tmp = (CRT_Effect*)ACTOR()->CopyFrom(effect);
			if(tmp)
			{
				CRT_Actor *actor;
				actor = g_activeActor->m_data;
				actor->m_eftVec.push_back(tmp);
				actor->DataChange(ON_ACTOR_EFFECT_ADD, actor->m_eftVec.size()-1);
				main->m_bRebuildGridActor = true;
			}
			break;
			*/

		//case VK_F3: // effect move up
		//	if(!g_activeActor) break;
		//	main = (CMainFrame*)AfxGetMainWnd();
		//	effect = main->GetSelectedEffect();
		//	if(!effect) break;
		//	break;

		//case VK_F4: // effect move down
		//	// CXTPPaintManager::SetTheme((XTPPaintTheme)(theme++%5));
		//	break;
    }
}

void CEditorFrame::OnRender()
{
	CMainFrame *mn;
	mn = (CMainFrame*)AfxGetMainWnd();
	mn->m_viewKey.InvalidateRect(NULL,FALSE);
	// rtSleep(0.01f);

	/*
	tmp.Unit();
	tmp.RotateX(DegreeToRadian(CameraRot.x*0.5));
	tmp.RotateY(DegreeToRadian(CameraRot.y*0.5));
	tmp.RotateZ(DegreeToRadian(CameraRot.z*0.5));
	*/

    if (g_bRenderFloor)
    {
        DWORD zt = RtGetRender()->GetRenderState(RTGRS_Z_TEST);
        DWORD zw = RtGetRender()->GetRenderState(RTGRS_Z_WRITE);
        DWORD lt = RtGetRender()->GetRenderState(RTGRS_LIGHTING);

        RtGetRender()->SetRenderState(RTGRS_Z_TEST, TRUE);
        RtGetRender()->SetRenderState(RTGRS_Z_WRITE, TRUE);
        RtGetRender()->SetRenderState(RTGRS_LIGHTING, FALSE);

        RtGetRender()->SetShader(&m_ShaderNull);
        const int _Num = 20;
        DWORD sVB = 0;
        RtgVertexVC* vb = (RtgVertexVC*)RtGetRender()->m_pVB->BeginUpload(sVB, (_Num + 1) * 2 * 2 * 2 * sizeof(RtgVertexVC), FALSE,sizeof(RtgVertexVC));
        RtgMatrix16 world;
        int idx = 0;
        float s = 5.f;

        for (int i = -_Num; i <= _Num; ++i)
        {
            vb[idx].c = 0xff808080;
            vb[idx++].v = RtgVertex3(-_Num * s, i * s, -1.f);
            vb[idx].c = 0xff808080;
            vb[idx++].v = RtgVertex3( _Num * s, i * s, -1.f);
            vb[idx].c = 0xff808080;
            vb[idx++].v = RtgVertex3(i * s, -_Num * s, -1.f);
            vb[idx].c = 0xff808080;
            vb[idx++].v = RtgVertex3(i * s,  _Num * s, -1.f);
        }

        vb[_Num * 4].c = 0xff202020;
        vb[_Num * 4 + 1].c = 0xff202020;
        vb[_Num * 4 + 2].c = 0xff202020;
        vb[_Num * 4 + 3].c = 0xff202020;

        RtGetRender()->m_pVB->EndUpload(sVB);
        RtGetRender()->m_pVB->Bind(0, 0);

        world.Unit();
        RtGetRender()->SetMatrix(RTGTS_WORLD,&world);
        RtGetRender()->SetVertexFormat(RTG_VC);
        RtGetRender()->DrawPrimitiveVB(RTG_LINES, 0, (_Num + 1) * 2 * 2 * 2, (_Num * 2 + 1) * 2);
        RtGetRender()->RestoreShader(&m_ShaderNull);

        RtGetRender()->SetRenderState(RTGRS_Z_TEST, zt);
        RtGetRender()->SetRenderState(RTGRS_Z_WRITE, zw);
        RtGetRender()->SetRenderState(RTGRS_LIGHTING, lt);

    }


	RtGetRender()->SetFogEnable(m_bFog);
	RtGetRender()->SetFogColor(RtgVectorToColor(m_fogColor,0));
	RtGetRender()->SetFogFar(m_fogFar);
	RtGetRender()->SetFogFar(m_fogNear);

	RtGetRender()->m_dwClearColor = RtgVectorToColor(m_clearColor,0);
	DWORD col;
	col = RtgVectorToColor(m_emi,1.0);
	RtGetRender()->SetRenderState(RTGRS_ENVIRONMENT_LIGHTING, col);

	static unsigned long last = timeGetTime();
	unsigned long delta;
	unsigned long now = timeGetTime();
    delta = now - last;
	last  = now;

    RtgMatrix16 m16;
    m16.Unit();
	// m16 = tmp;
    RtGetRender()->SetMatrix(RTGTS_WORLD, &m16);
    RtGetRender()->SetRenderState(RTGRS_LIGHTING, FALSE);

	POINT pt;
	DWORD Color = 0xA0FFFFFF;
	RtgVertex3 vOrig, vDir,v1,v2,v3;
	GetCursorPos(&pt);
	ScreenToClient((HWND)(g_pAppCase->GetWndHandle()),&pt);
	RtGetRender()->GetPickRayFromScreen(pt.x,pt.y, vOrig, vDir);
	vDir = vOrig + 2000.f*vDir;
	if (g_activeActor && g_bRenderWire )
	{
//		if(g_activeActor->m_inst->IsLineInsertActor(vOrig,vDir,RtGetRender(),v1,v2,v3))
			
		    RtGetRender()->SetShader(&m_ShaderNull);
		    RtGetRender()->SetRenderState(RTGRS_FILL_MODE,RTG_FILLSOLID);
			RtGetRender()->DrawTriangle(v1,v2,v3,&Color);
			RtGetRender()->RestoreShader(&m_ShaderNull);
			RtGetRender()->SetRenderState(RTGRS_FILL_MODE,RTG_FILLWIREFRAME);
    
	}

    // 画线
	if(g_bRenderCoor)
    {
        DWORD dwLineColor = 0xFFFF0000;
        RtGetRender()->SetShader(&m_ShaderNull);
        RtGetRender()->DrawAxis(m16, 40.f); // 坐标轴
        //RtGetRender()->DrawLine(m_vLastStart, m_vLastEnd, &dwLineColor); // 自己指定的线
        RtGetRender()->RestoreShader(&m_ShaderNull);
    }

 	// m_Light.vDirection = RtGetRender()->m_pCamera->GetViewDir();
	// Light
	m_Light.eMode = RTG_LIGHT_DIRECTION;
	m_Light.vAmbient = m_dirLight;
	m_Light.vDiffuse = m_dirLight;
	m_Light.vDirection.x =  -1; // 从右到左为 -1 -> 1
	m_Light.vDirection.y =  -1; // 从后到前为 -1 -> 1
	m_Light.vDirection.z =  -1; // 从上到下为 -1 -> 1
	m_Light.vDirection.Normalize();
	m_Light.vPosition.x = 1000; 
	m_Light.vPosition.y = 1000;
	m_Light.vPosition.z = 1000;
	m_Light.vSpecular = RtgVertex3(1,1,1);
	m_Light.fRange = 10000.f;
	RtGetRender()->SetRenderState(RTGRS_LIGHTING, TRUE);
	RtGetRender()->SetLight(0, &m_Light);

	if(m_bCameraLight)
	{
		/*
		m_Light.eMode = RTG_LIGHT_POINT;
		m_Light.fConstantAttenuation = 0;
		m_Light.fLinearAttenuation = 0.01;
		*/
		/*m_Light.vAmbient = RtgVertex3(0,0,0);
		m_Light.vDiffuse = RtgVertex3(0,0,0);
		m_Light.vSpecular = RtgVertex3(1,1,1);*/

		RtgMatrix16 mat;
		mat = RtGetRender()->m_pCamera->GetViewMatrix();
		RtgVertex3 x;
		mat.GetAxis(2,x);
		m_Light.vDirection = RtgVertex3(0,0,0)-x;
		m_Light.vPosition = RtGetRender()->m_pCamera->GetEyePt();
		m_Light.vDirection.Normalize();
		RtGetRender()->SetLight(0, &m_Light);
	}
	

	// set other light
	vector<RtgLight*>::iterator it;
	int lightNum = 1;
	for(it=g_editScene.m_lightList.begin(); it!=g_editScene.m_lightList.end(); it++)
	{
		RtGetRender()->SetLight(lightNum++,*it);
	}

    // render active actor bounding box
	if(g_activeActor && m_bShowBoundBox)
	{
		//TODO: Enable this later
/*
		RtgAABB *ab;
		ab = g_activeActor->m_inst->GetBoundingBox();
		RtgMatrix16 mat;
		mat.Set4X3Matrix(g_activeActor->m_inst->m_finalMat);
		RtGetRender()->SetMatrix(RTGTS_WORLD,&mat);
		DWORD col = RtgVectorToColor(RtgVertex3(1,1,0),1);
		RtGetRender()->SetRenderState(RTGRS_LIGHTING,FALSE);
		RtGetRender()->DrawBox(ab->Min(),ab->Max(),&col);
		RtGetRender()->SetRenderState(RTGRS_LIGHTING,TRUE);*/

	}

	//if(g_bRenderFloor)
	//{
	//	m_floor->Render(RtGetRender(),RTGRM_NOBLENDOBJ_ONLY);
	//}

	int UseFrame = 0,Render = 0;
	int beg = timeGetTime();
	for(int i=0; i<g_actorList.size(); i++)
	{
		g_actorList[i]->Render(RtGetRender(),RTGRM_NOBLENDOBJ_ONLY);
	}
	//if(g_bRenderFloor)
	//{
	//	m_floor->Render(RtGetRender(),RTGRM_BLENDOBJ_ONLY);
	//}
	for(int i=0; i<g_actorList.size(); i++)
	{
		g_actorList[i]->Render(RtGetRender(),RTGRM_BLENDOBJ_ONLY);
	}
	Render += timeGetTime() - beg;

	if(g_activeActor)
	{
		CMainFrame *main;
		main = (CMainFrame*)AfxGetMainWnd();
	    //main->m_SetCurFrame(g_activeActor->m_inst->GetCurFrame());
		m_curFrame = g_activeActor->m_inst->GetCurFrame();
	}

    
	
	if(g_bRenderInfo)
    {
       	static char szOutputString[1024];
		 /*sprintf(szOutputString, "Mem: %d(mb) Frame: %d(ms) Render: %d(ms)",
					RtGetRender()->GetAvailableTextureMem()/1024/1024,
					UseFrame,Render);
         RtGetRender()->DrawString(0, 30, 0xFF00FF00, szOutputString);*/
		
		// Modified by Wayne Wong 2010-11-09
		RtPerformance* p = RtGetPref();
		//SActorPerformance *p = PFM();

		sprintf(szOutputString, "Performance:");
		RtGetRender()->DrawString(5, 5, 0xFF00ffff, szOutputString);
		sprintf(szOutputString,
								/*
								"------------------------\n"
								"Render:     %2.3f(ms)\n"
								"CoreRender: %2.3f(ms)\n"
								"Phy:        %2.3f(ms)\n"
								"Bone:       %2.3f(ms)\n"
								"Skin:       %2.3f(ms)\n"
								"Shadow:     %2.3f(ms)\n"
								"Actor:      %d\n"
								"VaVtx:      %d\n"
								"SkelVtx:    %d\n"
								"SkelBone:   %d\n",
								p->RenderTimer      * RtCPU::fSecondsPerCycle * 1000,
								p->CoreRenderTimer  * RtCPU::fSecondsPerCycle * 1000,
								p->PhyTimer         * RtCPU::fSecondsPerCycle * 1000,
								p->BoneTimer        * RtCPU::fSecondsPerCycle * 1000,
								p->SkinTimer        * RtCPU::fSecondsPerCycle * 1000,
								p->ShadowTimer      * RtCPU::fSecondsPerCycle * 1000,
								p->ActorNum,
								p->VaVtxNum,
								p->SkelVtxNum,
								p->SkelBoneNum
								*/
								"------------------------\n"
								"Render:			%2.3f(ms)\n"
								"Update:			%2.3f(ms)\n"
								"Skin Render:       %2.3f(ms)\n"
								"Skin Update:		%2.3f(ms)\n"
								"Effect Render:		%2.3f(ms)\n"
								"Effect Update:		%2.3f(ms)\n"
								"Actor:				%d\n"
								"Render Object:		%d\n"
								"Face:				%d\n"
								"Draw Call:			%d\n",
								p->Time_Render      * RtCPU::fSecondsPerCycle * 1000,
								p->Time_Update      * RtCPU::fSecondsPerCycle * 1000,
								p->skinRender       * RtCPU::fSecondsPerCycle * 1000,
								p->skinUpdate       * RtCPU::fSecondsPerCycle * 1000,
								p->efftRender       * RtCPU::fSecondsPerCycle * 1000,
								p->efftUpdate       * RtCPU::fSecondsPerCycle * 1000,
								p->ActorNum,
								p->RenderObj_Num,
								p->Proc_FaceNum,
								p->Draw_Call

			);

		RtGetRender()->DrawString(5, 20, 0xFF00FF00, szOutputString);
	}
	RtGetRender()->m_pCamera->Render((float)delta/1000);
	
	for(int i=0; i<8; i++)
		RtGetRender()->SetLight(i,NULL);

	//Draw3DText();
}

void CEditorFrame::OnSelectFrame(int frame)
{
	// if(m_curFrame == frame) return;
	m_curFrame = frame;
	if(g_activeActor)
		g_activeActor->UseFrame(m_curFrame);
}

void CEditorFrame::SetDirectLight(float r,float g,float b)
{
	RtgVertex3 c3(r,g,b);
	m_Light.eMode = RTG_LIGHT_DIRECTION;
	m_Light.vAmbient = c3;
	m_Light.vDiffuse = c3;
	// m_Light.vDirection.x =  1.0f;	// 从右到左为 -1 -> 1
	// m_Light.vDirection.y =  1.0f;	// 从后到前为 -1 -> 1
	// m_Light.vDirection.z =  -1.0f;	// 从上到下为 -1 -> 1
	m_Light.vDirection.Normalize();
	m_Light.vPosition.z = 100.f;
	m_Light.vSpecular = c3;
	m_Light.fRange = 10000.f;
}

void CEditorFrame::ChangeFillMode()
{
	if (!g_bRenderWire)
	{
		RtGetRender()->SetRenderState(RTGRS_FILL_MODE,RTG_FILLWIREFRAME);
	}
	else
	{
		RtGetRender()->SetRenderState(RTGRS_FILL_MODE,RTG_FILLSOLID);
	}
	g_bRenderWire = !g_bRenderWire;

}

/*----------------------------------------------------------------------------
    The End.
----------------------------------------------------------------------------*/
