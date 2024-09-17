#include "config.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

Config::Config(const std::string &configPath)
{
    this->configPath = configPath;
}

void Config::init()
{
    std::ifstream file(configPath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << configPath << std::endl;
        return;
    }

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            configMap[key] = value;
        }
    }

    file.close();
}

std::string Config::get(const std::string &key)
{
    if (configMap.find(key) == configMap.end())
    {
        throw std::invalid_argument("Key not found in configuration settings. Please check, if init method was called.");
        return std::string();
    }
    return configMap[key];
}
