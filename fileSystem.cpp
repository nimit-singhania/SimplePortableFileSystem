// We implement a simple file system as part of this exercise that allows the following operations on the filesystem:
// * makeDir(path): Given a path, creates a new directory along the path.
//     Returns -1 if the parent directory is not present.
// * ls(path): Returns a space-seperated list of objects in the directory given by the path.
// * write(path, string): writes a new file with path and the string contents provided.
// * read(path): Returns the contents of a file if it exists.

// The root directory is '/'.

// Algo: we leverage a tree structure to represent the file system.
// We store additional boolean field to identify directories from files.
// We further store files as file entities with string data.

#include "fileSystem.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef BOOST
#include "boost/json.hpp"
#endif

dir_entity::~dir_entity() {
	for (auto p: this->children) {
		delete p.second;
	}
}

std::vector<std::string> splitPath(std::string path) {
	std::vector<std::string> result;

	if (path[0] != '/') {
		path = '/' + path;
	}

	int pos = 0;
	while(pos < path.size()) {
		int next_pos = path.find_first_of("/", pos+1);
		if (next_pos == std::string::npos) {
			// next_pos is end of string.
			std::string s = path.substr(pos+1);
			if (!s.empty()) result.push_back(s);
			break;
		}
		else {
			std::string s = path.substr(pos+1, next_pos - pos - 1);
			if (!s.empty()) result.push_back(s);
		}
		pos = next_pos;
	}
	return result;
}

std::string get_name(entity* node) {
    if (!node || node->parents.empty()) return "";
    dir_entity* parent = dynamic_cast<dir_entity*>(node->parents[0]);
    if (!parent) return "";
    for (auto p : parent->children) {
        if (p.second == node) return p.first;
	}
    return "";
}

bool isalphanumeric(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}	

bool is_valid(std::string name) {
	// Check that the name is valid and contains alphanumeric characters or a period (not at the first position).
	if (!isalphanumeric(name[0])) return false;

	for (int i = 1; i < name.size(); i++) {
		if (!isalphanumeric(name[i]) && name[i] != '.') 
			return false;
	}
	return true;
}

entity* traversePath(entity* root, std::vector<std::string> splitPath, int n) {
	auto* tmp = root;
	std::vector<entity*> parent_path;
	for (int i = 0; i < n; i++) {
		auto* tmp_dir = dynamic_cast<dir_entity*>(tmp);
		if (tmp_dir == nullptr) return nullptr;
		if (splitPath[i] == ".") {
			// No change in the current path
			continue;
		}
		if (splitPath[i] == "..") {
			// Go back to the parent directory.
			if (parent_path.empty() && tmp->parents.empty()) return nullptr;
			if (parent_path.empty()) {
				tmp = tmp->parents[0];
				continue;
			}
			tmp = parent_path.back();
			parent_path.pop_back();
			continue;
		}
		if (splitPath[i].find_first_of("..") == 0) {
			// Extract parent name and go to that parent.
			auto parentName = splitPath[i].substr(2);
			bool parentFound = false;
			for (auto p : tmp->parents) {
				if (get_name(p) == parentName) {
					tmp = p;
					parentFound = true;
					break;
				}
			}
			if (!parentFound) return nullptr;
			parent_path.pop_back();
			continue;
		}
		if (tmp_dir->children.find(splitPath[i]) == tmp_dir->children.end())
			return nullptr;
		// Update parent_path.
		parent_path.push_back(tmp_dir);
		tmp = tmp_dir->children[splitPath[i]];
	}
	return tmp;
}	


int makeDir(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0) return -1;

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() - 1));
	if (parentDir == nullptr) {
		std::cout << "Path invalid, please make sure that the parent directory is present!"
			  << std::endl;
		return -1;
	}

	auto dirname = split[split.size() - 1];
	if (!is_valid(dirname)) return -1;

	if (parentDir->children.find(dirname) != parentDir->children.end()) {
		std::cout << "Entity with name " << dirname << " already present!" << std::endl;
		return -1;
	}
	// Add directory as a child.
	auto* dir = new dir_entity;
	dir->parents.push_back(parentDir);
	parentDir->children.emplace(dirname, dir);
	return 0;
}

int moveDir(entity* root, std::string src_path, std::string dst_path) {
	std::vector<std::string> src_split = splitPath(src_path);
	if (src_split.size() == 0) return -1;

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, src_split, src_split.size() - 1));
	std::string dirname = src_split[src_split.size() - 1];
	if (parentDir == nullptr || parentDir->children.find(dirname) == parentDir->children.end()) {
		std::cout << "Path invalid, please make sure that the source path is valid."
			  << std::endl;
		return -1;
	}
	std::vector<std::string> dst_split = splitPath(dst_path);
	dir_entity* dstDir = dynamic_cast<dir_entity*>(traversePath(root, dst_split, dst_split.size()));
	if (dstDir == nullptr) {
		std::cout << "Destination path invalid, please make sure that the destination directory is present."
			  << std::endl;
		return -1;
	}
	auto* dir = parentDir->children[dirname];
	dir->parents.erase(std::find(dir->parents.begin(), dir->parents.end(), parentDir));
	parentDir->children.erase(dirname);
	dstDir->children[dirname] = dir;
	dir->parents.push_back(dstDir);
	return 0;
}

int rmDir(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0 || traversePath(root, split, split.size()) == root) {
		std::cout << "The current directory itself can not be removed. Please change directory before removing the directory." << std::endl;
		return -1;
	}

	dir_entity* parentDir = dynamic_cast <dir_entity*>(traversePath(root, split, split.size() -1));
	if (parentDir == nullptr) {
		std::cout << "Path invalid, please make sure that the parent directory is present."
			  << std::endl;
		return - 1;
	}

	auto dirname = split[split.size() - 1];
	if (parentDir->children.find(dirname) == parentDir->children.end()) {
		std::cout << "Entity with name " << dirname << " not found!" << std::endl;
		return -1;
	}

	dir_entity* dir = dynamic_cast<dir_entity*>(parentDir->children[dirname]);
	if (dir == nullptr) {
		std::cout << "Entity with name " << dirname << " not found!" << std::endl;
		return -1;
	}
	if (!dir->children.empty()) {
		std::cout << "Not empty directory found. Please empty the directory before removing it." << std::endl;
		return -1;
	}
	for (auto* p : dir->parents) {
		auto* pdir = dynamic_cast<dir_entity*>(p);
		std::vector<std::string> removed_entity_names;
		for (auto& c : pdir->children) {
			if (c.second == dir) {
				removed_entity_names.push_back(c.first);
			}
		}
		for (auto e : removed_entity_names) {
			pdir->children.erase(e);
		}
	}
	delete dir;
	return 0;
}

int link(entity* root, std::string path, std::string dst_path) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0) return -1;
	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() -1 ));
	auto entity_name = split[split.size()  - 1];
	if (parentDir == nullptr || parentDir->children.find(entity_name) == parentDir->children.end()) {
		std::cout << "Path invalid. Please make sure that the entity being linked is present."
			  << std::endl;
		return -1;
	}
	std::vector<std::string> dstsplit = splitPath(dst_path);
	if (dstsplit.size() == 0) return -1;
	dir_entity* dstparentDir = dynamic_cast<dir_entity*>(traversePath(root, dstsplit, dstsplit.size() - 1));
	auto dst_entity_name = dstsplit[dstsplit.size() -1];
	if (dstparentDir == nullptr || dstparentDir->children.find(dst_entity_name) != dstparentDir->children.end()) {
		std::cout << "Link path invalid. Please make sure that the link path is valid and entity with link path"
			  << " is not present." << std::endl;
		return -1;
	}
	auto* entity = parentDir->children[entity_name];
	entity->parents.push_back(dstparentDir);
	dstparentDir->children.emplace(dst_entity_name, entity);
	return 0;
}

int renameFile(entity* root, std::string path, std::string dst_path) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0) return -1;

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() -1 ));
	auto filename = split[split.size() -1];
	if (parentDir == nullptr || parentDir->children.find(filename) == parentDir->children.end()) {
		std::cout << "Path invalid. Please make sure that the file is present."
			  << std::endl;
		return -1;
	}

	std::vector<std::string> dstSplit = splitPath(dst_path);
	if (dstSplit.size() == 0) return -1;

	dir_entity* dstParentDir = dynamic_cast<dir_entity*>(traversePath(root, dstSplit, dstSplit.size() - 1));
	auto dstFileName = dstSplit[dstSplit.size() - 1];
	if (dstParentDir == nullptr) {
		std::cout << "Destination path invalid. Please make sure the destination directory is present."
			  << std::endl;
		return -1;
	} 
	if (dstParentDir->children.find(dstFileName) != dstParentDir->children.end()) {
		std::cout << "Destination path invalid. Please make file with new name is not already present!"
			  << std::endl;
		return -1;
	}
	auto* file = parentDir->children[filename];
	parentDir->children.erase(filename);
	file->parents.erase(std::find(file->parents.begin(), file->parents.end(), parentDir));
	dstParentDir->children.emplace(dstFileName, file);
	file->parents.push_back(dstParentDir);
	return 0;
}

int writeFile(entity* root, std::string path, std::string data) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0) return -1;

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() - 1));
	if (parentDir == nullptr) {
		std::cout << "Path invalid, please make sure that the parent directory is present!"
			  << std::endl;
		return -1;
	}

	auto filename = split[split.size() -1];
	if (!is_valid(filename)) return -1;
	if (parentDir->children.find(filename) != parentDir->children.end()) {
		std::cout << "Entity with name " << filename << " already present!" << std::endl;
		return -1;
	}

	// Add a new file as child.
	auto* file = new file_entity;
	file->data = data;
	file->parents.push_back(parentDir);
	parentDir->children.emplace(filename, file);
	return 0;
}

int removeFile(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);
	if (split.size() == 0) return -1;

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() - 1));
	if (parentDir == nullptr) {
		std::cout << "Path invalid. Please provide the correct path to the file." << std::endl;
		return -1;
	}

	auto filename = split[split.size() - 1];
	if (parentDir->children.find(filename) == parentDir->children.end()) {
		std::cout << "File not found. Please check the file is present in the path provided." << std::endl;
		return -1;
	}

	entity* file = parentDir->children[filename];

	for (auto* parent: file->parents) {
		auto* pdir = dynamic_cast<dir_entity*>(parent);
		std::vector<std::string> removed_entity_names;
		for (auto& p: pdir->children) {
			if (p.second == file) {
				removed_entity_names.push_back(p.first);
			}
		}
		for (auto e : removed_entity_names) {
			pdir->children.erase(e);
		}
	}
	delete file;
	return 0;
}

std::string ls(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size()));
	if (parentDir == nullptr) {
		std::cout << "Path not found! Please check that the path provided is valid." << std::endl;
		return "";
	}

	std::string names;
	for (auto p : parentDir->children) {
		names.append(p.first);
		names.append(" ");
	}

	return names;
}

std::string readFile(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);

	file_entity* file = dynamic_cast<file_entity*>(traversePath(root, split, split.size()));
	if (file == nullptr) {
		std::cout << "Path not found! Please check that the path provided is valid." << std::endl;
		return "";
	}
	return file->data;
}

#ifdef BOOST
boost::json::object exportEntityToJson(entity* root) {
	boost::json::object entity_json;

	// export directory structure.
	auto* dir_root = dynamic_cast<dir_entity*>(root);
	if (dir_root) {
		entity_json["type"] = "dir";
		boost::json::array children_json;
		for (auto p: dir_root->children) {
			auto child_obj = exportEntityToJson(p.second);
			child_obj["name"] = p.first;
			children_json.push_back(child_obj);
		}
		entity_json["children"] = children_json;
	}

	auto* file_root = dynamic_cast<file_entity*>(root);
	if (file_root) {
		entity_json["type"] = "file";
		entity_json["data"] = file_root->data;
	}

	return entity_json;
}

entity* readEntityFromJson(boost::json::object& obj_json) {
	if (obj_json["type"] == "dir") {
		dir_entity* dir_obj = new dir_entity();
		boost::json::array& children = obj_json["children"].as_array();
		for (auto& child: children) {
			auto& child_obj = child.as_object();
			entity* child_entity = readEntityFromJson(child_obj);
			std::string name = std::string(child_obj["name"].as_string().c_str());
			dir_obj->children.emplace(name, child_entity);
		}
		return dir_obj;
	}
	if (obj_json["type"] == "file") {
		file_entity* file_obj = new file_entity();
		file_obj->data = obj_json["data"].as_string().c_str();
		return file_obj;
	}
	return nullptr;
}
#endif

std::string getOperation(std::string s) {
	int pos = 0;
	while (s[pos] == ' ') pos++;
	int end_pos = s.find_first_of(" ", pos);
	if (end_pos == std::string::npos) return s.substr(pos);
	if (end_pos > pos) return s.substr(pos, end_pos - pos);
	return "";
}

std::string getPath(std::string s) {
	int pos = 0;
	// Look for second alpha-numeric string.
	while (s[pos] == ' ') pos++;
	pos = s.find_first_of(" ", pos);
	if (pos == std::string::npos) return "";
	while (s[pos] == ' ') pos++;
	int end_pos = s.find_first_of(" ", pos);
	if (end_pos == std::string::npos) return s.substr(pos);
	if (end_pos > pos) return s.substr(pos, end_pos - pos);
	return "";
}

std::string getSecondPath(std::string s) {
	int pos = 0;
	// Look for second alpha-numeric string.
	while (s[pos] == ' ') pos++;
	pos = s.find_first_of(" ", pos);
	if (pos == std::string::npos) return "";
	while (s[pos] == ' ') pos++;
	pos = s.find_first_of(" ", pos);
	if (pos == std::string::npos) return "";
	while (s[pos] == ' ') pos++;
	int end_pos = s.find_first_of(" ", pos);
	if (end_pos == std::string::npos) return s.substr(pos);
	if (end_pos > pos) return s.substr(pos, end_pos - pos);
	return "";
}

std::string getContents(std::string s) {
	int pos = 0;
	// Look for second alpha-numeric string.
	while (s[pos] == ' ') pos++;
	pos = s.find_first_of(" ", pos);
	if (pos == std::string::npos) return "";
	while (s[pos] == ' ') pos++;
	pos = s.find_first_of(" ", pos);
	if (pos == std::string::npos) return "";
	while (s[pos] == ' ') pos++;
	// Trim preceding and ending quotes.
	return s.substr(pos);
}


int main(int argc, char** argv) {
	dir_entity* root;
#ifdef BOOST
	if (argc > 1) {
		auto filename = argv[1];

		// read file and obtain json structure for the filesystem.
		std::ifstream is;
		is.open(filename);
		boost::json::value jv = boost::json::parse(is);
	
		boost::json::object& input_json = jv.as_object();
		// Read directory from the json object.
		root = dynamic_cast<dir_entity*>(readEntityFromJson(input_json));
	}
#endif

	if (!root)
		root = new dir_entity();
/*
	std::cout << "Make dir output: " << makeDir(root, "/test1") << std::endl;
	std::cout << "Write file output: " <<  writeFile(root, "/test1/testFile.txt", "Hello world!") << std::endl;
	std::cout << "Ls output: " << ls(root, "/") << std::endl;
	std::cout << "Ls output: " << ls(root, "/test1") << std::endl;
	std::cout << "File contents: " << readFile(root, "/test1/testFile.txt") << std::endl;
*/
	while (true) {
		char buf[1024];
		std::cout << "$";
		std::cin.getline(buf, 1024);
		// Read operation.
		std::string op = getOperation(buf);
		// std::cout << "Perform operation: " << op << "(" << getPath(buf) << ")" << std::endl;
		if (op == "exit") break;
		if (op == "makedir")
			makeDir(root, getPath(buf));
		if (op == "write_file")
			writeFile(root, getPath(buf), getContents(buf));
		if (op == "ls")
			std::cout << ls(root, getPath(buf)) << std::endl;
		if (op == "read_file")
			std::cout << readFile(root, getPath(buf)) << std::endl;
		if (op == "rmdir")
			std::cout << "Dir Deleted: " << (rmDir(root, getPath(buf))? "Failure": "Success")
				  << std::endl;
		if (op == "delete_file")
			std::cout << "File Deleted: " << (removeFile(root, getPath(buf))? "Failure":"Success")
				  << std::endl;
		if (op == "mvdir")
			moveDir(root, getPath(buf), getSecondPath(buf));
		if (op == "rename_file")
			renameFile(root, getPath(buf), getSecondPath(buf));
		if (op == "link")
			link(root, getPath(buf), getSecondPath(buf));
		if (op == "chdir") {
			auto split = splitPath(getPath(buf));
			auto tmp = dynamic_cast<dir_entity*>(traversePath(root, split, split.size()));
			if (!tmp) root = tmp;
		}
	}

	delete(root);
#ifdef BOOST
	boost::json::object root_json = exportEntityToJson(root);
	std::cout << boost::json::serialize(root_json) << std::endl;

	// write json to a file (specified by commandline).
	if (argc > 1) {
		std::ofstream os;
		os.open(argv[1], std::ios_base::out);

		if (!os.is_open()) return 0;

		boost::json::serializer sr;
		boost::json::value jv;
		boost::json::value_from(root_json, jv);
        	sr.reset( &jv );
        	while( ! sr.done() )
        	{
            		char buf[ 4000 ];
            		os << sr.read( buf );
        	}
	}
#endif

	return 0;
}
