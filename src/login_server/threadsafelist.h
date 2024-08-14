#ifndef __THREAD_SAFE_LIST_H__
#define __THREAD_SAFE_LIST_H__

#include <list>
#include <mutex>

template <class T>
class CThreadSafeList {
   public:
    CThreadSafeList();
    virtual ~CThreadSafeList();

    int  GetCmdNum() const;
    bool AddCmd(const T& cmd);
    bool GetCmd(T& cmd);
    bool SetMaxCmdNum(int num);

   private:
    mutable std::mutex m_mutex;  // 互斥锁
    std::list<T>       m_cmdList;
    int                m_maxCmd;
    std::atomic<int>   m_cmdNum;
};

template <class T>
CThreadSafeList<T>::CThreadSafeList() : m_maxCmd(1000), m_cmdNum(0) {
    // 初始化成员变量
}

template <class T>
CThreadSafeList<T>::~CThreadSafeList() {
    // 不再需要手动清理，因为std::list会自动管理内存
}

template <class T>
bool CThreadSafeList<T>::SetMaxCmdNum(int num) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxCmd = num;
    return true;
}

template <class T>
bool CThreadSafeList<T>::GetCmd(T& cmd) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cmdNum <= 0) {
        return false;
    }
    cmd = std::move(m_cmdList.front());
    m_cmdList.pop_front();
    m_cmdNum--;
    return true;
}

template <class T>
bool CThreadSafeList<T>::AddCmd(const T& cmd) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_cmdNum >= m_maxCmd) {
        return false;
    }

    m_cmdList.emplace_back(cmd);
    m_cmdNum++;
    return true;
}

template <class T>
int CThreadSafeList<T>::GetCmdNum() const {
    return m_cmdNum.load();  // 读取原子变量的值
}

#endif
