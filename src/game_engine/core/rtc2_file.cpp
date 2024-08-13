#include "core/rt2_core.h"

namespace rt2_core {

bool RtArchive::ReadObject(RtObject* _obj, const RtRuntimeClass* pClass) {
    RTASSERT(IsLoading());
    RTASSERT(_obj);

    RtRuntimeClass* pRefClass = NULL;

    if (pClass == NULL) {
        UINT nSchema;
        pRefClass = ReadClass(&nSchema);
    } else {
        pRefClass = (RtRuntimeClass*)pClass;
    }

    if (!pRefClass) {
        RtCoreLog().Error("Cannot call ReadObject.\n");
        return false;
    }

    if (!_obj->IsKindOf(pRefClass)) {
        RtCoreLog().Error("ReadObject() RuntimeClass No Match\n");
        return false;
    }

    if (_obj->Serialize(*this) == 0) {
        RtCoreLog().Error("读取Object [%s]失败.\n", pRefClass->m_lpszClassName);
        return false;
    }

    return true;
}

RtObject* RtArchive::ReadObject(const RtRuntimeClass* pClass) {
    CHECK(this->IsLoading());
    RtRuntimeClass* pRefClass;
    if (pClass == 0) {
        UINT nSchema;
        pRefClass = ReadClass(&nSchema);
    } else {
        pRefClass = (RtRuntimeClass*)pClass;
    }
    if (pRefClass == 0) {
        RtCoreLog().Warn("Warning: Cannot call ReadObject.\n");
        return 0;
    }
    RtObject* pObj;
    pObj = pRefClass->CreateObject();
    if (!pObj)
        return 0;
    if (pObj->Serialize(*this) == 0) {
        DEL_ONE(pObj);
        RtCoreLog().Warn("ERROR: 读取Object [%s]失败.\n", pRefClass->m_lpszClassName);
        return 0;
    }
    return pObj;
}

RtObject* RtArchive::NewObject() {
    RTASSERT(IsLoading());

    UINT            nSchema;
    RtRuntimeClass* pRefClass = ReadClass(&nSchema);
    return pRefClass->CreateObject();
}

RtArchive& RtArchive::operator<<(class RtObject*& Res) {
    Res->Serialize(*this);
    return *this;
}

bool RtArchive::WriteObject(RtObject* pObj) {
    CHECK(this->IsStoring());
    WriteClass(pObj->GetRuntimeClass());
    if (pObj->Serialize(*this) == 0) {
        RtCoreLog().Warn("ERROR: 写入Object [%s]失败.\n", pObj->GetRuntimeClass()->m_lpszClassName);
        return false;
    }
    return true;
}

RtRuntimeClass* RtArchive::ReadClass(UINT* pSchema) {
    CHECK(this->IsLoading());
    RtRuntimeClass* pRefClass = RtRuntimeClass::Load(*this, pSchema);
    return pRefClass;
}

void RtArchive::WriteClass(const RtRuntimeClass* pClassRef) {
    CHECK(this->IsStoring());
    pClassRef->Store(*this);
}

/*------------------------------------------------------------------------
	RtMemoryArchive
------------------------------------------------------------------------*/

RtMemoryArchive::RtMemoryArchive(char* pBuf, int iSize) {
    m_nMode     = RtArchive::load;
    m_pAllocMem = NULL;
    m_buf       = pBuf;
    m_pos       = pBuf;
    m_dataSize  = iSize;
    m_bufSize   = iSize;
}

RtMemoryArchive::~RtMemoryArchive() {
    Close();
}

bool RtMemoryArchive::InitReader(int size) {
    m_nMode = RtArchive::load;
    AllocBuffer(size);
    m_dataSize = size;
    m_pos      = m_buf;
    return true;
}

bool RtMemoryArchive::InitWriter(int size) {
    m_nMode = RtArchive::store;
    AllocBuffer(size);
    m_pos      = m_buf;
    m_dataSize = 0;
    return true;
}

void RtMemoryArchive::Seek(int InPos, SeekMode eSeek) {
    if (IsOverflow(InPos))
        return;
    switch (eSeek) {
        case SeekCurrent:
            m_pos = m_pos + InPos;
            break;

        case SeekEnd:
            m_pos = m_buf + (m_dataSize - InPos);
            break;

        case SeekBegin:
        default:
            m_pos = m_buf + InPos;
            break;
    }
}

int RtMemoryArchive::Tell() {
    return m_pos - m_buf;
}

int RtMemoryArchive::TotalSize() {
    return m_dataSize;
}

BOOL RtMemoryArchive::Close() {
    if (m_pAllocMem) {
        RtCoreMem().Free(m_pAllocMem);
    }
    m_pAllocMem = NULL;
    m_buf       = NULL;
    m_dataSize  = 0;
    return !m_bIsError;
}

void RtMemoryArchive::Flush() {}

long RtMemoryArchive::Serialize(void* V, int iLength) {
    if (IsOverflow(m_pos - m_buf + iLength)) {
        return 0;
    }

    if (IsLoading()) {
        memcpy(V, m_pos, iLength);
    } else {
        memcpy(m_pos, V, iLength);
    }

    m_pos += iLength;
    if (IsStoring() && m_pos - m_buf > m_dataSize) {
        m_dataSize = m_pos - m_buf;
    }
    return iLength;
}

char* RtMemoryArchive::AllocBuffer(int size) {
    m_buf = m_pAllocMem = (char*)RtCoreMem().Alloc(size, __FILE__, __LINE__);
    m_bufSize           = size;
    return m_buf;
}

char* RtMemoryArchive::GetBuffer() {
    return m_buf;
}

int RtMemoryArchive::GetBufferSize() {
    return m_bufSize;
}

int RtMemoryArchive::GetDataSize() {
    return m_dataSize;
}

bool RtMemoryArchive::RellocBuffer(int min) {
    char* old    = m_buf;
    int   offset = m_pos - m_buf;
    int   size   = m_bufSize * 2;
    while (size < min)
        size *= 2;

    m_buf = m_pAllocMem = (char*)RtCoreMem().Alloc(size, __FILE__, __LINE__);
    memcpy(m_buf, old, m_dataSize);
    m_bufSize = size;
    m_pos     = m_buf + offset;
    RtCoreMem().Free(old);
    return true;
}

bool RtMemoryArchive::IsOverflow(int offset) {
    if (offset < 0) {
        m_bIsError = TRUE;
        RtCoreLog().Error("RtMemoryArchive: ar = memory overflow,m_size = %d,offset = %d",
                          m_dataSize, offset);
        return true;
    }

    if (IsLoading()) {
        if (offset > m_dataSize) {
            m_bIsError = TRUE;
            RtCoreLog().Error("RtMemoryArchive: ar = memory overflow,m_size = %d,offset = %d",
                              m_dataSize, offset);
            return true;
        }
    } else {
        // check if need realloc buffer
        if (offset > m_bufSize)
            RellocBuffer(offset);
        return false;
    }
    return false;
}

/*------------------------------------------------------------------------
	RtFileManager
------------------------------------------------------------------------*/

void RtFileManager::CloseFile(RtArchive* pFileArchive) {
    DEL_ONE(pFileArchive);
}

bool RtFileManager::CreateAsyncFileRender(
    const char*             pFilename,   // 文件名
    TCBAsyncFileRender      pCBRead,     // 读取Callback, 如果读取失败, pArch返回NULL
    TCBAsyncFileRenderState pCBState,    // 状态Callback
    int                     iParam1,     // 参数1
    int                     iParam2,     // 参数2
    DWORD                   dwReadFlags  // 标志
) {
    CHECK(pFilename != NULL);
    CHECK(pCBRead != NULL);
    if (strnicmp(pFilename, "http://", 7) == 0)  // 超文本传输协议
    {
        return false;
    } else if (strnicmp(pFilename, "gttp://", 7) == 0)  // 游戏文本传输协议
    {
        return false;
    } else {
        RtArchive* pArc = CreateFileReader(pFilename, dwReadFlags);
        if (pCBState)
            pCBState(pFilename, pArc ? FRS_COMPLETE : FRS_FAIL, iParam1, iParam2);
        pCBRead(pArc, pFilename, iParam1, iParam2);
        return true;
    }
}

RT2_CORE_API const char* rtGetFileExt(const char* szFilename) {
    static char szExt[20];
    rtfSplitpath(szFilename, NULL, NULL, NULL, szExt);
    return szExt;
}

RT2_CORE_API void rtfSplitpath(const char* path, char* drive, char* dir, char* fname, char* ext) {
    CHECK(path != NULL);
    const char *pt = path, *pe, *pf, *pd;
    if (path[0] && path[1] == ':') {
        pt = path + 2;
        if (drive) {
            drive[0] = path[0];
            drive[1] = path[1];
            drive[2] = 0;
        }
    } else {
        if (drive)
            drive[0] = 0;
    }
    if (pt[0]) {
        pe = pt;
        while (pe[1])
            pe++;
        if (rtChrInStr(PATH_SEPARATOR, *pe) != -1) {
            if (dir)
                strcpy(dir, pt);
            if (fname)
                fname[0] = 0;
            if (ext)
                ext[0] = 0;
        } else {
            pf = rtStrRChrs(path, PATH_SEPARATOR);
            if (pf) {
                if (dir) {
                    //strncpy(dir, pt, pf-pt+1); dir[pf-pt+1]=0;
                    rt2_strncpy2(dir, pt, pf - pt + 1);
                    dir[pf - pt + 1] = 0;
                }
                pf++;
                pd = rtStrRChr(pf, '.');
                if (pd && pd > pf) {
                    if (fname) {
                        //strncpy(fname, pf, pd-pf); fname[pd-pf]=0;
                        rt2_strncpy2(fname, pf, pd - pf);
                        fname[pd - pf] = 0;
                    }
                    if (ext) {
                        //strncpy(ext, pd+1, pe-pd); ext[pe-pd]=0;
                        rt2_strncpy2(ext, pd + 1, pe - pd);
                        ext[pe - pd] = 0;
                    }
                } else {
                    if (fname)
                        strcpy(fname, pf);
                    if (ext)
                        ext[0] = 0;
                }
            } else {
                /*
                if (dir)
                {
                    strcpy(dir, pt);
                    char* pdir = dir+strlen(dir);
                    pdir[0] = '/';
                    pdir[1] = 0;
                }
                if (fname) fname[0] = 0;
                if (ext)   ext[0] = 0;
                */
                if (dir)
                    dir[0] = 0;
                pd = rtStrRChr(path, '.');
                if (pd && pd > path) {
                    if (fname) {
                        //strncpy(fname, path, pd-path); fname[pd-path]=0;
                        rt2_strncpy2(fname, path, pd - path);
                        fname[pd - path] = 0;
                    }
                    if (ext) {
                        //strncpy(ext, pd+1, pe-pd); ext[pe-pd]=0;
                        rt2_strncpy2(ext, pd + 1, pe - pd);
                        ext[pe - pd] = 0;
                    }
                } else {
                    if (fname)
                        strcpy(fname, path);
                    if (ext)
                        ext[0] = 0;
                }
            }
        }
    } else {
        if (dir)
            dir[0] = 0;
        if (fname)
            fname[0] = 0;
        if (ext)
            ext[0] = 0;
    }
}

RT2_CORE_API void rtfMakepath(char* path, const char* drive, const char* dir, const char* fname,
                              const char* ext) {
    CHECK(path != NULL);
    path[0] = 0;
    if (drive)
        strcat(path, drive);
    if (dir && dir[0]) {
        strcat(path, dir);
        int iLen = (int)strlen(path);
        if (rtChrInStr(PATH_SEPARATOR, path[iLen - 1]) == -1) {
            strcat(path, "/");
        }
    }
    if (fname)
        strcat(path, fname);
    if (ext && ext[0]) {
        strcat(path, ".");
        strcat(path, ext);
    }
}

}  // namespace rt2_core

/*-----------------------------------------------------------------------------
-   The End.
-----------------------------------------------------------------------------*/
