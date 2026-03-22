#include<string>
#include<iostream>
#include<fstream>
#include<unordered_map>
#include<sstream>
#include<vector>
#include <unistd.h>
#include <filesystem>
#include<dirent.h>
//system(cpCmd.c_str());
using namespace std;

//flags
bool mainF = false;



//maps
unordered_map<std::string,std::string> variables;

//functions
string findNub(){
    for(auto& entry : std::filesystem::directory_iterator(".")) {
        if(entry.path().extension() == ".nub")
            return entry.path().string().substr(2);
    }
    return "";
}
//parse
void parse(){
    string filename = findNub();
    ifstream in(filename);
    string line;
    while(getline(in,line)){
        if(!line.empty()){
            if(line=="main{"){
                mainF=true;
            }
            if(mainF && line.find("sh:") != string::npos){
                int i = line.find("sh:");
                string cmd = line.substr(i+3);
                system(cmd.c_str());
            }
            if(line=="}"){
                mainF=false;
            }
        }
    }
}



int main(){
    setuid(0);
    setgid(0);
    if(findNub()!=""){
        parse();
    }else{
        cout<<"no file found \n";
    }
}