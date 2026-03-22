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
//subs
string substitute(string cmd) {
    string result = "";
    int i = 0;
    while(i < cmd.size()) {
        if(cmd[i] == '(') {
            int end = cmd.find(')', i);
            string varname = cmd.substr(i+1, end-i-1);
            if(variables.find(varname) != variables.end())
                result += variables[varname];
            i = end + 1;
        } else {
            result += cmd[i++];
        }
    }
    return result;
}
//parse
void parse(){
    string filename = findNub();
    ifstream in(filename);
    string line;
    while(getline(in,line)){
        if(!line.empty()){
            //variables
            if(!mainF && line.find("=") != string::npos) {
                istringstream ss(line);
                string name, eq, value;
                ss >> name >> eq;
                getline(ss, value);
                if(value.empty()) continue;
                value = value.substr(1);
                if(value.front() == '"') value = value.substr(1);
                if(value.back() == '"') value.pop_back();
                variables[name] = value;
            }

            //main function
            if(line=="main{"){
                mainF=true;
            }
            //shell commands
            if(mainF && line.find("sh:") != string::npos){
                int i = line.find("sh:");
                string cmd = substitute(line.substr(i+3));
                system(cmd.c_str());
            }
            //main end
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
        cout<<"no .nub file found \n";
    }
}