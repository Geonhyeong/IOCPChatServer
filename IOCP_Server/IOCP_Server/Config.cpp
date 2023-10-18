#include "pch.h"
#include "Config.h"

Config::Config()
{
	_config.LoadFile("config.xml");
}

Config::~Config()
{
}

Config& Config::GetInstance()
{
	static Config* instance = new Config();
	return *instance;
}

XMLDocument* Config::GetConfig()
{
	return &_config;
}