#pragma once
#include<string>
#include<fstream>
#include<cstdio>
#include<map>
struct resource_s{
    std::string key;
    std::string value;

    friend std::ifstream& operator>>(std::ifstream&in, resource_s&that){
        std::getline(in,that.key);
        std::getline(in,that.value);
        return in;
    }
};

std::map<std::string, std::string> resources;
void initresources(std::string path){
    std::ifstream file(path);
    resource_s resource;
    while(file>>resource)resources[resource.key]=resource.value;
}