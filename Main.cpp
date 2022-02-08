#include "Main.h"

class node{
	std::string m_NodeHash;
	void GenerateNodeHash();
	void DestroyNodeHash();
	int m_data;
	//We do not want copy semantics and move semantics
	node(const node &obj) = delete;
	node(const node &&obj) = delete;
public:
	node(){
		node::count++;
		GenerateNodeHash();
		m_data=-1;
	}
	virtual ~node(){
		DestroyNodeHash();
	}
	std::string GetNodeHash();
	static int count;
	int GetData();
	node * next;
	node * prev;
	void AddNode(const int&);
	void AddData(const int&);
	void CreateNodeHead(const int&);

};

void node::AddNode(const int& data)
{
	
}

void node::AddData(const int &val)
{
	m_data = val;
}

void node::DestroyNodeHash()
{
	m_NodeHash.empty();
}

void node::GenerateNodeHash()
{
	using namespace std;
	m_NodeHash = "Nandeesh"+std::to_string(node::count);
	cout<<GetNodeHash()<<endl;
}
std::string node::GetNodeHash()
{
	return m_NodeHash;
}
int node::count = 0;

void ExecuteCommand(const std::string cmd,std::string & ans)
{
	FILE* fpipe = popen(cmd.c_str(),"r");
	char line[256];
	ans.clear();
	while (fgets(line, sizeof(line), fpipe))
		ans+=line;
	pclose(fpipe);
	ans.erase( std::remove(ans.begin(), ans.end(), '\r'),ans.end() );
	ans.erase( std::remove(ans.begin(), ans.end(), '\n'),ans.end() );
}

std::string SHA256Enc(std::string tokenToEncrypt)
{
	std::string encryptedString ;
	using namespace std;
	//removing carriage return
	tokenToEncrypt.erase( std::remove(tokenToEncrypt.begin(), tokenToEncrypt.end(), '\r'), tokenToEncrypt.end() );
	tokenToEncrypt.erase( std::remove(tokenToEncrypt.begin(), tokenToEncrypt.end(), '\n'), tokenToEncrypt.end() );
	
	std::string commandToEncrypt = "echo -n "+tokenToEncrypt+" | sha256sum | sed \'s/ .*//\' | sed 's/\r$//'";
	const char * cmd = commandToEncrypt.c_str();
	ExecuteCommand(cmd,encryptedString);
	//cout<<"encrypted string is "<<encryptedString<<endl;//This print has to be removed after debugging
	tokenToEncrypt = encryptedString;
	tokenToEncrypt.erase( std::remove(tokenToEncrypt.begin(), tokenToEncrypt.end(), '\r'), tokenToEncrypt.end() );
        tokenToEncrypt.erase( std::remove(tokenToEncrypt.begin(), tokenToEncrypt.end(), '\n'), tokenToEncrypt.end() );
	return tokenToEncrypt;
}

void CreateDirectory(const char* path, mode_t mode)
{
	using namespace std;
	int res = mkdir(path, 0777);
	if (-1 == res)
	{
		if(errno == EEXIST)
		cout<<"Directory already exists at "<<path<<endl;
		else
		cout<<"directory creation failed with "<<errno<<endl;
		return;
	}
	mode_t old = umask(S_IWGRP | S_IWOTH);
	res = chmod(path, mode);
	umask(old);
	if (-1 == res)
	{
		cout<<"directory creation failed with "<<errno<<endl;
	}
	return;
}

bool IsFileExists(const char* fname)
{
	struct stat64 buff64;
	int res = stat64(fname, &buff64);
	if (-1 != res)
		return true;

	return false;
}

void CreateFile(const char* file)
{
	using namespace std;
	if(IsFileExists(file))
	{
		cout<<"File already exists"<<endl;
		return;
	}
	int res = open(file,O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (-1 == res)
	{
		cout<<"File creation failed with "<<errno<<endl;
		return;
	}
}

void WriteToFile(const char * fname, const char * data)
{
	using namespace std;
	//CreateFile(fname);
	std::ofstream file;
	file.open(fname,ios::out|ios::trunc);
	if(file.is_open())
	{
		file << data;
		file.close();
	}
}
std::string GenerateSerialNumber()
{
	//sleep(1);
	static const char alpha[]=
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	const int stringLength=sizeof(alpha)-1;	
	//srand(time(0));
	time_t t;
	srand((unsigned) time(&t));
	std::string randStr;
	for(unsigned int i=0;i<34;i++)
	{
		randStr+=alpha[rand() %stringLength];
	}
	return randStr;
}

void InitFiles()
{
	CreateDirectory("/opt/nandeesh",0777);
	std::string date;
	ExecuteCommand("date",date);
	std::string data = "Serial No : "+GenerateSerialNumber()+"\nDate : "+date;
	WriteToFile("/opt/nandeesh/SerialNumber",data.c_str());
}

void CreateBlock(const char * data)
{
	using namespace std;
	std::string prevNum = to_string(nonce++);
	std::string num = to_string(nonce);
	std::string fname = "blocks/"+num;
	
	std::string ans;
	ExecuteCommand("date",ans);
	
	std::string data_ = "NONCE : "+num+"\nTransaction Time : "+ans+"\nData : "+data;
	std::string hash = SHA256Enc(data_);
	if(nonce == 1)
		ans = "No previous hash for genesis block";
	else
		ExecuteCommand("head -n 1 blocks/"+prevNum,ans);
	hash.append("\n"+ans);
	cout<<hash<<endl;
	ExecuteCommand("date +%s%m",ans);
	hash.append("\n"+ans+"\n"+data_);
	WriteToFile(fname.c_str(),hash.c_str());
	//cout<<hash<<endl;
}

void MonitorAllBlocks()
{
	using namespace std;
	if(currBlock > nonce)
	{
		currBlock = 1;
		return;
	}
	string cmd = "sed -n \'3p\' < blocks/"+to_string(currBlock);
	std::string tsFromFile = "";
	ExecuteCommand(cmd,tsFromFile);
	cmd = "date -r blocks/"+to_string(currBlock)+" +%s%m";
	std::string tsFromDateCmd = "";
	ExecuteCommand(cmd,tsFromDateCmd);

	if(tsFromDateCmd != tsFromFile)
	{
		cout<<"File "<<currBlock<<" is not okay"<<endl;
		recalculateHash(to_string(currBlock));
	}
	if(currBlock > 2)
	{
		std::string currHash,prevHash;
		cmd = "sed -n \'1p\' < blocks/"+to_string(currBlock-1);
		ExecuteCommand(cmd,prevHash);
		cmd = "sed -n \'2p\' < blocks/"+to_string(currBlock);
		ExecuteCommand(cmd,currHash);
		if(strstr(prevHash.c_str(),currHash.c_str()) == NULL)
			cout<<"hash mismatch for "<<currBlock-1<<endl;
	}
	currBlock++;
}

void recalculateHash(std::string block)
{
	using namespace std;
	std::string ans;
	std::string cmd = "sed -n \'$=\' blocks/"+block;
	ExecuteCommand(cmd,ans);
	cmd = "sed -n \'4,"+ans+"p\' blocks/"+block;
	ExecuteCommand(cmd,ans);
	std::string hash = (SHA256Enc(ans));
	cmd = "sed -i \"1s/.*/"+hash+"/\" blocks/"+block;
	ExecuteCommand(cmd,ans);
	cmd = "date +%s%m";
	ExecuteCommand(cmd,ans);
	cmd = "sed -i \"3s/.*/"+ans+"/\" blocks/"+block;
	ExecuteCommand(cmd,ans);
	cmd = "cat blocks/"+block;
	ExecuteCommand(cmd,ans);
	//cout<<ans<<endl;
	
}

int main()
{	
	using namespace std;
	CreateDirectory("blocks",0777);
	for(int i =0;i<100;i++)
	{
		std::string data = "data"+to_string(i+1);
		CreateBlock(data.c_str());
		//recalculateHash(to_string(i+1));
	}
	while(1)
	{
		MonitorAllBlocks();
	}
	return 0;
}
