


#ifndef PACK_ERROR_H 
#define PACK_ERROR_H
#include <iostream>
#include <string>
#include "core/rt2_core.h"

using std::string;

class CPackErr
{
public:
	virtual void OnInit()=0;
	virtual void OnClear()=0;

	virtual CPackErr& operator <<(const string vMessage)=0;
};


class CPackErrStd:public CPackErr
{
public:
	virtual void OnInit(){};
	virtual void OnClear(){};

	virtual CPackErr& operator <<(const string vMessage){std::cout<<vMessage;return *this; };

};

class CPackErrRtk:public CPackErr
{
public:
	virtual void OnInit(){};
	virtual void OnClear(){};

	virtual CPackErr& operator <<(const string vMessage)
	{
		//ERR(vMessage.c_str());
		return *this; 
	};
};




#endif//PACK_ERROR_H
