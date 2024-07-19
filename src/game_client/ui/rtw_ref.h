/*
 * =====================================================================================
 *
 *       Filename:  rtw_ref.h
 *
 *    Description:  This file use to provide a smart point class. 
 *
 *        Version:  1.0
 *        Created:  11/3/2008 8:00:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Louis Huang (louis), louis.huang@yqidea.com
 *        Company:  YQIdea
 *
 * =====================================================================================
 */

template <class T>
class RtwRefPtr
{
public:
    RtwRefPtr();
    ~RtwRefPtr();

private:
    T*  m_point;
};

