/* 
* wsdlpull- A C++ parser  for WSDL  (Web services description language)
* Copyright (C) 2005-2007 Vivek Krishna
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*
*/

#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <wininet.h>
#include "w3c.h"
#pragma comment(lib, "wininet.lib")
#endif

#ifndef _WIN32
#include <wsdl-config.h>
#endif

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#ifdef WITH_CURL
#include <curl/curl.h>
#endif

#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <map>
#include "xmlpull/XmlUtils.h"

const std::string ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::map<std::string,std::string> urlCache_;
//put all I/O and std::string manip  utiliy functions here

int
XmlUtils::parseInt (std::string s, int radix)
{
  int len = s.size ();
  int value = 0;
  if (s.empty ())
    return -1;
  for (int i = 0; i < len; i++) {
    if (radix == 10) {
      if (s[i] <= '9' && s[i] >= 0)
	value = (i == 0) ? (s[i] - '0') : radix * value + (s[i] - '0');

      else
	throw std::exception ();
    }
    else if (radix == 16) {
      //assumes that the encoding format has arranges the alphabets and numbers in increasing order
      if (s[i] <= '9' && s[i] >= 0)
	value = (i == 0) ? (s[i] - '0') : radix * value + (s[i] - '0');

      else if (s[i] <= 'F' && s[i] >= 'A')
	value =
	  (i ==
	   0) ? (s[i] - 'A') + 10 : radix * value + (s[i] - 'A') + 10;

      else if (s[i] <= 'f' && s[i] >= 'a')
	value =(i ==0) ? (s[i] - 'a') + 10 : radix * value + (s[i] - 'a') + 10;
    }
  }
  return value;
}


std::ostream &
XmlUtils::dbsp (std::ostream & str)
{
  return str << "  ";
}


std::ostream &
XmlUtils::blk (std::ostream & str)
{
  return str << std::endl << "*************" << std::endl;
}

/*
* Fetch a document at the given URI  and store it in a file
*/

bool
WSDLPULL_EXPORT
XmlUtils::fetchUri(std::string uri,
		   std::string& filename)
{
  if(uri.find("http://")!=std::string::npos || 
     uri.find("https://")!=std::string::npos || 
     uri.find("ftp://") !=std::string::npos)
    {  

      if (urlCache_.find(uri) != urlCache_.end()) {
	
	filename=urlCache_[uri];
	return true;
      }

#ifndef _WIN32 
      filename=uri.substr(uri.rfind('/')+1);
      if (filename.empty()) {
	//for *nix try to use the name from the url 
#endif
      // Generate a random "[8 chars].[3 chars]" filename
      srand(time(NULL));
      filename.clear();
      for (int i = 0; i < 8; i++){
	
	filename += ALPHA.at(rand()%52);
      }
      filename.append(".wp-tmp");
#ifndef _WIN32 
      }
      std::string dir="/tmp/";
      filename = dir + filename;
#endif

      urlCache_[uri]=filename;
#ifdef WITH_CURL
      CURL * curl;
      CURLcode res;
      curl=curl_easy_init();
      FILE * file;
      if(curl){
        file=fopen(filename.c_str(),"w");

	if (file == NULL) {
	  fprintf(stderr, "Can't open file %s: %s\n", filename.c_str(),
		  strerror(errno));
	  exit(-1);
	}

        curl_easy_setopt(curl, CURLOPT_URL,uri.c_str());
        curl_easy_setopt(curl,CURLOPT_FILE,(void*)file);
        curl_easy_setopt(curl,CURLOPT_TIMEOUT,60);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);	
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

        if (XmlUtils::getProxy()){
          curl_easy_setopt(curl,CURLOPT_PROXY,XmlUtils::getProxyHost().c_str());
          std::string tmp=XmlUtils::getProxyUser()+":"+XmlUtils::getProxyPass();
	  curl_easy_setopt(curl,CURLOPT_PROXYUSERPWD,tmp.c_str());
	}
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        fclose(file);
        if(res)
          return false;
        else 
          return true;
      }
#elif _WIN32 
      std::ofstream ofs(filename.c_str());
      unsigned long nread;
      W3Client w3;
          
      if(w3.Connect(uri.c_str())){
	if(w3.Request(w3.GetURI())){
	  unsigned char buf[1024]="\0";
	  while((nread=w3.Response(buf, 1023))){
	    buf[nread]='\0';
	    ofs << buf;
	  }
	}
	w3.Close();
      }
      ofs.close();	    
      return true;
    
#else
      return false;
#endif
    }
  else  { 
    /*
     * assume its a file on the disk
     */
    // modifications from F.V. Fri Nov 30 09:42:06 CET 2007:
    // - #ifdef replaced by #if !defined, 
    // - added p to get the start position, 
    // - revised offsets
    // - use-case with a single '/' added (I am not sure this is conform to spec)
// #ifdef _WIN32
#if     !defined(_WIN32)
    unsigned int p;
    if ((p=uri.find("file:///"))!=std::string::npos)
      {
	uri = uri.substr(p+7, uri.length()-p-7);
      }
    else if ((p=uri.find("file://"))!=std::string::npos)
      {
	uri = uri.substr(p+6, uri.length()-p-6);
      }
    else if ((p=uri.find("file:/"))!=std::string::npos)
      {
	uri = uri.substr(p+5, uri.length()-p-5);
      }
#endif

    filename=uri;
    std::ifstream ifs;
    ifs.open(filename.c_str(),std::ios::in);
    if(ifs.fail()) {
      ifs.close();
      return false;
    }
    else {
      ifs.close();
      return true;
    }
  }
  return true;
}


std::string
WSDLPULL_EXPORT
XmlUtils::acceptSecretKey(const std::string& field)
{
  std::cerr<<field<<": ";
  char password [50];
#ifndef _WIN32  
  tcflag_t oflags;
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  oflags = term.c_lflag;
  term.c_lflag = oflags & ~(ECHO | ECHOK | ICANON);
  term.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);

  scanf("%s", password);

  term.c_lflag = oflags;
  term.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
#else
  scanf("%s", password);
#endif
  return password;
}

#ifdef _WIN32
void 
XmlUtils::winPost(const std::string uri,const std::string username,
                  const std::string password,const std::string data,
		  std::string action,char* &results)
{
  W3Client w3;
  const char* d = data.c_str()   ;       
  if(w3.Connect(uri.c_str())){
    w3.InitializePostArguments();
    w3.setContentType("Content-Type: text/xml; charset=UTF-8\r\n");
    w3.setAcceptTypes("Accept: text/xml\r\n");
    w3.AddPostArgument(d,data.length());
    std::string tmp="SOAPAction: ";
    tmp+='"';
    tmp+=action;
    tmp+='"';
    tmp+="\r\n";
    w3.setSoapAction(tmp.c_str());
	         
    if(w3.RequestPost(w3.GetURI())){
      unsigned long nread = 0,tot=0;
      char buf[1024]="\0";
			  
      while((nread=w3.Response(reinterpret_cast<unsigned char *>(buf), 1023))){

	     	 
	if (results == 0){
	  results = (char*)malloc(sizeof(unsigned char) * nread);
	}
	else{
	  results = (char*) realloc(results,sizeof(unsigned char) *  (nread + tot));
	}
	memcpy (results+tot,buf,nread);
	tot+=nread;
      }
    }
    //std::cout<<results;
  }
}
#endif

static bool g_bProxy = false;
static std::string g_sProxyHost; //host:port format
static std::string g_sProxyUser;
static std::string g_sProxyPass;

bool
WSDLPULL_EXPORT
XmlUtils::getProxy ()
{
  return g_bProxy;
}

void
WSDLPULL_EXPORT
XmlUtils::setProxy (const bool bProxy)
{
  g_bProxy = bProxy;
}

std::string
WSDLPULL_EXPORT
XmlUtils::getProxyHost ()
{
  return g_sProxyHost;
}

void
WSDLPULL_EXPORT
XmlUtils::setProxyHost (const std::string& sProxyHost)
{
  g_sProxyHost = sProxyHost;
}

std::string
WSDLPULL_EXPORT
XmlUtils::getProxyUser ()
{
  return g_sProxyUser;
}

void
WSDLPULL_EXPORT
XmlUtils::setProxyUser (const std::string& sProxyUser)
{
  g_sProxyUser = sProxyUser;
}

std::string
WSDLPULL_EXPORT
XmlUtils::getProxyPass ()
{
  return g_sProxyPass;
}

void
WSDLPULL_EXPORT
XmlUtils::setProxyPass (const std::string& sProxyPass)
{
  g_sProxyPass = sProxyPass;
}
