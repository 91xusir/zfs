
#ifdef WIN32 

#include <windows.h>
#include <stdio.h>
//#include <FSTREAM.h>
#include <fstream>
#include "profile.h"
#include "profile_result.h"
#include <string>
#include "net/rt_net.h"



fstream g_profile_stream;

char *l_szString0[6] =
{
    {"占用父节点时间百分比"},
    {"占用总时间百分比"},
    {"每帧占用时间ms"},
    {"占用时间ms"},
    {"调用次数"},
    {"missing_time"}
};

char *l_szString1[6] =
{
    {"ppercent"},
    {"tpercent"},
    {"pre"},
    {"total"},
    {"call"},
    {"missing_time"}
};

char **l_szString;




bool profile_log(char *format, ...)
{
    if( !g_profile_stream.is_open() )
        return false;
    
    fstream *pfs = &g_profile_stream;
    
    va_list args;
    va_start(args, format);
    const int MAX_BUFFER_SIZE_1024=1024;
    int  nBuf = 0;
    char szBuffer[MAX_BUFFER_SIZE_1024];
    
#ifdef WIN32
    nBuf += _vsnprintf(szBuffer+nBuf,MAX_BUFFER_SIZE_1024-nBuf,format, args);
#else // linux
    nBuf += vsnprintf(szBuffer+nBuf,MAX_BUFFER_SIZE_1024-nBuf,format, args);
#endif	
    
    if( nBuf > MAX_BUFFER_SIZE_1024-10 ) 
        nBuf = MAX_BUFFER_SIZE_1024-10;
    //printf("%s",szBuffer);
    
    pfs->write(szBuffer,nBuf);
    pfs->flush();
    
    va_end(args);
    
    return true;
}

CProfileResult* CProfileResult::m_pInstance = 0;

CProfileResult::CProfileResult()
{
    m_pNextNode  = 0;
    m_pLastNode = 0;
    m_iPrintStyle = REPORT_STYLE;// 0 普通模式, 1 紧凑模式
    switch (m_iPrintStyle)
    {
    case 0: l_szString=l_szString0;break;
    case 1: l_szString=l_szString1;break;
    }
}

CProfileResult::~CProfileResult()
{
    
}

CProfileResult* CProfileResult::Instance()
{
    if( !m_pInstance )
        m_pInstance = RT_NEW CProfileResult;
    return m_pInstance;
}

//#include "interface.h"
//#include "g_util.h"
void CProfileResult::ExportToXML(const char* szFile/* ="profile.xml" */)
{
    g_profile_stream.open( szFile, ios::out );
    profile_log( "<?xml version='1.0' encoding='GB2312'?>\n" );
    DFS( CProfileManager::Root.Get_Child() );
    g_profile_stream.close();   
}

void CProfileResult::DFS(CProfileNode *pNode)
{
    int i;
    float f1, f2;
    CProfileNode *pSubNode;
	if(!pNode) return;
	switch (m_iPrintStyle)
    {
    case 0:
        {
            profile_log( "<%s>",    pNode->Get_Name() );
            PrintNode( pNode);
            Visited( pNode );

            f1 = 100.00f;
            f2 = pNode->Get_Percent_of_Total();

            for (i=0; i<pNode->Get_Child_Count(); i++ )
            {
                pSubNode = pNode->Get_Sub_Node(i);
                if( !pSubNode ) continue;

                if( !HaveVisited(pSubNode) )
                    DFS( pSubNode );
                f1 -= pSubNode->Get_Percent_of_Parent();
                f2 -= pSubNode->Get_Percent_of_Total();
            }

            if(i>0)     PrintNotRecorded(f1, f2);

            profile_log( "</%s>\n",  pNode->Get_Name() );
        }
        break;
    case 1:
        {
            if (pNode->Get_Child_Count())
            {
                profile_log( "<%s", pNode->Get_Name());
                PrintNode( pNode);
                profile_log(">");

                Visited( pNode );

                f1 = 100.00f;
                f2 = pNode->Get_Percent_of_Total();
                for (i=0; i<pNode->Get_Child_Count(); i++ )
                {
                    pSubNode = pNode->Get_Sub_Node(i);
                    if( !pSubNode ) continue;

                    if( !HaveVisited(pSubNode) )
                        DFS( pSubNode );
                    f1 -= pSubNode->Get_Percent_of_Parent();
                    f2 -= pSubNode->Get_Percent_of_Total();
                }
                if(i>0)     PrintNotRecorded(f1, f2);

                profile_log( "</%s>\n",  pNode->Get_Name() );
            }else
            {
                profile_log( "<%s", pNode->Get_Name());
                PrintNode( pNode);
                profile_log("/>");
                Visited( pNode );
            }
        }
        break;
    }
}

void CProfileResult::PrintNotRecorded(float f1, float f2)
{   
    switch (m_iPrintStyle)
    {
    case 0:
        profile_log( "<%s>", l_szString[5] );
        profile_log( "<%s>%.2f%s</%s>\n", l_szString[0], f1, "%", l_szString[0] );
        profile_log( "<%s>%.2f%s</%s>\n", l_szString[1], f2, "%", l_szString[1] );    
        profile_log( "</%s>", l_szString[5] );
        break;
    case 1:
        profile_log( "<%s", l_szString[5] );
        profile_log( " %s=\"%.2f%%\"", l_szString[0], f1);
        profile_log( " %s=\"%.2f%%\"", l_szString[1], f2);
        profile_log( "/>");
        break;
    }
}

void CProfileResult::PrintNode(CProfileNode *pNode)
{
    if(!pNode)  return;

    float f1, f2;
    switch (m_iPrintStyle)
    {
    case 0:
        f1 = pNode->Get_Percent_of_Parent();
        profile_log( "<%s>%.2f%s</%s>\n", l_szString[0], f1, "%", l_szString[0] );
        f2 = pNode->Get_Percent_of_Total();
        profile_log( "<%s>%.2f%s</%s>\n", l_szString[1], f2, "%", l_szString[1] );    
        profile_log( "<%s>%8.3fms</%s>\n ", l_szString[2], pNode->Get_Time_Per_Call(), l_szString[2] );
        profile_log( "<%s>%8.3fms</%s>\n ", l_szString[3], pNode->Get_Total_Time(), l_szString[3] );
        profile_log( "<%s>%6d</%s>\n", l_szString[4], pNode->Get_Total_Calls(), l_szString[4] );
        break;
    case 1:
        f1 = pNode->Get_Percent_of_Parent();
        profile_log( " %s=\"%.2f%%\"", l_szString[0], f1);
        f2 = pNode->Get_Percent_of_Total();
        profile_log( " %s=\"%.2f%%\"", l_szString[1], f2);    
        profile_log( " %s=\"%8.3fms\"", l_szString[2], pNode->Get_Time_Per_Call());
        profile_log( " %s=\"%8.3fms\"", l_szString[3], pNode->Get_Total_Time());
        profile_log( " %s=\"%d\"", l_szString[4], pNode->Get_Total_Calls());
        break;
    }
}

void CProfileResult::Visited(CProfileNode *pNode)
{
    if( !pNode )     return;
    
    m_VisitedNodes.push_back(pNode);
}

bool CProfileResult::HaveVisited(CProfileNode *pNode)
{
    if( !pNode )    return false;

    std::list<CProfileNode *>::iterator itr;
    for( itr=m_VisitedNodes.begin(); itr!=m_VisitedNodes.end(); itr++)
    {
        if( pNode==(*itr) )
            return true;
    }
    
    return false;
}

#endif
