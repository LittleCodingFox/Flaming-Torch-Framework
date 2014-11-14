#pragma once
#include <vector>

namespace SimpleDelegate
{
	template<typename... Args>
	class SimpleDelegate
	{
	private:
		typedef void(*SimpleCallback)(Args...);
		typedef void(*SimpleMemberCallback)(void *, Args...);

		struct CallInfo
		{
			void *Owner;
			SimpleCallback Callback;
		};

		std::vector<CallInfo> CallData;

		SimpleDelegate(const SimpleDelegate &o);
		SimpleDelegate &operator=(const SimpleDelegate &o);
	public:

		template<class T, void (T::*Callback)(Args...)>
		static void MethodCaller(void *Self, Args... params)
		{
			(((T *)Self)->*Callback)(params...);
		};

		SimpleDelegate() {};

		void operator()(Args... params)
		{
			for(std::vector<CallInfo>::iterator it = CallData.begin(); it != CallData.end(); it++)
			{
				if (it->Owner)
				{
					((SimpleMemberCallback)it->Callback)(it->Owner, params...);
				}
				else
				{
					it->Callback(params...);
				}

				if(it == CallData.end())
					break;
			};
		};

		void Connect(SimpleCallback Callback)
		{
			CallInfo Info;
			Info.Owner = NULL;
			Info.Callback = Callback;

			CallData.push_back(Info);
		};

		template<class T, void (T::*Callback)(Args...)>
		void Connect(T *Owner)
		{
			CallInfo Info;
			Info.Owner = Owner;
			Info.Callback = (SimpleCallback)&MethodCaller<T, Callback>;

			CallData.push_back(Info);
		};

		void Disconnect(SimpleCallback Callback)
		{
			for (std::vector<CallInfo>::iterator it = CallData.begin(); it != CallData.end(); it++)
			{
				if (it->Owner == NULL && it->Callback == Callback)
				{
					CallData.erase(it);

					return;
				};
			};
		};

		template<class T, void (T::*Callback)(Args...)>
		void Disconnect(T *Owner)
		{
			for (std::vector<CallInfo>::iterator it = CallData.begin(); it != CallData.end(); it++)
			{
				if (it->Owner == Owner && it->Callback == (SimpleCallback)&MethodCaller<T, Callback>)
				{
					CallData.erase(it);

					return;
				};
			};
		};

		template<class T>
		void DisconnectObject(T *Owner)
		{
			for (std::vector<CallInfo>::reverse_iterator it = CallData.rbegin(); it != CallData.rend(); it++)
			{
				std::vector<CallInfo>::iterator rit = --it.base();

				if (rit->Owner == Owner)
				{
					CallData.erase(rit);

					it = CallData.rbegin();

					if (it == CallData.rend())
						break;
				};
			};
		};
	};
};
