#ifndef _DEDUCT_
#define _DEDUCT_

#include "region.h"


struct SDeductData 
{
	int 			deductid;
	DeductType		type;
// 	vector<SItemID> vItemID;
// 	char*			cWorldMessage;
	void*			vdata;
};

class CDeduct
{
public:
	inline static CDeduct* GetDeductInstance()
	{
		if (m_deductInstance == 0)
		{
			m_deductInstance = RT_NEW CDeduct();
		}
		return m_deductInstance;
	}
	~CDeduct();
	SDeductData* GetDeductDataById(int deductId);
	void	AddDeductData(int deductid,DeductType type,void* v);
	void    DeleteData(int deductid);
//	void    ChangeDeductId(int i);
	bool	UseDeductData(long userid,int deductid,char* cResult,short sPoint,short sCurPoint);
	int     GetDeductId();
protected:
private:
	list<SDeductData*> m_listDeductData;
	CDeduct();
	ULONG	   m_iDeductId;
	static CDeduct* m_deductInstance;
};
#endif

