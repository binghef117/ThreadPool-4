#include"CCThreadPoolItem.h"

namespace nTool
{
	template<class Func_t>
	void CThreadPoolItem<Func_t>::loop_()
	{
		while(waiting_(),!destructor_)
			exec_->exec();
	}

	template<class Func_t>
	CThreadPoolItem<Func_t>::CThreadPoolItem()
		:destructor_{false},joinable_{false},wait_{0},thr_{&CThreadPoolItem<Func_t>::loop_,this}{}

	template<class Func_t>
	template<class ... Args>
	void CThreadPoolItem<Func_t>::assign(Args &&...args)
	{
		exec_=std::make_unique<CThreadPoolItemExecutorJoin<Func_t>>(commun_.get(),std::forward<Args>(args)...);
		joinable_=true;
		wake_();
	}

	template<class Func_t>
	template<class ... Args>
	void CThreadPoolItem<Func_t>::assign_and_detach(Args &&...args)
	{
		exec_=std::make_unique<CThreadPoolItemExecutorDetach<Func_t>>(commun_.get(),std::forward<Args>(args)...);
		joinable_=false;
		wake_();
	}

	template<class Func_t>
	template<class ... Args>
	void CThreadPoolItem<Func_t>::assign_and_ret(Args &&...args)
	{
		exec_=std::make_unique<CThreadPoolItemExecutorRet<Func_t>>(commun_.get(),std::forward<Args>(args)...);
		joinable_=false;
		wake_();
	}

	template<class Func_t>
	CThreadPoolItem<Func_t>::~CThreadPoolItem()
	{
		if(exec_->is_running())
			exec_->wait();
		destructor_=true;
		wake_();
	}

	template<class Func_t>
	IThreadPoolItemExecutorBase<Func_t>::~IThreadPoolItemExecutorBase(){}

	template<class Func_t>
	template<class ... Args>
	CThreadPoolItemExecutorDetach<Func_t>::CThreadPoolItemExecutorDetach(ThreadPoolCommunBase<Func_t> *commun,Args &&...args)
		:commun_{commun},complete_{0},func_{std::bind(std::forward<Args>(args)...)}{}

	template<class Func_t>
	void CThreadPoolItemExecutorDetach<Func_t>::exec()
	{
		func_();
		commun_->communPoolDetach();
		complete_.signal();
	}

	template<class Func_t>
	template<class ... Args>
	CThreadPoolItemExecutorJoin<Func_t>::CThreadPoolItemExecutorJoin(ThreadPoolCommunBase<Func_t> *commun,Args &&...args)
		:commun_{commun},complete_{0},func_{std::bind(std::forward<Args>(args)...)},running_{true}{}

	template<class Func_t>
	void CThreadPoolItemExecutorJoin<Func_t>::exec()
	{
		func_();
		commun_->communPoolFinish();
		complete_.signal();
	}

	template<class Func_t>
	void CThreadPoolItemExecutorJoin<Func_t>::wait()
	{
		complete_.wait();
		running_=false;
		commun_->communPoolJoin();
	}

	template<class Func_t>
	template<class ... Args>
	CThreadPoolItemExecutorRet<Func_t>::CThreadPoolItemExecutorRet(ThreadPoolCommunBase<Func_t> *commun,Args &&...args)
		:commun_{commun},task_{std::forward<Args>(args)...}{}

	template<class Func_t>
	decltype(std::declval<CTask<Func_t>>().get()) CThreadPoolItemExecutorRet<Func_t>::get()
	{
		const auto temp{task_.get()};
		commun_->communPoolDetach();
		return temp;
	}
}