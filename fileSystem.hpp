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

// Operations

int makeDir(entity* root, std::string path);

int moveDir(entity* root, std::string src_path, std::string dst_path);

int rmDir(entity* root, std::string path);

int linkEntity(entity* root, std::string path, std::string dst_path);

int renameFile(entity* root, std::string path, std::string dst_path);

int writeFile(entity* root, std::string path, std::string data);

int removeFile(entity* root, std::string path);

std::string ls(entity* root, std::string path);

std::string readFile(entity* root, std::string path);

// Utils

std::vector<std::string> splitPath(std::string path);

entity* traversePath(entity* root, std::vector<std::string> splitPath, int n);
