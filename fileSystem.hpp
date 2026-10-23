#pragma once
#include <map>
#include <string>
#include <vector>

class entity {
public:
	virtual ~entity() {}
	std::vector<entity*> parents;
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


