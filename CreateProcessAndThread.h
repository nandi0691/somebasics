#ifndef _TASK_H_
#define _TASK_H_
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <array>

//#define INIT(a) BC<a> obj;
#define INIT(a) BC<a> *ptr = reinterpret_cast<a>&(Singleton<BC<a>>::getInstance());ptr->init()
//#define INIT(a) std::cout << "output: " #a << '\n';
enum process {
	eProcessManager=0,
	eBCInput,
	eBCManager,
	eBCWriter,
	eBCTest,

	MAX_NUM_PROCESS
};

static std::array<std::string,MAX_NUM_PROCESS> procArray {"ProcessManager","BCInput","BCManager","BCWriter","BCTest"};
class Task
{
	private:
		std::string m_ProcName;
		bool m_isItRoot;
		pid_t m_processID;
		void SetPid(pid_t pid){m_processID = pid;}
		void SetProcessName(std::string name){m_ProcName = name;}
		void SetRoot(bool val){m_isItRoot = val;}
	public:
		pid_t CreateProcess(const bool isGiveRoot = false);
		int CreateThread(std::string name);
		std::string GetProcessName()const {return m_ProcName;}
		bool GetRoot()const {return m_isItRoot;}
		pid_t GetPid() const {return m_processID;}
		Task(){};
		~Task(){};
		//Nandeesh - we do not want copy , move or assignment operations fot this class
		//This can be changed if there is a need
		Task(const Task & obj) = delete;
		Task(const Task && obj) = delete;
		Task & operator=(const Task & obj) = delete;
};

template <typename T>
class Singleton
{
	public:
		static bool m_InitDone;
		static T& getInstance()
		{
			static T instance;
			return instance;
		}
	protected:
		Singleton() {}
		~Singleton() {}
	public:
		Singleton(Singleton const &) = delete;
		Singleton& operator=(Singleton const &) = delete;
};

template <typename T>
class BC
{
	friend class Singleton<T>;
	public:
		void init();
};

class BCWriter
{
	friend class BC<BCWriter>;
	private:
		BCWriter(){};
		~BCWriter(){};
};

class BCTest
{
	friend class BC<BCTest>;
	private:
		BCTest(){};
		~BCTest(){};
};

class BCManager
{
	friend class Singleton<BCManager>;
	private:
		BCManager(){};
		~BCManager(){};
	public:
		void init();
};

class BCInput
{
	friend class Singleton<BCInput>;
	private:
		BCInput(){};
		~BCInput(){};
	public:
		void init();
};

template <typename V>
void init()
{
	BC<V> * ptr = &(Singleton<BC<V>>::getInstance());
	ptr->init();
};
#endif

