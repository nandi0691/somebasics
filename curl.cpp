//This is not a clean code
//This will help me monitor my monthly expenses through telegram BOT
//https://api.telegram.org/bot5292498458:AAE5DO5M_9YtnuM4oWD64pkvkyh2jxHT7rg/getUpdates?offset=167329543?limit=1
//https://api.telegram.org/bot5292498458:AAE5DO5M_9YtnuM4oWD64pkvkyh2jxHT7rg/sendMessage?chat_id=2058010923&&text=youcandoit
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <curl/curl.h>
#include <unistd.h>
#include <json/value.h>
#include <json/json.h>
#include <json/writer.h>
#include <ctime>
#include <fstream>
#include<algorithm>

std::string botID;
std::string botToken;
unsigned long long offset;
unsigned long long date;
std::string msg;
std::string fname;

bool IsFileExists(const char* fname)
{
	struct stat64 buff64;
	int res = stat64(fname, &buff64);
	if (-1 != res)
		return true;

	return false;
}



static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void SendToBot(const std::string &str)
{
	using namespace std;
	CURL *curl;
        CURLcode res;
	std::string readBuffer;
        readBuffer.clear();
	curl = curl_easy_init();
	std::string url = "https://api.telegram.org/bot5292498458:AAE5DO5M_9YtnuM4oWD64pkvkyh2jxHT7rg/sendMessage?chat_id=2058010923&&text="+str;
	if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
        }
	cout<<readBuffer<<endl;
}
bool GetMessage()
{
	using namespace std;
	CURL *curl;
	CURLcode res;
	Json::Value root;
	Json::Reader reader;
	std::string &readBuffer = msg;
	readBuffer.clear();

	curl = curl_easy_init();
	std::string url = "https://api.telegram.org/bot"+botID+":"+botToken+"/getUpdates?offset="+to_string(offset)+"?limit=1";
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	cout<<msg<<endl;

	bool parsingSuccessful = reader.parse( readBuffer, root );
	if (!parsingSuccessful)
	{
		std::cout << "Error parsing the string" << std::endl;
		msg = "invalid data";
		return false;
	}

	const Json::Value mynames = root;

        bool val = false;
        for (const Json::Value& text : root["result"])
        {
                val = true;
		readBuffer = text["message"]["text"].toStyledString();
		offset = text["update_id"].asInt();
		date = text["message"]["date"].asInt();
        }
	return val;
}

void HandleCommand()
{
	using namespace std;
	cout<<"HandleCommand "<<msg<<endl;
	if(msg.find("/print") != string::npos)
	{
		const time_t tt = (time_t)date;
		tm *local_time = localtime(&tt);
		fname = "/opt/data/"+to_string(1900 + local_time->tm_year) + to_string(1 + local_time->tm_mon);
		if(!IsFileExists(fname.c_str()))
		{	
			SendToBot("Nothing_AS_of_NOW");
			return ;
		}
		std::ifstream file(fname);
		std::string str,line;
		str = "[";
		while (std::getline(file, line))
		{
			str += line;
		}
		str += "]";
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( str, root );
		if ( !parsingSuccessful )
		{
			cout << "Error parsing the string at fread" << endl;
			return;
		}
		std::stringstream ss;
		str.clear();
		int sum = 0;
		for (const Json::Value& text : root)
		{
			str += "\%0A--------------------------------------------------------\%0A";
			str += "date:%20";
			std::string _date = text["date"].asString();
			std::replace(_date.begin(), _date.end(), ' ', '_');
			_date.erase( std::remove(_date.begin(), _date.end(), '\r'),_date.end() );
			_date.erase( std::remove(_date.begin(), _date.end(), '\n'),_date.end() );
			cout<<_date<<endl;
			str += _date;
			str += "\%0A";
			str += "amount:%20";
			std::string amt = text["amount"].asString();
			int amt_ = stoi(amt);
			sum += amt_;
			str += amt;
			str += "\%0A";
			str += "mode:%20";
			str += text["mode"].asString();
			str += "\%0A";
			str += "purpose:%20";
			str += text["purpose"].asString();
			str += "\%0A";
			str += "vendor:%20";
			str += text["vendor"].asString();
			str += "\%0A";
			str += "\%0A";
		}
		str += "\%0A======================================================\%0A";
		str += "total%20Amount%20till%20date%20for%20this%20calendar%20month%20:%20";
		str += to_string(sum);
		str += "\%0A";
		SendToBot(str);
	}
}

void HandleData(std::vector<std::string> &token)
{
	std::string &s = msg;
	msg.erase(0,1);
	std::string delimiter = " ",tok;

	size_t pos = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) 
	{
		token.push_back(s.substr(0, pos));
		s.erase(0, pos + delimiter.length());
	}
	s.erase(s.length()-2,s.length()-1);	
	token.push_back(s);
}

bool is_number(std::string line)
{
    char* p;
    strtol(line.c_str(), &p, 10);
    return *p == 0;
}

int main()
{
	using namespace std;
	botID = "5292498458";
	botToken = "AAE5DO5M_9YtnuM4oWD64pkvkyh2jxHT7rg";
	offset = 1;

	while(1)
	{
		const time_t tt = (time_t)date;
		tm *local_time = localtime(&tt);
		fname = "/opt/data/"+to_string(1900 + local_time->tm_year) + to_string(1 + local_time->tm_mon);
		if(GetMessage() == false)
			continue;

		offset += 1;
		if(msg.find("/") != string::npos)
		{
			HandleCommand();
		}
		else
		{
			std::vector<std::string> token;
			HandleData(token);
			if(token.size()<3)
			{
				cout<<"less size for "<<msg<<endl;
				SendToBot("Format_is_purpose_amt_mode_vendor");
				continue;
			}
			if(!is_number(token[1]))
			{
				continue;
			}
			Json::Value event;
			short val = 0;
			std::string array[4]={"purpose","amount","mode","vendor"};
			event["vendor"] = "vendor";
			for(const auto &i:token)
			{
				event[array[val]] = token[val];
				if(++val>3)
					break;
			}

			bool isFileExists = IsFileExists(fname.c_str());
			event["date"] = ctime(&tt);
			/*std::string strJson;
			Json::Value root;
			root.append(event);
			Json::Value mainNode;
			mainNode["transaction"] = root;*/
			std::ofstream file_id;
			file_id.open(fname,std::ios_base::app);

			if(isFileExists)
				file_id <<",";
			
			Json::StreamWriterBuilder builder;
			std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
			writer -> write(event,&file_id);

			file_id.close();
			SendToBot("OKAY_noted_it");
		}
		sleep(0);
	}
	return 0;
}
