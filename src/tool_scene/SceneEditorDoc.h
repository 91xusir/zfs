// SceneEditorDoc.h :  CSceneEditorDoc ��Ľӿ�
//


#pragma once

class CSceneEditorDoc : public CDocument
{
protected: // �������л�����
	CSceneEditorDoc();
	DECLARE_DYNCREATE(CSceneEditorDoc)

// ����
public:

// ����
public:

// ��д
	public:
	virtual BOOL OnNewDocument();
//	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CSceneEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};


