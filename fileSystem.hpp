#pragma once
#include <map>
#include <string>

class entity {
public:
	virtual ~entity() {}
};

class dir_entity: public entity {
public:
	~dir_entity();
	std::map<std::string, entity*> children;
};

class file_entity: public entity {
public:
	std::string data;
};


