/******************************************************************************
 * File: 	CBlowFishSimple
 * Author: 	Louis Huang
 * Des: 	Use blowfish to dec and enc string
 * ******************************************************************************/

#ifndef CBLOWFISHSIMPLE_H
#define CBLOWFISHSIMPLE_H

#include <string>
#include <core/rt2_core.h>

class CBlowFishSimple
{
public:
	CBlowFishSimple(const std::string strKey);
	~CBlowFishSimple();

	std::string
		EncryptHexString(std::string strPlain);
	std::string
		DecryptHexString(std::string strCipher);

	bool
		Encrypt(const char* pPlain, int planLen, char* pCipher, int& cipherLen);
	bool
		Decrypt(const char* pCipher, int cipherLen, char* pPlain, int& planLen);

	static std::string ASCIIToHex(const char* pInput, size_t datalen);
	static void HexToASCII(char* pOut, std::string strInput, size_t inputlen);

private:
	
	inline
	void _encrypt8byte(char* szMessage)
	{
		_encipher((unsigned long&)szMessage[0],(unsigned long&)szMessage[4]);
	}

	inline
	void _decrypt8byte(char* szMessage)
	{
		_decipher((unsigned long&)szMessage[0],(unsigned long&)szMessage[4]);
	}

	template <class T> void _swap(T& x,T& y)
	{
		T temp;
		temp=x;
		x=y;
		y=temp;
	};

	void _encipher(unsigned long& XL,unsigned long& XR);
	unsigned long _f(const char Bytes[4]);
	void _decipher(unsigned long& XL,unsigned long& XR);


private:
	static unsigned long m_sbox[4][256];
	static unsigned long m_pbox[18];
	unsigned long m_keybox[18+4*256];
};

#endif //CBLOWFISHSIMPLE_H

