
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
		RtgVertex3  m_vEyePt;       // ��ͷ(�۾�)λ�� Attributes for view matrix
		RtgVertex3  m_vLookatPt;    // Ŀ��(�۲��)λ��
		RtgVertex3  m_vAxisY;       // ��ͷ�Ϸ���
		RtgVertex3  m_vAxisX;       // ��ͷ�ҷ���
		RtgVertex3  m_vAxisZ;       // ��ͷǰ����
		RtgVertex4  m_Planes[6];    // ��׶��

		RtgMatrix16 m_matView;      // ��ͼ����
		RtgMatrix16 m_matBillboard; // ��������

		float       m_fFOV;         // �ӽ� Attributes for projection matrix
		float       m_fAspect;		// ��ͷ��߱�
		float       m_fNearPlane;	// ��������
		float       m_fFarPlane;	// Զ������
		RtgMatrix16 m_matProj;
		RtgMatrix16 m_matViewProj;
		RtgMatrix16 m_matViewProj_Inv;
		float       m_fLeft;
		float       m_fRight;
		float       m_fBottom;
		float       m_fTop;

		int         m_bUpdate;    // ���±�־
		int         m_bOrtho; // �Ƿ�������ͶӰ��־

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

		// ��ȡ�������ͼ����
		RtgMatrix16 GetViewMatrix() { return m_matView; }

		// ��ȡ����Ĺ���ƾ���
		RtgMatrix16 GetBillboardMatrix() { return m_matBillboard; }

		// ��ȡ�����ͶӰ����
		RtgMatrix16 GetProjMatrix() { return m_matProj; }
		// ���¸�������ľ���
		void ReUpdate();
		// ������ľ�����Ϊ��λ����
		void Identity(void);
		// ʹ�ø�����λ�á�Ŀ���������������ͼ����
		void SetViewParams(float fx, float fy, float fz, float cx, float cy, float cz, float ux, float uy, float uz);
		// ʹ�ø�����λ�á�Ŀ���������������ͼ���������ذ汾��
		void SetViewParams(RtgVertex3& vEyePt, RtgVertex3& vLookatPt, RtgVertex3& vUpVec);
		// ����ͶӰ��������Ұ���ݺ�ȡ����������Զ������
		void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane);
		// ��������ͶӰ���������ϡ��ҡ��¡����������Զ������
		void SetOrthoParams(float fLeft, float fTop, float fRight, float fBottom, float fNearPlane, float fFarPlane);

		// ������ͼ���� ����������������λ��
		void SetViewMatrix(RtgMatrix16& m16);

		// ��������ľ��� �����������λ�ã�ͬʱ�������ͼ����ͨ�����ú�����
		void SetMatrix(RtgMatrix16& ma);
		// ��������ľ���ͨ������ͼ��ͶӰ�����ı����ã�
		void UpdateMatrix(void);

		/** ȡ���۾�����λ�úͷ���.
		 * ע�⣺����ͨ��GetEyePtȡ���۾�����λ�ã�ֻ��ͨ�������������ȡ�õ�ǰ��������۾�����λ��
		 */
		void GetEyeInfo(RtgVertex3& eyePos, RtgVertex3& eyeDir);

		/** �ָ��豸״̬.
		 * ���ڴ����豸��ʧ�����ú�Ļָ�������
		 */
		void RestoreDevice();

		/** �л�ͶӰ���ͣ�͸��ͶӰ������ͶӰ��.
		 */
		void ToggleProjectType();

		/** ����ͶӰ����.
		 * @param bPerspective ���ΪTRUE��������Ϊ͸��ͶӰ����������Ϊ����ͶӰ��
		 */
		void SetProjectType(BOOL bPerspective);

		/** �жϵ�ǰ�Ƿ�Ϊ͸��ͶӰ.
		 * @return ���m_bOrtho����0���򷵻�TRUE����ʾ��ǰΪ͸��ͶӰ�����򷵻�FALSE����ʾΪ����ͶӰ��
		 */
		bool IsPerspective() { return m_bOrtho == 0; }

		/** ����������ĸ����ǣ���б��.
		 * @param amount ֵΪ���ӵĸ����Ƕȡ�
		 */
		void AddPitch(float amount);

		/** �����������ƫ���ǣ�ˮƽ��ת��.
		 * @param amount ֵΪ���ӵ�ƫ���Ƕȡ�
		 */
		void AddYaw(float amount);

		/** ����������Ĺ����ǣ������߷�����ת��.
		 * @param amount ֵΪ���ӵĹ����Ƕȡ�
		 */
		void AddRoll(float amount);

		/** ��������������߷����ƶ�.
		 * @param fAmount �ƶ��ľ��롣
		 */
		void MoveForward(float fAmount);

		/** ������������Ҳ෽���ƶ�.
		 * @param fAmount �ƶ��ľ��롣
		 */
		void MoveRight(float fAmount);

		/** ������������Ϸ����ƶ�.
		 * @param fAmount �ƶ��ľ��롣
		 */
		void MoveUp(float fAmount);

		/** ����������ĸ����ǣ���б��.
		 * @param fValue �趨�ĸ����Ƕȡ�
		 */
		void SetPitch(float fValue);

		/** �����������ƫ���ǣ�ˮƽ��ת��.
		 * @param fValue �趨��ƫ���Ƕȡ�
		 */
		void SetYaw(float fValue);

		/** ����������Ĺ����ǣ������߷�����ת��.
		 * @param fValue �趨�Ĺ����Ƕȡ�
		 */
		void SetRoll(float fValue);

		/** ����������������߷����ǰ������.
		 * @param fValue �趨��ǰ�����롣
		 */
		void SetForward(float fValue);

		/** ��������������Ҳ෽����ƶ�����.
		 * @param fValue �趨���ƶ����롣
		 */
		void SetRight(float fValue);

		/** ��������������Ϸ�����ƶ�����.
		 * @param fValue �趨���ƶ����롣
		 */
		void SetUp(float fValue);

		/** �����������λ��.
		 * @param vPos �趨��λ�á�
		 */
		void SetPosition(RtgVertex3& vPos);

		/** �������������׶�壨frustum��.
		 * ���ڲü�����ͼ��ؼ��㡣
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
		// ��, fScope����, fTimeʱ��(��)
		void Tremble(float fScope = 20.f, float fCycle = 0.3f, float fTime = 0.6f);
		void SetColoredGlass(EGlassType eType = COLOR_ADD, DWORD dwColor = 0xFFFFFFFF, const char* szFilename = NULL, float fTimeIn = 0.05, float fTimeOut = 0.2f, float fAlphaMax = 1.f, float fAlphaMin = 0.f);
		virtual void Render(float fDifSecond);

	protected:
		bool            m_bTremble;             // �Ƿ�����
		bool            m_bColoredGlass;        // �Ƿ�����ɫ����

		float           m_fTrembleScope;        // �񶯷���
		float           m_fTrembleTime;         // ��ʱ��
		float           m_fTrembleCycle;        // ������
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
