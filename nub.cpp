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
string modF = "";



//maps
unordered_map<std::string,std::string> variables;
unordered_map<string, vector<string>> functions;

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
void parse(string f){
    string filename = findNub();
    ifstream in(filename);
    string line;
    while(getline(in,line)){
        // handle line continuation
        while(!line.empty() && line.back() == '\\'){
            line.pop_back();
            string next;
            getline(in, next);
            int start = next.find_first_not_of(" \t");
            if(start != string::npos)
                next = next.substr(start);
            line += " " + next;
        }
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
                variables[name] = substitute(value);
            }
            //main function
            if(line=="main{"){
                mainF=true;
            }
            //shell commands in main
            if(mainF && line.find("sh:") != string::npos && f==""){
                int i = line.find("sh:");
                string cmd = substitute(line.substr(i+3));
                system(cmd.c_str());
            }
            //calling functions from main
            if(line.find("fn: ") != string::npos && mainF && f.empty()){
                int i = line.find("fn: ");
                string fname = substitute(line.substr(i+4));
                for(auto a:functions[fname]){
                    system(a.c_str());
                }
            }
            //main end
            if(line=="}"){
                mainF=false;
                modF="";
            }
            //modular function start
            if(line.starts_with("/")&&!mainF){
                int end = line.find('{');
                string funcname = line.substr(1, end-1);
                modF=funcname;
            }
            //shell commands in modular function (not called)
            if(line.find("sh:") != string::npos && !mainF && !modF.empty()){
                int i = line.find("sh:");
                string cmd = substitute(line.substr(i+3));
                functions[modF].push_back(cmd);
            }
            //shell commands in modular function (called from cmd)
            if(!modF.empty() && line.find("sh:") != string::npos && modF==f){
                int i = line.find("sh:");
                string cmd = substitute(line.substr(i+3));
                system(cmd.c_str());
            }
        }
    }
}


int main(int argc, char* argv[]){
    setuid(0);
    setgid(0);
    string f="";
    if(argc>1) f = argv[1];
    if(findNub()!=""){
        parse(f);
        cout<<"done\n";
    }else{
        cout<<"no .nub file found \n";
    }
}