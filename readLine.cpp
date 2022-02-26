#include<iostream>
#include<fstream>

using namespace std;

void ReplaceLine(const std::string& filename,const int lineToReplace ,const std::string & dataToBeReplaced)
{
	std::fstream file(filename.c_str(),std::ios::in | std::ios::out);
	char line[1000];
	for (int i=0; i<lineToReplace-1; ++i) 
		file.getline(line, 1000);

	file.tellg();
	file << dataToBeReplaced;
	file.close();
}

std::string ReadLine(const std::string& filename, int N,bool readFull=false)
{
	std::ifstream in(filename.c_str());
	bool appendNewLineChar = false;
	std::string s,line;

	for(int i = 1; i < N; ++i)
		std::getline(in, s);

	if(readFull)
	{
		s.clear();
		while(std::getline(in, line))
		{
			if(appendNewLineChar)
			{
				s +="\n";
				appendNewLineChar = false;
			}
			s += line;
			appendNewLineChar = true;
		}
	}
	else
		std::getline(in,s);

	return s; 
}

int main()
{
	cout<<ReadLine("blocks/12",1)<<endl;
	cout<<ReadLine("blocks/12",2)<<endl;
	cout<<ReadLine("blocks/12",3)<<endl;
	cout<<ReadLine("blocks/12",4)<<endl;
	cout<<ReadLine("blocks/12",5)<<endl;
	cout<<ReadLine("blocks/12",6)<<endl;
	cout<<ReadLine("blocks/12",1,true)<<endl;
	ReplaceLine("blocks/12",2,"1ff3503dbffde67e485a6f2cefbea94d6af5d1907bb173c574962d67b8125d46");
	cout<<ReadLine("blocks/12",1,true)<<endl;
}
