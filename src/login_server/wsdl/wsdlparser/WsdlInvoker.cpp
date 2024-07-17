/* 
 * wsdlpull - A C++ parser  for WSDL  (Web services description language)
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
 */

#ifndef _WIN32
#include <wsdl-config.h>
#endif


#ifdef  WITH_CURL
#include <curl/curl.h>
#endif 
#include "wsdlparser/WsdlInvoker.h"

extern "C" {
  size_t storeResults(void * buf,size_t sz,size_t nmemb,void* userdata);
}
static char* results_ = 0;

namespace WsdlPull {

WsdlInvoker::WsdlInvoker()
  :wParser_(0),
   ourParser_(0),
   xmlStream_(0),
   soap_(0),
   soapheaders_(false),
   hPartId_(-1),
   soapstr_(0),
   status_(false),
   serializeMode_(false),
   verbose_(false),
   dontPost_(false),
   oHeaders_(0),
   op_(0),
   n_(0),
   iHeaders_(0),
   messageType_(WsdlPull::Input)
{
}

WsdlInvoker::WsdlInvoker(const std::string & url)
  :wParser_(0),
   ourParser_(0),
   xmlStream_(0),
   soap_(0),
   soapheaders_(false),
   hPartId_(-1),
   status_(false),
   serializeMode_(false),
   verbose_(false),
   dontPost_(false),
   op_(0),
   n_(0),
   iHeaders_(0),
   messageType_(WsdlPull::Input)
{
  parseWsdl(url);
}

WsdlInvoker::WsdlInvoker(const std::string & url, const std::string & schemaPath)
   :wParser_(0),
   ourParser_(0),
   xmlStream_(0),
   soap_(0),
   soapheaders_(false),
   hPartId_(-1),
   status_(false),
   serializeMode_(false),
   verbose_(false),
   dontPost_(false),
   op_(0),
   n_(0),
   iHeaders_(0),
   messageType_(WsdlPull::Input)
{
      parseWsdl(url, schemaPath);
}


void
WsdlInvoker::parseWsdl(const std::string & url, const std::string & schemaPath)
{
  try{
    wParser_ = new WsdlParser(url,logger_, schemaPath);
    ourParser_= wParser_;
    if (wParser_){
      //parse the web service
      while (wParser_->getNextElement () != WsdlParser::END);
      if (wParser_->status()){
      
	status_=true;
	init(wParser_);
      }
    }
  }
    catch (WsdlException we)
      {
       logger_<<"An exception  occurred at "<<we.line   
  	     <<":"<<we.col<<std::endl;
        logger_<<we.description<<std::endl;
        status_ =false;
      }
    catch (SchemaParserException spe)
      {
       logger_<<"An exception occurred at "<<spe.line
  	     <<":"<<spe.col<<std::endl;
        logger_<<spe.description<<std::endl;
        status_ =false;
      }
    catch (XmlPullParserException xpe)
      {
       logger_<<"An exception occurred at "<<xpe.line
  	     <<":"<<xpe.col<<std::endl;
       logger_<<xpe.description<<std::endl;
       status_= false;
      }
}

bool
WsdlInvoker::init(WsdlParser* parser)
{
  try{
    wParser_ = parser;
    status_ = wParser_->status();
    if (status_){
      
      PortType::cPortTypeIterator p1,p2;
      wParser_->getPortTypes(p1,p2);
      int i=0;
      Soap* soap=static_cast<Soap*> (wParser_->getExtensibilityHandler(Soap::soapBindingUri));
      while(p1!=p2){
    
	Operation::cOpIterator op1,op2;
	(*p1)->getOperations(op1,op2);
	const Binding *bn = (*p1)->binding(Soap::soapBindingUri);
	if (!bn){
	  p1++;
	  continue;
	}
	int soap_binding_elem =soap->getElementName (bn->getBindingInfo ());
	//check if the port type has a soap binding
	if (soap_binding_elem == 0){
	  p1++;
	  continue;
	}

	while(op1!=op2){
	
	  opMap_[(*op1)->getName()]=*op1;
	  op1++; 
	  i++;
	}
	p1++;
      }
    }
  }
  catch (WsdlException we)
    {
      logger_<<"A WSDL exception occurred at"<<we.line
	     <<":"<<we.col<<std::endl;
      logger_<<we.description<<std::endl;
      status_ =false;
    }
  catch (SchemaParserException spe)
    {
      logger_<<"A Schema Parser exception occurred at "<<spe.line
	     <<":"<<spe.col<<std::endl;
      logger_<<spe.description<<std::endl;
      status_ =false;
    }
  catch (XmlPullParserException xpe)
    {
      logger_<<"An Xml Parsing exception occurred at row:col "<<xpe.line
	     <<":"<<xpe.col<<std::endl;
      logger_<<xpe.description<<std::endl;
      status_ =false;
    }
  return status_;
}
 
int
WsdlInvoker::getOperations(std::vector<std::string> & operations)
{
  int i = 0;
  for(
  std::map<std::string,const Operation*>::iterator it =
    opMap_.begin();
  it != opMap_.end();
  it++,i++){

    operations.push_back(it->first);
  }
  return i;
}

std::string
WsdlInvoker::getDocumentation()
{
   return *(wParser_->getDocumentation());
}
   
std::string
WsdlInvoker::getOpDocumentation(const std::string & n)
{
  
  std::map<std::string,const Operation*>::iterator it =
    opMap_.find(n);
  
  if (it != opMap_.end()){
    
    return  it->second->getDocumentation();
  }
  return "";
}

bool
WsdlInvoker::setOperation(const std::string & opname,
			  WsdlPull::MessageType mType)
{
  reset();
  messageType_ = mType;
   std::map<std::string,const Operation*>::iterator it =
    opMap_.find(opname);

  if (it != opMap_.end()){
    
    op_ = it->second;

    getOperationDetails(op_);
    
    if (soapheaders_){
      serializeHeader();
    }
    serialize();
    n_ = iHeaders_;
    return true;
  }
  else{
    return false;
  }
}

std::string
WsdlInvoker::getServiceEndPoint(const std::string & opname) 
{

  reset();
  location_="";
  std::map<std::string,const Operation*>::iterator it =
    opMap_.find(opname);

  if (it != opMap_.end()){
    
    const Operation* op = it->second;

    getOperationDetails(op);
    reset();
  }
  return location_;
}

void
WsdlInvoker::getOperationDetails(const Operation* op) 
{
  const Binding * bnSoap = op->portType()->binding(Soap::soapBindingUri);
  soap_ = static_cast<Soap*> (wParser_->getExtensibilityHandler(Soap::soapBindingUri));    

  ///get various soap properties
  soap_->getServiceLocation (bnSoap->getServiceExtId (),location_);
  style_ = soap_->getStyle();

  //get the soap:operation's SOAPAction and style
  const int *bindings = 0;
  int opIndex  = op->portType()->getOperationIndex(op->getName());
  bnSoap->getOpBinding (opIndex, bindings);
  int soapOpBindingId = bindings[0];
  //operation's style over rides 
  soap_->getSoapOperationInfo (soapOpBindingId, action_, style_);

  //get the soap:body namespace and use attributes
  int nBindings=bnSoap->getInputBinding(opIndex,bindings);
  //get the body and header
  for (int x=0;x<nBindings;x++){
    if (soap_->isSoapBody(bindings[x])){

      soap_->getSoapBodyInfo(bindings[x],nsp_,use_,encodingStyle_);
    }
    if (soap_->isSoapHeader(bindings[x]))
      soapheaders_ = true;

  }
    
  if (nsp_.empty()){

    nsp_ = wParser_->getNamespace();
  }
}

void
WsdlInvoker::serializeHeader()
{
  //create input  holders for the soap header,use the same list
  //but just remember where the header's params end
  std::string name;
  
  int hPartId;
  const Message* hMessage;//message corresponding to soap header

  const Binding * bnSoap = op_->portType()->binding(Soap::soapBindingUri);
  const int *bindings = 0;
  int opIndex  = op_->portType()->getOperationIndex(op_->getName());
  int nBindings=bnSoap->getInputBinding(opIndex,bindings);
  //get the body and header
  for (int x=0;x<nBindings;x++){
    
    if (soap_->isSoapHeader(bindings[x])){
      soap_->getSoapHeaderInfo(bindings[x],hnsp_,hPartId,hMessage);  

  
      Schema::Type pType =Schema::XSD_INVALID;
      if (hMessage->getPartRefType(hPartId)==Part::Type){
	name = hMessage->getMessagePart(hPartId)->element()->getName();
	pType = (Schema::Type)hMessage->getMessagePart(hPartId)->element()->getType();
      }
      else {
	name = hMessage->getPartName(hPartId);
	pType = (Schema::Type)hMessage->getMessagePart(hPartId)->type();
      }
      std::vector<std::string> parents;
      parents.push_back(name);
      serializeType(pType,
		    name,
		    wParser_->getSchemaParser(hMessage->getPartContentSchemaId(hPartId)),
		    1,1,parents,hnsp_);
    }
  }
  iHeaders_ = elems_.size();

}

//this method extracts the  atomic types needed for the web service
//it recursively calls serializeType for all the input or output types expected
//This method works in 2 modes.In the serializeMode_ == false it creates holders
//for the parameter values.In serializeMode_ == true it uses the inputs from the holders 
//to generate the SOAP XML message
void
WsdlInvoker::serialize()
{
  const Message * m = op_->getMessage(messageType_);
  if (!m)
    return;
  
  for (int i = 0 ;i<m->getNumParts();i++){
      
    Part::PartRefType prt = m->getPartRefType(i);
    const Part * p = m->getMessagePart(i);
    const SchemaParser * sParser = wParser_->getSchemaParser(p->schemaId());
    
    std::vector<std::string> parents;
    if (prt == Part::Elem){
      
      const Element * e = p->element();
      serializeType((Schema::Type)e->getType(),e->getName(),sParser,1,1,parents);
    }
    else{
      
      serializeType((Schema::Type)p->type(),p->name(),sParser,1,1,parents);
    }
  }
}

void
WsdlInvoker::serializeType(Schema::Type typeId,
			   const std::string &tag,
			   const SchemaParser * sParser,
                           int minimum,
                           int maximum,
			   std::vector<std::string> parents,
			   const std::string nsp)
{
  std::string t = tag;
  if (t == "*")
    t = "item";


  //for( std::vector<std::string>::iterator it=parents.begin();it!=parents.end();it++) std::cout<<*it; 

  const XSDType * pType = sParser->getType(typeId);
  if ( pType== 0 ||
       pType->isSimple() ||
       pType->getContentModel() == Schema::Simple){
    
    if (serializeMode_ == false){
      
      parents.push_back(tag);
      Parameter p(typeId,t,minimum,maximum,sParser,parents);
      elems_.push_back(p);

#ifdef LOGGING

      std::cout<<"Adding input type "<<tag<<XmlUtils::dbsp
               <<sParser->getTypeName(typeId)<<XmlUtils::dbsp;
      std::cout<<sParser->getNamespace()<<std::endl;
#endif
    }
    else{
      //generate the xml
      serializeParam(n_++,t,sParser,nsp);
    }
  }
  else{

    if (serializeMode_){
      
      if (style_ == Soap::DOC){
    
	xmlStream_->setPrefix(getPrefix(sParser->getNamespace()),sParser->getNamespace());
	xmlStream_->startTag(sParser->getNamespace(),t);
      }
      else{

	xmlStream_->startTag(nsp,t);

	//fix for sending SOAP arrays.add the soap arrayType attribute
	//works only for 1-D arrays
	const ComplexType* ct = static_cast<const ComplexType*>(pType);
	if(isSoapArray(ct,sParser)){

	  std::string arrayName = ct->getName();
	  arrayName = "ns:"+arrayName+"[1]";
	  xmlStream_->attribute(Soap::soapEncUri,"arrayType",arrayName);
	}
      }
    }
    else {
      
      //complex types with multiple occurences


      //      parents.push_back(tag);
      //      Parameter p(typeId,t,minimum,maximum,sParser,parents);
      //      elems_.push_back(p);       TODO
      
    }
    
    
    const ComplexType * ct =
      static_cast<const ComplexType*>(pType);
    
    //complex types handling
    if (ct->getNumAttributes() > 0) {
      
      for (int i = 0; i < ct->getNumAttributes(); i++) {
	  
	const Attribute*at = ct->getAttribute(i);
	/*
	 * Check for the correctness of each attribute
	 */
	if (at->isRequired()){
	
	  if (serializeMode_ == false){

	    std::vector<std::string> attparents(parents);
	    attparents.push_back(tag);
	    attparents.push_back("#" + at->getName() + "#");
	    Parameter p((Schema::Type)at->getType(),at->getName(),elems_.size(),0,sParser,
	                attparents);
	    elems_.push_back(p);
	  }
	  else{
	    //generate the xml
	    
	    xmlStream_->attribute(sParser->getNamespace(),at->getName(),elems_[n_++].data_[0]);
	  }
	}
	else
	  continue;
      }
    }
  
    if (ct->getContentModel() == Schema::Simple) {

      if (serializeMode_ == false){
      
	parents.push_back(tag);
        Parameter p((Schema::Type)ct->getContentType(),tag,minimum,maximum,sParser,parents);
	elems_.push_back(p);
      }
      else{
	//generate the xml
	serializeParam(n_++,t,sParser,nsp);
      }
    }
    else{
          
      ContentModel* cm=ct->getContents();
      if(cm){

	parents.push_back(tag);
	serializeContentModel(cm,sParser,parents,nsp);
      }
    }

    if (serializeMode_){

      if (style_ == Soap::DOC){
    
	    xmlStream_->endTag(sParser->getNamespace(),tag);
      }
      else{
	
	    xmlStream_->endTag(nsp,t);
      }
    }
  }
}

void
WsdlInvoker::serializeContentModel(ContentModel *cm,
				   const SchemaParser *sParser,
				   std::vector<std::string> parents,
				   const std::string nsp)
{
  
  ContentModel::ContentsIterator cit_b=cm->begin();
  ContentModel::ContentsIterator cit_e=cm->end();
  ContentModel::ContentsIterator ci=cit_b;

	
  switch (cm->getCompositor())
    {
    case Schema::All:
    case Schema::Sequence:
    case Schema::Choice:
      {
	// a simple logic to start with
	// not taking care of all,choice ,sequence as of now
	
	for (ci=cit_b;ci!=cit_e;ci++){
	  
	  if(ci->second==ContentModel::Particle &&
             ci->first.e->getMax() > 0){

	    const SchemaParser* s1Parser = sParser;
	    Schema::Type t=(Schema::Type)ci->first.e->getType();
	    
	    if (!ci->first.e->getTypeNamespace().empty() &&
		sParser->isImported(ci->first.e->getTypeNamespace()) &&
                sParser->getNamespace() != ci->first.e->getTypeNamespace()) {
	
	      //here the type of the element is defined in another imported schemaparser
	      //so try to get the pointer.
	      if ( !sParser->isBasicType(t)){ 
	          t = (Schema::Type)sParser->getType(t)->getTypeId();
	          sParser = sParser->getImportedSchemaParser(ci->first.e->getTypeNamespace());
	      }
	    }
	   
	    serializeType(t,
			  ci->first.e->getName(),
			  sParser,
                          ci->first.e->getMin(),
                          ci->first.e->getMax(),
			  parents,
			  nsp);
	    sParser = s1Parser;
	  }
	  else if (ci->second==ContentModel::Container) {
	    
	    //nested xsd:sequence inside choice..nested content models
	    serializeContentModel(ci->first.c,
				  sParser,
				  parents,
				  nsp);
	      
	  }
	  else if (ci->second==ContentModel::ParticleGroup){
	    
	    //xsd:group inside 
	    serializeContentModel(ci->first.g->getContents(),
				  sParser,
				  parents,
				  nsp);
	  }
	}
	break;
      }
    }
}


void
WsdlInvoker::serializeParam(int n,const std::string & tag,
			    const SchemaParser * sParser,
			    const std::string nsp)
{

  std::string t=tag;
  if (tag=="*")
    t="item";
    
  for (int i = 0 ;i<elems_[n].n_;i++){

    if (style_ == Soap::DOC){
    
      xmlStream_->setPrefix(getPrefix(sParser->getNamespace()),
			    sParser->getNamespace());
      xmlStream_->startTag(sParser->getNamespace(),t);
    }
    else{
      
	xmlStream_->startTag(nsp,t);

      //xsi::type is needed for many SOAP servers
      if (sParser->isBasicType(elems_[n].type_)){

	xmlStream_->attribute(Schema::SchemaInstaceUri,
			      "type",
			      "xsd:"+sParser->getTypeName(elems_[n].type_));
      }
    }
    
    xmlStream_->text(elems_[n].data_[i]);
	
    if (style_ == Soap::DOC){
    
      xmlStream_->endTag(sParser->getNamespace(),t);
    }
    else{

      xmlStream_->endTag(nsp,t);
    }
  }
}


bool
WsdlInvoker::setInputValue(const int param,void** values,unsigned int occurs)
{

  if (occurs < elems_[param].min_ ||
      occurs > elems_[param].max_)
    return false;

  SchemaValidator *sv = new SchemaValidator (elems_[param].sParser_);
  for (unsigned int i = 0 ;i < occurs ;i++){
      
    TypeContainer * tc = sv->validate(values[i],
				      elems_[param].type_);
    if (!tc->isValueValid()){
	
      return false;
    }
    std::ostringstream oss;
    tc->print(oss);
    elems_[param].data_.push_back(oss.str());
    delete tc;
  }
  delete sv;
  
  elems_[param].n_ = occurs;
  return true;
}

bool
WsdlInvoker::setInputValue(const int param,std::vector<std::string> values)
{


  if (values.size() < elems_[param].min_ ||
      values.size() > elems_[param].max_)
    return false;

  SchemaValidator *sv = new SchemaValidator (elems_[param].sParser_);
  
  for (size_t i = 0 ;i < values.size() ;i++){
      
    TypeContainer * tc = sv->validate(values[i],
				      elems_[param].type_);
    if (!tc->isValueValid()){
	
      return false;
    }
    elems_[param].data_.push_back(values[i]);
    delete tc;
  }
  delete sv;
  
  elems_[param].n_ = values.size();
  return true;
}

bool
WsdlInvoker::setInputValue(const int param,std::string val)
{

  const SchemaParser* sParser = elems_[param].sParser_;
  SchemaValidator *sv = new SchemaValidator (sParser);
  Schema::Type t = elems_[param].type_;
  const XSDType * pType = sParser->getType(t);
  if (pType && !pType->isSimple()){

    if (pType->getContentModel() != Schema::Simple)
      return false;

    const ComplexType * ct = static_cast<const ComplexType*>(pType);
    t = (Schema::Type)ct->getContentType();
  }
  
  TypeContainer * tc = sv->validate(val,t);
  if (!(tc && tc->isValueValid())){
	
    return false;
  }
  if (elems_[param].data_.size() == 0)
    elems_[param].data_.push_back(val);
  else
    elems_[param].data_[0]=val;

  delete tc;
    
  delete sv;
  
  elems_[param].n_ = 1;
  return true;
}



bool
WsdlInvoker::setInputValue(const int param,void* val)
{

  const SchemaParser*  sParser = elems_[param].sParser_;
  SchemaValidator *sv = new SchemaValidator (sParser);
  Schema::Type t = elems_[param].type_;
  const XSDType * pType = sParser->getType(t);
  if (pType && !pType->isSimple()){

    if (pType->getContentModel() != Schema::Simple)
      return false;

    const ComplexType * ct = static_cast<const ComplexType*>(pType);
    t = (Schema::Type)ct->getContentType();
  }

  TypeContainer * tc = sv->validate(val,t);
  if (!(tc && tc->isValueValid())){
	
    return false;
  }
  std::ostringstream oss;
  tc->print(oss);
  if (elems_[param].data_.size() == 0)
    elems_[param].data_.push_back(oss.str());
  else
    elems_[param].data_[0]=oss.str();
  delete tc;
  delete sv;
  elems_[param].n_ = 1;
  return true;
}

bool
WsdlInvoker::setValue(const std::string & param,void* val)
{
  for (size_t s = 0;s<elems_.size();s++){
    
    if (elems_[s].tag_ == param)
      return setInputValue(s,val);
  }
  return false;
}

bool
WsdlInvoker::setValue(const std::string & param,void** values,unsigned int occur)
{

  for (size_t s = 0;s<elems_.size();s++){
    
    if (elems_[s].tag_ == param)
      return setInputValue(s,values,occur);
  }
  return false;
}
 
bool
WsdlInvoker::setValue(const std::string & param,std::string val)
{
  for (size_t s = 0;s<elems_.size();s++){
    
    if (elems_[s].tag_ == param)
      return setInputValue(s,val);
  }
  return false;
}

bool
WsdlInvoker::setValue(const std::string & param,std::vector<std::string> values)
{
  for (size_t s = 0;s<elems_.size();s++){
    
    if (elems_[s].tag_ == param)
      return setInputValue(s,values);
  }
  return false;
}


std::string
WsdlInvoker::getSoapMessage(){

  dontPost_ = true;
  invoke();
  return soapstr_->str();
}



bool
WsdlInvoker::invoke(long timeout)
{

  if (xmlStream_){
    
    delete xmlStream_;
	xmlStream_ = NULL;
  }
  if (soapstr_){
    
    delete soapstr_;
	soapstr_ = NULL;
  }
  if (results_){
    results_ = 0;
  }


  for (size_t x = 0;x<outputs_.size();x++)
    delete outputs_[x].second;

  outputs_.clear();

  soapstr_ = new std::ostringstream();
  xmlStream_ = new XmlSerializer(*soapstr_);

  serializeMode_ = true;

  xmlStream_->startDocument("UTF-8",false);
  xmlStream_->setPrefix("SOAP-ENV",Soap::soapEnvUri);
  xmlStream_->setPrefix("SOAP-ENC",Soap::soapEncUri);
  xmlStream_->setPrefix("xsd",Schema::SchemaUri);
  xmlStream_->setPrefix("xsi",Schema::SchemaInstaceUri);
  xmlStream_->setPrefix(getPrefix(nsp_),nsp_);
  xmlStream_->startTag(Soap::soapEnvUri,"Envelope");
  
  if (style_ == Soap::RPC) {
    
    xmlStream_->attribute(Soap::soapEnvUri,
			  "encodingStyle",
			  Soap::soapEncUri);
  }

  n_ = 0;
  if (soapheaders_){
    xmlStream_->startTag(Soap::soapEnvUri,"Header");
    serializeHeader();
    xmlStream_->endTag(Soap::soapEnvUri,"Header");
  }

  xmlStream_->startTag(Soap::soapEnvUri,"Body");
  if (style_ == Soap::RPC){
    
    xmlStream_->startTag(nsp_,op_->getName());
  }

  serialize();
  if (style_ == Soap::RPC){
    xmlStream_->endTag(nsp_,op_->getName());
  }

  xmlStream_->endTag(Soap::soapEnvUri,"Body");
  xmlStream_->endTag(Soap::soapEnvUri,"Envelope");
  xmlStream_->flush();
  


  //test
  //status_ = true;
  //processResults();
  //return status_;
  //test
  if (dontPost_)
    return true;

  post(timeout);
  if (results_){
    processResults();
    if (status_)
      return true;
  }
  else{
    
    logger_<<"Couldnt connect to "<<location_;
  }
    
  return false;
}

int
WsdlInvoker::getNextInput(std::string & param ,Schema::Type & type,int & minimum,int & maximum)
{
  std::vector<std::string> parents;
  return getNextInput(param, type, minimum, maximum, parents);
}

int
WsdlInvoker::getNextInput(std::string & param ,Schema::Type & type,int & minimum,int & maximum,
			  std::vector<std::string> & parents)
{
  if (n_ < elems_.size()){
    
    param = elems_[n_].tag_;
    type = elems_[n_].type_;
    minimum = elems_[n_].min_;
    parents = elems_[n_].parents_;
    maximum = elems_[n_].max_;
    return n_++;
  }
  else{
    return -1;
  }
}

int
WsdlInvoker::getNextHeaderInput(std::string & param ,Schema::Type & type,
				int & minimum,int & maximum)
{

  std::vector<std::string> parents;
  return getNextHeaderInput(param,type,minimum,maximum,parents);
}

int
WsdlInvoker::getNextHeaderInput(std::string & param ,Schema::Type & type,
				int & minimum,int & maximum,
				std::vector<std::string> & parents)
{
  static int  h=0;
  if (h<iHeaders_){
    param = elems_[h].tag_;
    type = elems_[h].type_;
    minimum = elems_[h].min_;
    maximum = elems_[h].max_;
    parents = elems_[h].parents_;
    return h++;
  }
  else{
    return -1;
  }
}

void
WsdlInvoker::processResults()
{
  try{

    const Message* m = op_->getMessage(WsdlPull::Output);
    std::istringstream respstr(results_);
    //    std::ifstream respstr("r.xml");//test
    XmlPullParser* xpp = new XmlPullParser(respstr);
    xpp->setFeature (FEATURE_PROCESS_NAMESPACES, true);
    xpp->require (XmlPullParser::START_DOCUMENT, "", "");

    while (xpp->getEventType () != XmlPullParser::END_DOCUMENT) {

      if (xpp->getEventType () == XmlPullParser::END_DOCUMENT)
	break;
      
      if (xpp->getEventType () == XmlPullParser::END_TAG &&
	  xpp->getName() == "Envelope" &&
	  xpp->getNamespace() ==  Soap::soapEnvUri)
	break;
	
      if (xpp->getEventType () != XmlPullParser::START_TAG){
	xpp->nextToken ();
	continue;
      }
      
      xpp->nextTag ();
      Qname elemName (xpp->getName ());
      elemName.setNamespace(xpp->getNamespace());
      
      if (elemName.getNamespace() == Soap::soapEnvUri){
	
	if (elemName.getLocalName() == "Fault"){
	  processFault(xpp);
	  status_ = false;
	  return;
	} 
	else if (elemName.getLocalName() == "Header"){

	  processHeader(xpp);
	}
	else if (elemName.getLocalName() == "Body"){

	  xpp->nextTag();
	  processBody(m,xpp);
	}
	continue; //ignore soap:Body and soap:Envelope tags
      }
    }
    delete xpp;
    n_ = oHeaders_;
  }
  catch (WsdlException we)
    {
      logger_<<"A WSDL exception occurred while parsing the response at line "<<we.line
	     <<":"<<we.col<<std::endl;
      logger_<<we.description<<std::endl;
      status_ =false;
    }
  catch (SchemaParserException spe)
    {
      logger_<<"A Schema Parser exception occurred while parsing the response at line "<<spe.line
	     <<":"<<spe.col<<std::endl;
      logger_<<spe.description<<std::endl;
      status_ =false;
    }
  catch (XmlPullParserException xpe)
    {
      logger_<<"An Xml Parsing exception occurred while parsing the response at line "<<xpe.line
	     <<":"<<xpe.col<<std::endl;
      logger_<<xpe.description<<std::endl;
      status_ =false;
    }
  return;
}

WsdlInvoker::~WsdlInvoker()
{
  reset();
  if (ourParser_){  
      delete ourParser_;
  }
  if (xmlStream_){
    
    delete xmlStream_;
  }
  if (soapstr_){
    
    delete soapstr_;
  }
}

void
WsdlInvoker::reset()
{
  n_ =  iHeaders_ = oHeaders_ = 0;
  elems_.clear();

  for (size_t x = 0;x<outputs_.size();x++)
    delete outputs_[x].second;

  outputs_.clear();
  serializeMode_ = false;
}

bool 
WsdlInvoker::getNextOutput(std::string & name,TypeContainer * & tc)
{
  if (status_ && n_ < outputs_.size()){
    
    name = outputs_[n_].first;
    tc = outputs_[n_].second;
    n_++;
    return true;
  }
  n_ = oHeaders_;
  return false;
}


TypeContainer*
WsdlInvoker::getOutput(const std::string  & name)
{
  for (unsigned int i = 0 ;status_ && i <outputs_.size();i++){

    if ( name == outputs_[i].first)
      return outputs_[i].second;
  }
  return 0;
}

bool
WsdlInvoker::getNextHeaderOutput(std::string & name,TypeContainer*& tc)
{
  static int j = 0;
  if(j<oHeaders_){
    name = outputs_[j].first;
    tc = outputs_[j].second;
    j++;
    return true;
  }
  else{
    j = 0;
    return false;
  }
}

void * 
WsdlInvoker::getValue(const std::string  & name ,Schema::Type & t)
{
  for (unsigned int i = 0 ;status_ && i <outputs_.size();i++){

    if (outputs_[i].second!=0){
      outputs_[i].second->rewind();
      void * tmp= outputs_[i].second->getValue(name,t);
      if (tmp)
	return tmp;
    }
  }
  return 0;
}



void
WsdlInvoker::post(long timeout, std::string username, std::string passwd)
{
  const std::string  postData = soapstr_->str();
  if(verbose_){
    
    std::ofstream ofs("request.log",std::ios::app);
    ofs<<postData;
    ofs<<std::endl;
    ofs.flush();
  }
    
#ifdef WITH_CURL
  CURL * ctx=0;
  CURLcode res;
  curl_global_init( CURL_GLOBAL_ALL ) ;
  ctx=curl_easy_init();
  int bufsize = 0;
  if (!ctx)
    return ;
  curl_easy_setopt( ctx , CURLOPT_URL,  location_.c_str()) ;

  curl_easy_setopt( ctx , CURLOPT_NOPROGRESS , 1 ) ;
  if(timeout){
    curl_easy_setopt( ctx ,CURLOPT_TIMEOUT, timeout);
  }

  if (verbose_) {
    curl_easy_setopt( ctx , CURLOPT_VERBOSE,1);
    curl_easy_setopt( ctx , CURLOPT_NOPROGRESS , 0 ) ;
  }

  curl_easy_setopt( ctx , CURLOPT_POST , 1 );
  curl_easy_setopt( ctx , CURLOPT_POSTFIELDS , postData.c_str()) ;
  curl_slist* responseHeaders = NULL ;
  std::string tmp="SOAPAction: ";
  tmp.push_back('"');
  tmp+=action_;
  tmp.push_back('"');
  responseHeaders = curl_slist_append( responseHeaders , tmp.c_str());
  responseHeaders = curl_slist_append( responseHeaders ,"Content-Type: text/xml; charset=UTF-8");
  responseHeaders = curl_slist_append( responseHeaders ,"Accept: text/xml;");
  curl_easy_setopt( ctx , CURLOPT_HTTPHEADER , responseHeaders ) ;
  tmp = "wsdlpull";
#ifdef HAVE_CONFIG_H
  tmp=tmp+"/"+VERSION;
#endif
  curl_easy_setopt( ctx,CURLOPT_USERAGENT,tmp.c_str());
  curl_easy_setopt( ctx,CURLOPT_POSTFIELDSIZE,postData.length());
  
  if (XmlUtils::getProxy()){
    curl_easy_setopt(ctx,CURLOPT_PROXY,XmlUtils::getProxyHost().c_str());
    tmp=XmlUtils::getProxyUser()+":"+XmlUtils::getProxyPass();
    curl_easy_setopt(ctx,CURLOPT_PROXYUSERPWD,tmp.c_str());
  }
  curl_easy_setopt( ctx ,CURLOPT_WRITEDATA ,&bufsize) ;
  curl_easy_setopt( ctx ,CURLOPT_WRITEFUNCTION,storeResults) ;
  
  // std::logger_ << "- - - BEGIN: response - - -" << std::endl ;
  res=curl_easy_perform(ctx);
  //  std::logger_ << "- - - END: response - - -" << std::endl ;

  curl_slist_free_all( responseHeaders ) ;
  curl_easy_cleanup( ctx ) ;
  curl_global_cleanup() ;
  

#elif _WIN32
  XmlUtils::winPost(location_,username,passwd,postData,action_,results_);
#endif

  if(verbose_ && results_){
    
    std::ofstream ofs("response.log",std::ios::app);
    ofs<<results_;
    ofs<<std::endl;
    ofs.flush();
  }

}

void
WsdlInvoker::printTypeNames(bool f)
{
  TypeContainer::printTypeNames_ = false;
}


void
WsdlInvoker::processFault(XmlPullParser* xpp)
{

  while (!(xpp->getEventType () == XmlPullParser::END_TAG &&
	   xpp->getName() == "Fault")) {
    
    if (xpp->getEventType() == XmlPullParser::START_TAG &&
	xpp->getName() == "faultcode"){
      
      xpp->next();
      logger_<<"SOAP Fault Code: "<<xpp->getText()<<std::endl;
    }
    
    if (xpp->getEventType() == XmlPullParser::START_TAG &&
	xpp->getName() == "faultstring"){
      
      xpp->next();
      logger_<<"SOAP Fault String: "<<xpp->getText()<<std::endl;
    }
    if (xpp->getEventType() == XmlPullParser::START_TAG &&
	xpp->getName() == "faultactor"){
      
      xpp->next();
      logger_<<"SOAP Fault Actor: "<<xpp->getText()<<std::endl;
    }
    xpp->next();
  }
}

void 
WsdlInvoker::processBody(const Message* m,
			 XmlPullParser* xpp)
{
  
  if (xpp->getName() == "Fault") {
    
    processFault(xpp);
    status_ = false;
    return;
  }

  if (style_ == Soap::RPC && use_==Soap::ENCODED){
	  
    if (xpp->getName () == op_->getName()+"Response") {

      //operation's name followed by 'Response' must be the containing element
      xpp->nextTag ();
      
      do {
      
	
	//first look for xsi:type
	Qname typ(xpp->getAttributeValue(Schema::SchemaInstaceUri, "type"));
	typ.setNamespace(xpp->getNamespace(typ.getPrefix()));
	const SchemaParser * sParser = 0;
	int typeId = 0;
	  
	if (!(typ.getNamespace() == Soap::soapEncUri &&
	      typ.getLocalName() == "Array"))//for soap array just use the part's type info
	  sParser= wParser_->getSchemaParser(typ.getNamespace());
	  
	if (sParser){
	    
	  typeId = (const_cast<SchemaParser*>(sParser))->getTypeId(typ);
	}
	else{

	  //if xsi:type doesnt give a clue then see if the part name matches
	  const Part * p = m->getMessagePart(xpp->getName ());
	  if (p){
	    
	    sParser = wParser_->getSchemaParser(p->schemaId());
	    typeId = p->type();
	  }else {


	  }
	}
	if (sParser && typeId !=0){  
	    
	  SchemaValidator * sv= new SchemaValidator(sParser);	  
	  std::string tag = xpp->getName();
	  TypeContainer * t = sv->validate (xpp, typeId);
	  outputs_.push_back(std::pair<std::string,TypeContainer*>(tag,t));
	  xpp->nextTag();
	  delete sv;
	}
	else{

	  status_ = false;
	  logger_<<"Unknown element "<<xpp->getName()<<std::endl;
	  return;
	}
      } while (!(xpp->getName() == op_->getName()+"Response" &&
		 xpp->getEventType() == XmlPullParser::END_TAG));
    }
  }
  else{
    
    while (!(xpp->getName() == "Body" && 
	     xpp->getNamespace() == Soap::soapEnvUri &&
	     xpp->getEventType() == XmlPullParser::END_TAG)) {
	     
      Qname elemName (xpp->getName ());
      elemName.setNamespace(xpp->getNamespace());

      //doc/literal has ref type element in the part
      const SchemaParser * sParser =
	wParser_->getSchemaParser(elemName.getNamespace());
      if (!sParser){
	  
	status_ = false;
	logger_<<"Unknown element "<<elemName<<std::endl;
	return;
      }
      SchemaValidator * sv= new SchemaValidator(sParser);	  
	  
      const Element * e = sParser->getElement (elemName);
      if(e){
	int typeId = e->getType () ;
	TypeContainer * t = sv->validate (xpp, typeId);
	std::pair<std::string,TypeContainer*> pr(elemName.getLocalName(),t);
	outputs_.push_back(pr);
      }
      else{
	status_ = false;
	std::cerr<<"Unkown element "<<elemName.getLocalName()<<std::endl;
	return;
      }
      delete sv;
      xpp->nextTag();
    }
  }
  status_ = true;
}

void
WsdlInvoker::processHeader(XmlPullParser *xpp)
{
  Qname elem;
  const SchemaParser * sParser = 0;
  int type = Schema::XSD_INVALID;
  xpp->nextTag ();
  std::string tag = xpp->getName();

  while (!(xpp->getEventType() == XmlPullParser::END_TAG &&
	   xpp->getName() == "Header")){


    //first look for xsi:type
    if (xpp->getAttributeValue(Schema::SchemaInstaceUri, "type") != "" ) {
      
      elem = Qname(xpp->getAttributeValue(Schema::SchemaInstaceUri, "type"));
      elem.setNamespace(xpp->getNamespace(elem.getPrefix()));
      sParser= wParser_->getSchemaParser(elem.getNamespace());
      type = (const_cast<SchemaParser*>(sParser))->getTypeId(elem);  
    }
    else {

      elem = Qname(xpp->getName());
      elem.setNamespace(xpp->getNamespace());      
      sParser=wParser_->getSchemaParser(elem.getNamespace());
      const Element * e = sParser->getElement (elem);
      if(e){
	     type = e->getType ();
      }
    }
    SchemaValidator * sv= new SchemaValidator(sParser);	  
    TypeContainer * t = sv->validate (xpp, type);
    outputs_.push_back(std::pair<std::string,TypeContainer*>(tag,t));
    oHeaders_++; 
    xpp->nextTag();
    delete sv;
  }
  xpp->nextTag();   
}

bool
WsdlInvoker::isSoapArray (const ComplexType * ct,
			  const SchemaParser * sParser)
{
  const XSDType * baseType=sParser->getType(ct->getBaseTypeId());
  if (baseType) {
    if(baseType->getNamespace()==Soap::soapEncUri &&
       baseType->getName()=="Array")
      return true;
  }
  return false;
}

void
WsdlInvoker::setCredentials(const std::string & user, const std::string & pass)
{
  username_ = user;
  password_ = pass;
  XmlUtils::setProxyUser(user);
  XmlUtils::setProxyPass(pass);
  XmlUtils::setProxy(true);
}

void
WsdlInvoker::setProxy(const std::string & host,int  port)
{
  host_ = host;
  port_ = port;
  std::ostringstream oss;
  oss<<host<<":"<<port;
  XmlUtils::setProxyHost(oss.str());
  XmlUtils::setProxy(true);
}

std::string
WsdlInvoker::getPrefix(const std::string & nsp)
{
  
  unsigned int i = 0;
  char prefix='1';
  while (i<prefixes_.size()) {
    if (prefixes_[i] == nsp)
      break;
    i++;
  }

  std::string tmp("ns");
  tmp.append(1,prefix+i);
  if (i == prefixes_.size())
    prefixes_.push_back(nsp);
  
  return tmp;

}

}

size_t
storeResults(void * buf,size_t sz,size_t nmemb,void* userdata)
{
  int *bufsize= (int*)userdata;
  if (results_ == 0){
    
    results_ = (char*)malloc(sizeof(char) * sz * nmemb);
  }
  else{
    results_ = (char*) realloc(results_,sizeof(char) * sz * nmemb+ (*bufsize));
  }
  memcpy (results_+(*bufsize),buf,sz*nmemb);
  *bufsize+=sz*nmemb;
  return sz*nmemb;
}
