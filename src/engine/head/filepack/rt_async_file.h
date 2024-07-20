#ifndef _INC_RT_ASYNC_FILE_H_
#define _INC_RT_ASYNC_FILE_H_

#include "memory_file.h"

class RtAsyncFile : public RtArchive
{
private:
    
};

class RtPreLoadFileThread
{
private:
	typedef unordered_map<string, Buffer> BufferTable;
	
	struct Buffer
	{
		string	filename;
		char*	data;
		int		size;
		int		used;	// 0 cached, -1 pending, > 0 used
	};

public:
	void Startup();

	void Shutdown();

	Buffer*	LookupBuffer(const string& filename)
	{
		BufferTable::iterator iter = m_buffer.find(filename);

		if (iter == m_buffer.end())
			return NULL;

		return iter->second;
	}

	void*	Open(const string& filename)
	{
		Buffer* buffer = LookupBuffer(filename);

		if (buffer == NULL)
			return NULL;

		return NULL;
	}

	void	PreLoad(const string& filename)
	{
		Buffer* buffer = LookupBuffer(filename);

		if (buffer == NULL)
		{
			buffer = RT_NEW Buffer;			
			buffer->data = NULL;
			buffer->size = 0;
			buffer->used = -1;

			m_buffer[filename] = buffer; 

			m_queue.push_back(buffer);
		}		
	}

	void	GarbageCollect()
	{
		while(m_cachingSize > m_capacity && !m_cached.empty())
		{
			Buffer* buffer = m_cached.back();

			m_cachingSize -= buffer->size;

			DEL_ONE(buffer);

			m_cached.pop_back();
		}
	}

	static DWORD RunCB()
	{
		return 0;
	}

	void	Run()
	{
		for (;;)
		{
			WaitForSingleObject(m_event, INFINITE);

			Buffer* buffer = m_cached.front();

			m_cached.pop_front();

			RtArchive* pArchive;

			pArchive = m_ansi.CreateFileReader(buffer->filename.c_str(), 0, NULL);

			if (pArchive == NULL)
				pArchive = m_pack.CreateFileReader(buffer->filename.c_str(), 0, NULL);

			if (pArchive == NULL)
				continue;

			buffer->size = pArchive->TotalSize();
			buffer->data = RT_NEW char[buffer->size];

			pArchive->Serialize(buffer->data, buffer->size);

			DEL_ONE(pArchive);

			buffer->used = 0;

			m_cached.push_front(buffer);
		}
	}

public:
	RtPreLoadFileThread()
		: m_hThread(NULL)
		, m_event(NULL)
	{
	}

	virtual ~RtPreLoadFileThread();

private:
	BufferTable<string, Buffer>	m_buffer;
	list<Buffer*>				m_queue;	
	list<Buffer*>				m_cached;
	int							m_capacity;
	int							m_cachingSize;
	RtFileManagerAnsi			m_ansi;	
	CRtPackManager				m_pack;
	HEVENT						m_event;
	HANDLE						m_hThread;
};

#endif // _INC_RT_ASYNC_FILE_H_
