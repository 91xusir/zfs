// tool_characterDoc.h :  Ctool_characterDoc ��Ľӿ�
//
#pragma once
class Ctool_characterDoc : public CDocument
{
protected: // �������л�����
	Ctool_characterDoc();
	DECLARE_DYNCREATE(Ctool_characterDoc)

// ����
public:

// ����
public:

// ��д
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~Ctool_characterDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};
