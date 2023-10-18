#pragma once
#include "tinyxml2.h"

using namespace tinyxml2;

class Config
{
private:
	Config();
	~Config();

public:
	static Config& GetInstance();
	XMLDocument* GetConfig();

private:
	XMLDocument _config;
};

