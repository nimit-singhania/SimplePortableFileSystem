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

#include <iostream>
#include <fstream>
#include <vector>

#include "boost/json.hpp"

std::vector<std::string> splitPath(std::string path) {
	std::vector<std::string> result;

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

entity* traversePath(entity* root, std::vector<std::string> splitPath, int n) {
	auto* tmp = root;
	for (int i = 0; i < n; i++) {
		auto* tmp_dir = dynamic_cast<dir_entity*>(tmp);
		if (tmp_dir == nullptr) return nullptr;
		if (tmp_dir->children.find(splitPath[i]) == tmp_dir->children.end())
			return nullptr;
		tmp = tmp_dir->children[splitPath[i]];
	}
	return tmp;
}	


int makeDir(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() - 1));
	if (parentDir == nullptr)
		return -1;

	auto dirname = split[split.size() - 1];
	if (parentDir->children.find(dirname) != parentDir->children.end()) {
		return -1;
	}
	// Add directory as a child.
	auto* dir = new dir_entity;
	parentDir->children.emplace(dirname, dir);
	return 0;
}

int writeFile(entity* root, std::string path, std::string data) {
	std::vector<std::string> split = splitPath(path);

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size() - 1));
	if (parentDir == nullptr)
		return -1;

	auto filename = split[split.size() -1];
	if (parentDir->children.find(filename) != parentDir->children.end()) {
		return -1;
	}

	// Add a new file as child.
	auto* file = new file_entity;
	file->data = data;

	parentDir->children.emplace(filename, file);
	return 0;
}

std::string ls(entity* root, std::string path) {
	std::vector<std::string> split = splitPath(path);

	dir_entity* parentDir = dynamic_cast<dir_entity*>(traversePath(root, split, split.size()));
	if (parentDir == nullptr)
		return "";

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
	if (file == nullptr) return "";
	return file->data;
}

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
	}

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

	return 0;
}
