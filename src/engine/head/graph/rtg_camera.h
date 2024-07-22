
namespace rt_graph {

	class RtgCamera : public RtObject
	{
		enum Frustum
		{
			NEAR_LEFT_BOTTOM = 0,
			NEAR_RIGHT_BOTTOM,
			NEAR_LEFT_TOP,
			NEAR_RIGHT_TOP,
			FAR_LEFT_BOTTOM,
			FAR_RIGHT_BOTTOM,
			FAR_LEFT_TOP,
			FAR_RIGHT_TOP
		};

		enum FrustumPlane
		{
			NEARPLANE = 0,
			FARPLANE,
			LEFTPLANE,
			RIGHTPLANE,
			TOPPLANE,
			BOTTOMPLANE
		};

	public:
		enum EGlassType
		{
			COLOR_ADD,
			COLOR_MODULATE
		};

	public:
		RtgVertex3  m_vEyePt;       // 镜头(眼睛)位置 Attributes for view matrix
		RtgVertex3  m_vLookatPt;    // 目标(观察点)位置
		RtgVertex3  m_vAxisY;       // 镜头上方向
		RtgVertex3  m_vAxisX;       // 镜头右方向
		RtgVertex3  m_vAxisZ;       // 镜头前方向
		RtgVertex4  m_Planes[6];    // 视锥面

		RtgMatrix16 m_matView;      // 视图矩阵
		RtgMatrix16 m_matBillboard; // 公告板矩阵

		float       m_fFOV;         // 视角 Attributes for projection matrix
		float       m_fAspect;		// 镜头宽高比
		float       m_fNearPlane;	// 近剪裁面
		float       m_fFarPlane;	// 远剪裁面
		RtgMatrix16 m_matProj;
		RtgMatrix16 m_matViewProj;
		RtgMatrix16 m_matViewProj_Inv;
		float       m_fLeft;
		float       m_fRight;
		float       m_fBottom;
		float       m_fTop;

		int         m_bUpdate;    // 更新标志
		int         m_bOrtho; // 是否是正交投影标志

	public:
		RT_DECLARE_DYNCREATE(RtgCamera, RtObject, 0, "RtGraph")
			RtgCamera();
		virtual ~RtgCamera();

		// Access functions
		RtgVertex3 GetEyePt() { return m_vEyePt; }
		RtgVertex3 GetUpVec() { return m_vAxisY; }
		RtgVertex3 GetLookatPt() { return m_vLookatPt; }
		RtgVertex3 GetViewDir() { RtgVertex3 v(m_vAxisZ); v.Negative(); return v; }
		RtgVertex3 GetCross() { return m_vAxisX; }
		virtual RtgVertex3* GetLookAtPlayerPos() { return NULL; }

		float     GetFOV() { return m_fFOV; }
		float     GetAspect() { return m_fAspect; }
		float     GetNearPlane() { return m_fNearPlane; }
		float     GetFarPlane() { return m_fFarPlane; }

		void      SetFOV(float f) { m_fFOV = f; }
		void      SetAspect(float f) { m_fAspect = f; }
		void      SetNearPlane(float f) { m_fNearPlane = f; }
		void      SetFarPlane(float f) { m_fFarPlane = f; }

		// 获取相机的视图矩阵
		RtgMatrix16 GetViewMatrix() { return m_matView; }

		// 获取相机的广告牌矩阵
		RtgMatrix16 GetBillboardMatrix() { return m_matBillboard; }

		// 获取相机的投影矩阵
		RtgMatrix16 GetProjMatrix() { return m_matProj; }
		// 重新更新相机的矩阵
		void ReUpdate();
		// 将相机的矩阵设为单位矩阵
		void Identity(void);
		// 使用给定的位置、目标和上向量设置视图参数
		void SetViewParams(float fx, float fy, float fz, float cx, float cy, float cz, float ux, float uy, float uz);
		// 使用给定的位置、目标和上向量设置视图参数（重载版本）
		void SetViewParams(RtgVertex3& vEyePt, RtgVertex3& vLookatPt, RtgVertex3& vUpVec);
		// 设置投影参数：视野、纵横比、近剪裁面和远剪裁面
		void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane);
		// 设置正交投影参数：左、上、右、下、近剪裁面和远剪裁面
		void SetOrthoParams(float fLeft, float fTop, float fRight, float fBottom, float fNearPlane, float fFarPlane);

		// 设置视图矩阵 描述物体相对相机的位置
		void SetViewMatrix(RtgMatrix16& m16);

		// 设置相机的矩阵 及描述相机的位置，同时会更新视图矩阵（通用设置函数）
		void SetMatrix(RtgMatrix16& ma);
		// 更新相机的矩阵（通常在视图或投影参数改变后调用）
		void UpdateMatrix(void);

		/** 取得眼睛所在位置和方向.
		 * 注意：不能通过GetEyePt取得眼睛所在位置，只能通过这个函数才能取得当前摄像机的眼睛所在位置
		 */
		void GetEyeInfo(RtgVertex3& eyePos, RtgVertex3& eyeDir);

		/** 恢复设备状态.
		 * 用于处理设备丢失或重置后的恢复操作。
		 */
		void RestoreDevice();

		/** 切换投影类型（透视投影和正交投影）.
		 */
		void ToggleProjectType();

		/** 设置投影类型.
		 * @param bPerspective 如果为TRUE，则设置为透视投影；否则设置为正交投影。
		 */
		void SetProjectType(BOOL bPerspective);

		/** 判断当前是否为透视投影.
		 * @return 如果m_bOrtho等于0，则返回TRUE，表示当前为透视投影；否则返回FALSE，表示为正交投影。
		 */
		bool IsPerspective() { return m_bOrtho == 0; }

		/** 增加摄像机的俯仰角（倾斜）.
		 * @param amount 值为增加的俯仰角度。
		 */
		void AddPitch(float amount);

		/** 增加摄像机的偏航角（水平旋转）.
		 * @param amount 值为增加的偏航角度。
		 */
		void AddYaw(float amount);

		/** 增加摄像机的滚动角（绕视线方向旋转）.
		 * @param amount 值为增加的滚动角度。
		 */
		void AddRoll(float amount);

		/** 将摄像机沿其视线方向移动.
		 * @param fAmount 移动的距离。
		 */
		void MoveForward(float fAmount);

		/** 将摄像机沿其右侧方向移动.
		 * @param fAmount 移动的距离。
		 */
		void MoveRight(float fAmount);

		/** 将摄像机沿其上方向移动.
		 * @param fAmount 移动的距离。
		 */
		void MoveUp(float fAmount);

		/** 设置摄像机的俯仰角（倾斜）.
		 * @param fValue 设定的俯仰角度。
		 */
		void SetPitch(float fValue);

		/** 设置摄像机的偏航角（水平旋转）.
		 * @param fValue 设定的偏航角度。
		 */
		void SetYaw(float fValue);

		/** 设置摄像机的滚动角（绕视线方向旋转）.
		 * @param fValue 设定的滚动角度。
		 */
		void SetRoll(float fValue);

		/** 设置摄像机沿其视线方向的前进距离.
		 * @param fValue 设定的前进距离。
		 */
		void SetForward(float fValue);

		/** 设置摄像机沿其右侧方向的移动距离.
		 * @param fValue 设定的移动距离。
		 */
		void SetRight(float fValue);

		/** 设置摄像机沿其上方向的移动距离.
		 * @param fValue 设定的移动距离。
		 */
		void SetUp(float fValue);

		/** 设置摄像机的位置.
		 * @param vPos 设定的位置。
		 */
		void SetPosition(RtgVertex3& vPos);

		/** 计算摄像机的视锥体（frustum）.
		 * 用于裁剪和视图相关计算。
		 */
		void ComputeFrustrum();


		//bool ViewCheckPoint(float x, float y, float z);
		//bool ViewCheckSphere(float x, float y, float z, float fRadius);
		//bool ViewCheckBound(const RtgAABB &aabb, const RtgMatrix16 &m16);

		virtual int SaveToBuffer(char* szBuffer) { return 0; }
		virtual int LoadFromBuffer(char* szBuffer) { return 0; }

		bool CheckVisable(const RtgAABB* _aabbox) const
		{
			RTASSERT(_aabbox);

			const RtgVertex3 _pos = _aabbox->vPos;
			const RtgVertex3 _ext = _aabbox->vExt;

			for (int i = 0; i < 6; ++i)
			{
				float fDist =
					m_Planes[i].x * _pos.x +
					m_Planes[i].y * _pos.y +
					m_Planes[i].z * _pos.z +
					m_Planes[i].w * 1.f;
				float fMaxAbsDist =
					fabs(m_Planes[i].x * _ext.x) +
					fabs(m_Planes[i].y * _ext.y) +
					fabs(m_Planes[i].z * _ext.z);

				if (fDist < -fMaxAbsDist)
					return false;
			}

			return true;
		}

		bool CheckVisable(const RtgVertex3& _min, const RtgVertex3& _max) const
		{
			const RtgVertex3 _pos = (_min + _max) * 0.5f;
			const RtgVertex3 _ext = _pos - _min;

			for (int i = 0; i < 6; ++i)
			{
				float fDist =
					m_Planes[i].x * _pos.x +
					m_Planes[i].y * _pos.y +
					m_Planes[i].z * _pos.z +
					m_Planes[i].w * 1.f;
				float fMaxAbsDist =
					fabs(m_Planes[i].x * _ext.x) +
					fabs(m_Planes[i].y * _ext.y) +
					fabs(m_Planes[i].z * _ext.z);

				if (fDist < -fMaxAbsDist)
					return false;
			}

			return true;
		}


	protected:
		virtual void OnSetViewParams();
		virtual void OnSetProjParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float amount) { return FALSE; }
		virtual BOOL OnAddYaw(float amount) { return FALSE; }
		virtual BOOL OnAddRoll(float amount) { return FALSE; }
		virtual BOOL OnMoveForward(float fAmount) { return FALSE; }
		virtual BOOL OnMoveRight(float fAmount) { return FALSE; }
		virtual BOOL OnMoveUp(float fAmount) { return FALSE; }
		virtual BOOL OnSetPitch(float fValue) { return FALSE; }
		virtual BOOL OnSetYaw(float fValue) { return FALSE; }
		virtual BOOL OnSetRoll(float fValue) { return FALSE; }
		virtual BOOL OnSetForward(float fValue) { return FALSE; }
		virtual BOOL OnSetRight(float fValue) { return FALSE; }
		virtual BOOL OnSetUp(float fValue) { return FALSE; }
		virtual BOOL OnSetPosition(RtgVertex3& vPos) { return FALSE; }

	public:
		// 振动, fScope幅度, fTime时间(秒)
		void Tremble(float fScope = 20.f, float fCycle = 0.3f, float fTime = 0.6f);
		void SetColoredGlass(EGlassType eType = COLOR_ADD, DWORD dwColor = 0xFFFFFFFF, const char* szFilename = NULL, float fTimeIn = 0.05, float fTimeOut = 0.2f, float fAlphaMax = 1.f, float fAlphaMin = 0.f);
		virtual void Render(float fDifSecond);

	protected:
		bool            m_bTremble;             // 是否有震动
		bool            m_bColoredGlass;        // 是否有有色玻璃

		float           m_fTrembleScope;        // 振动幅度
		float           m_fTrembleTime;         // 振动时间
		float           m_fTrembleCycle;        // 振动周期
		float           m_fTrembleScopeCur;
		float           m_fTrembleTimeCur;

		RtgShader       m_GlassShader;
		RTGVertexFormat m_eVF;
		RtString        m_szFilename;
		EGlassType      m_eGlassType;
		float           m_fGlassTimeIn;
		float           m_fGlassTimeOut;
		float           m_fGlassTimeCur;
		DWORD           m_dwGlassColor;
		float           m_fAlphaMax;
		float           m_fAlphaMin;

		RtgNVertexVCT   m_svct[4];
		RtgNVertexVC    m_svc[4];

	};

	/*----------------------------------------------------------------------------
	-   class RtgCameraFPS.
	----------------------------------------------------------------------------*/

	class RtgCameraFPS : public RtgCamera
	{
	public:
		RtgMatrix16 m_RefMat;
		float m_fRX, m_fRY, m_fRZ;

	public:
		RT_DECLARE_DYNCREATE(RtgCameraFPS, RtgCamera, 0, "RtGraph")
			RtgCameraFPS();
		virtual ~RtgCameraFPS();

		virtual void OnSetViewParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float fAmount);
		virtual BOOL OnAddYaw(float fAmount);
		virtual BOOL OnAddRoll(float fAmount);
		virtual BOOL OnMoveForward(float fAmount);
		virtual BOOL OnMoveRight(float fAmount);
		virtual BOOL OnMoveUp(float fAmount);

	};

	/*----------------------------------------------------------------------------
	-   class RtgCameraEditor.
	----------------------------------------------------------------------------*/

	class RtgCameraEditor : public RtgCamera
	{
	public:
		RtgVertex3 m_vPos;
		float m_fPitch, m_fYaw, m_fRoll;

	public:
		RT_DECLARE_DYNCREATE(RtgCameraEditor, RtgCamera, 0, "RtGraph")
			RtgCameraEditor();
		virtual ~RtgCameraEditor();

		virtual int SaveToBuffer(char* szBuffer);
		virtual int LoadFromBuffer(char* szBuffer);

		virtual void OnSetViewParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float fAmount);
		virtual BOOL OnAddYaw(float fAmount);
		virtual BOOL OnAddRoll(float fAmount);
		virtual BOOL OnMoveForward(float fAmount);
		virtual BOOL OnMoveRight(float fAmount);
		virtual BOOL OnMoveUp(float fAmount);
		virtual BOOL OnSetPitch(float fValue);
		virtual BOOL OnSetYaw(float fValue);
		virtual BOOL OnSetRoll(float fValue);
		virtual BOOL OnSetUp(float fValue);
		virtual BOOL OnSetPosition(RtgVertex3& vPos);

	};

	/*----------------------------------------------------------------------------
	-   class RtgCamera3DMax.
	----------------------------------------------------------------------------*/

	class RtgCamera3DMax : public RtgCamera
	{
	public:
		RtgVertex3 m_vPos;
		float m_fPitch, m_fYaw, m_fRoll;

	public:
		RT_DECLARE_DYNCREATE(RtgCamera3DMax, RtgCamera, 0, "RtGraph")
			RtgCamera3DMax();
		virtual ~RtgCamera3DMax();

		virtual int SaveToBuffer(char* szBuffer);
		virtual int LoadFromBuffer(char* szBuffer);

		virtual void OnSetViewParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float fAmount);
		virtual BOOL OnAddYaw(float fAmount);
		virtual BOOL OnAddRoll(float fAmount);
		virtual BOOL OnMoveForward(float fAmount);
		virtual BOOL OnMoveRight(float fAmount);
		virtual BOOL OnMoveUp(float fAmount);
		virtual BOOL OnSetPitch(float fValue);
		virtual BOOL OnSetYaw(float fValue);
		virtual BOOL OnSetRoll(float fValue);
		virtual BOOL OnSetUp(float fValue);
		virtual BOOL OnSetPosition(RtgVertex3& vPos);

	};

	/*----------------------------------------------------------------------------
	-   class RtgCameraGame.
	----------------------------------------------------------------------------*/

	class RtgCameraGame : public RtgCamera
	{

	public:
		RtgVertex3 m_vPos;
		float m_fPitch, m_fYaw, m_fRoll, m_fDistance, m_fUpOffset;

	public:
		RT_DECLARE_DYNCREATE(RtgCameraGame, RtgCamera, 0, "RtGraph")
			RtgCameraGame();
		virtual ~RtgCameraGame();

		virtual int SaveToBuffer(char* szBuffer);
		virtual int LoadFromBuffer(char* szBuffer);
		virtual RtgVertex3* GetLookAtPlayerPos() { return &m_vPos; }

		void  SetUpOffset(float fOffset) { m_fUpOffset = fOffset; }
		float GetUpOffset() { return m_fUpOffset; }

		virtual void OnSetViewParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float fAmount);
		virtual BOOL OnAddYaw(float fAmount);
		virtual BOOL OnAddRoll(float fAmount);
		virtual BOOL OnSetPitch(float fValue);
		virtual BOOL OnSetYaw(float fValue);
		virtual BOOL OnSetRoll(float fValue);
		virtual BOOL OnMoveForward(float fAmount);
		virtual BOOL OnSetForward(float fValue);
		virtual BOOL OnSetPosition(RtgVertex3& vPos);

	};

	/*----------------------------------------------------------------------------
	-   class RtgCameraD3DView.
	----------------------------------------------------------------------------*/

	class RtgCameraD3DView : public RtgCamera
	{

	public:
		RtgVertex3 m_vPos;
		float m_fPitch, m_fYaw, m_fRoll, m_fDistance, m_fUpOffset, m_fTranslationX, m_fTranslationY;

	public:
		RT_DECLARE_DYNCREATE(RtgCameraD3DView, RtgCamera, 0, "RtGraph")
			RtgCameraD3DView();
		virtual ~RtgCameraD3DView();

		virtual int SaveToBuffer(char* szBuffer);
		virtual int LoadFromBuffer(char* szBuffer);
		virtual RtgVertex3* GetLookAtPlayerPos() { return &m_vPos; }

		void  SetUpOffset(float fOffset) { m_fUpOffset = fOffset; }
		float GetUpOffset() { return m_fUpOffset; }

		virtual void OnSetViewParams();
		virtual void OnUpdateMatrix();

		virtual BOOL OnAddPitch(float fAmount);
		virtual BOOL OnAddYaw(float fAmount);
		virtual BOOL OnAddRoll(float fAmount);
		virtual BOOL OnSetPitch(float fValue);
		virtual BOOL OnSetYaw(float fValue);
		virtual BOOL OnSetRoll(float fValue);
		virtual BOOL OnMoveForward(float fAmount);
		virtual BOOL OnMoveRight(float fAmount);
		virtual BOOL OnMoveUp(float fAmount);
		virtual BOOL OnSetRight(float fValue);
		virtual BOOL OnSetUp(float fValue);
		virtual BOOL OnSetForward(float fValue);
		virtual BOOL OnSetPosition(RtgVertex3& vPos);

	};

} // namespace rt_graph

/*----------------------------------------------------------------------------
-   The End.
----------------------------------------------------------------------------*/
