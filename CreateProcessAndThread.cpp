#include "CreateProcessAndThread.h"

pid_t Task::CreateProcess(const bool isGiveRoot)
{
	using namespace std;
	for(const auto &proc:procArray)
	{
		pid_t pid = fork();
		if(pid == 0)
		{
			prctl(PR_SET_NAME, (unsigned long) proc.c_str());
			SetProcessName(proc);
			SetRoot(isGiveRoot);
			SetPid(getpid());
			//Nandeesh - If you need root. spawn ProcessManager as root
			if(!isGiveRoot)
			{
				setuid(1000);
				setgid(1000);
			}
			if(getuid() == 0)
				cout<<"I am root - "<<proc<<endl;
			
			const std::string &name = proc;
			if(proc == "BCInput")
				init<BCInput>();
			else if(proc == "BCManager")
				init<BCManager>();
			else if(proc == "BCWriter")
				init<BCWriter>();
			else if(proc == "BCTest")
				init<BCTest>();
			else
			{
				exit(0);
				return getpid();
			}

			/*
			   if(name == "BCInput")
			   {
			//BCInput *ptr = &(Singleton<BCInput>::getInstance());
			BC<BCInput> *ptr = &(Singleton<BC<BCInput>>::getInstance());
			ptr->init();
			}
			if(name == "BCManager")
			{
			//BCManager *ptr = &(Singleton<BCManager>::getInstance());
			BC<BCManager> *ptr = &(Singleton<BC<BCManager>>::getInstance());
			ptr->init();
			}
			if(name == "BCTest")
			{
			BC<BCTest> *ptr = &(Singleton<BC<BCTest>>::getInstance());
			ptr->init();
			}
			if(name == "BCWriter")
			{
			BC<BCWriter> *ptr = &(Singleton<BC<BCWriter>>::getInstance());
			ptr->init();
			}*/
			sleep(0);
			//Nandeesh - thread must not return to main
			//Create Function pointer to continue
			cout<<proc <<" with pid "<<getpid()<<" exited"<<endl;
			exit(0);
		}
	}
	return 0;
}

template <typename T>
bool Singleton<T>::m_InitDone = false;

template <typename U>
void BC<U>::init()
{
	using namespace std;
	cout<<"BC init"<<endl;
	if(Singleton<U>::m_InitDone)
	{
		cout<<"you can call init only once"<<endl;
		return;
	}
	Singleton<U>::m_InitDone = true;
	sleep(2);
}

/*void BCInput::init()
{
	using namespace std;
	cout<<"BCInput init"<<endl;
	if(Singleton<BCInput>::m_InitDone)
	{
		cout<<"you can call init only once"<<endl;
		return;
	}
	Singleton<BCInput>::m_InitDone = true;
	sleep(0);
}

void BCManager::init()
{
	using namespace std;
	cout<<"BCManager init"<<endl;
	if(Singleton<BCManager>::m_InitDone)
	{
		cout<<"init can be called only once"<<endl;
		return;
	}
	Singleton<BCManager>::m_InitDone = true;
	sleep(0);
}*/

int main()
{
	using namespace std;
	pid_t wpid;
	int status = 0;
	if (prctl(PR_SET_NAME, (unsigned long) "ProcessManager") < 0)
	cout<<"Error in setting process name";
	Task procManager;
	procManager.CreateProcess();

	while ((wpid = wait(&status)) > 0);
	return 0;
}
