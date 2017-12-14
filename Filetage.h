#ifndef CPP_FILETAGE_H
#define CPP_FILETAGE_H

#include <windows.h>
#include <functional>
#include <memory>
#include <chrono>
#include <system_error>
#include <cerrno>
#include <process.h>

#define FILETAGE_INVALID_HANDLE 0
class Filetage
{
public:
    class id
    {
        DWORD mId;
        void clear() {mId = 0;}
        friend class Filetage;
    public:
        explicit id(DWORD aId=0):mId(aId){}
        bool operator==(const id& other) const {return mId == other.mId;}
    };
protected:
    HANDLE mHandle;
    id mFiletageId;
public:
    typedef HANDLE native_handle_type;
    id get_id() const noexcept {return mFiletageId;}
    native_handle_type native_handle() const {return mHandle;}
    Filetage(): mHandle(FILETAGE_INVALID_HANDLE){}

    Filetage(Filetage&& other)
    :mHandle(other.mHandle), mFiletageId(other.mFiletageId)
    {
        other.mHandle = FILETAGE_INVALID_HANDLE;
        other.mFiletageId.clear();
    }

    Filetage(const Filetage &other)=delete;

    template<class Function, class... Args>
    explicit Filetage(Function&& f, Args&&... args)
    {
        typedef decltype(std::bind(f, args...)) Call;
        Call* call = new Call(std::bind(f, args...));
        mHandle = (HANDLE)_beginthreadex(NULL, 0, FiletageCall<Call>,
            (LPVOID)call, 0, (unsigned*)&(mFiletageId.mId));
        if (mHandle == FILETAGE_INVALID_HANDLE)
        {
            int errnum = errno;
            delete call;
            throw std::system_error(errnum, std::generic_category());
        }
    }
    template <class Call>
    static unsigned int __stdcall FiletageCall(void* arg)
    {
        std::unique_ptr<Call> upCall(static_cast<Call*>(arg));
        (*upCall)();
        return (unsigned long)0;
    }
    bool joinable() const {return mHandle != FILETAGE_INVALID_HANDLE;}
    void join()
    {
        if (get_id() == id(GetCurrentThreadId()))
            throw std::system_error(EDEADLK, std::generic_category());
        if (mHandle == FILETAGE_INVALID_HANDLE)
            throw std::system_error(ESRCH, std::generic_category());
        if (!joinable())
            throw std::system_error(EINVAL, std::generic_category());
        WaitForSingleObject(mHandle, INFINITE);
        CloseHandle(mHandle);
        mHandle = FILETAGE_INVALID_HANDLE;
        mFiletageId.clear();
    }

    ~Filetage()
    {
        if (joinable())
            std::terminate();
    }
    Filetage& operator=(const Filetage&) = delete;
    Filetage& operator=(Filetage&& other) noexcept
    {
        if (joinable())
          std::terminate();
        swap(std::forward<Filetage>(other));
        return *this;
    }
    void swap(Filetage&& other) noexcept
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mFiletageId.mId, other.mFiletageId.mId);
    }
    static unsigned int hardware_concurrency() noexcept
    {
        static int ncpus = -1;
        if (ncpus == -1)
        {
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            ncpus = sysinfo.dwNumberOfProcessors;
        }
        return ncpus;
    }
    void detach()
    {
        if (!joinable())
            throw std::system_error(EINVAL, std::generic_category());
        if (mHandle != FILETAGE_INVALID_HANDLE)
        {
            CloseHandle(mHandle);
            mHandle = FILETAGE_INVALID_HANDLE;
        }
        mFiletageId.clear();
    }
};

#endif // CPP_FILETAGE_H