
#ifndef C_PROFILE_RESULT_H
#define C_PROFILE_RESULT_H



#ifdef WIN32


#include <list>
using namespace std;

const int NORMAL_REPORT=0; //普通模式
const int COMPACT_REPORT=1; //紧凑模式

extern fstream g_profile_stream;


class CProfileNode;

class CProfileResult
{
public:
    ~CProfileResult();
    static CProfileResult* Instance();
    
    void    ExportToXML(const char* szFile="profile.xml");
    
    void    DFS(CProfileNode *pNode);
    void    PrintNode(CProfileNode* pNode);
    void    PrintNotRecorded(float f1, float f2);

    void    Visited(CProfileNode *pNode);
    bool    HaveVisited(CProfileNode *pNode);
   
protected:
    CProfileResult();
    
private:
    int                         m_iPrintStyle;
    static CProfileResult       *m_pInstance;
    std::list<CProfileNode *>   m_VisitedNodes;
    CProfileNode                *m_pNextNode;
    CProfileNode                *m_pLastNode;
};

#endif

#endif