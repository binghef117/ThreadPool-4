#ifndef CTHREADPOOLITEM_RET
#define CTHREADPOOLITEM_RET
#include<memory>
#include"../../lib/header/tool/CScopeGuard.h"
#include"CThreadPoolItemExecutor_Ret.h"
#include"IThreadPoolItem.h"

namespace nThread
{
	template<class Ret>
	class CThreadPoolItem_Ret:public IThreadPoolItemBase
	{
		std::unique_ptr<CThreadPoolItemExecutor_Ret<Ret>> exec_;
		CThreadQueue<CThreadPoolItem_Ret<Ret>*> *waitingQue_;
	public:
		CThreadPoolItem_Ret(CThreadQueue<CThreadPoolItem_Ret<Ret>*> *);
		CThreadPoolItem_Ret(const CThreadPoolItem_Ret &)=delete;
		CThreadPoolItem_Ret(CThreadPoolItem_Ret &&) noexcept=default;
		void assign(std::unique_ptr<CThreadPoolItemExecutor_Ret<Ret>> &&);
		inline Ret get()
		{
			const nTool::CScopeGuard<void()> sg{[=]{waitingQue_->emplace(this);}};
			return exec_->get();
		}
		bool is_running() const noexcept override
		{
			return exec_->is_running();
		}
		void wait() const override
		{
			exec_->wait();
		}
		CThreadPoolItem_Ret& operator=(const CThreadPoolItem_Ret &)=delete;
		CThreadPoolItem_Ret& operator=(CThreadPoolItem_Ret &&) noexcept=default;
		~CThreadPoolItem_Ret();
	};
}

#include"CThreadPoolItem_Ret.cpp"

#endif