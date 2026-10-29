#include "fileSystem.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef BOOST
#include "boost/json.hpp"
#endif

/*
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
			if (s == "..") {
				if (result.size() > 0 && result.back() != "..") result.pop_back();
				else {
					result.push_back(s);
				}
			}
			else if (!s.empty() && s != ".") result.push_back(s);
		}
		pos = next_pos;
	}
	return result;
}

std::vector<std::string> get_name(entity* node) {
    if (!node || node->parents.empty()) return {""};
    std::vector<std::string> names;
    for (auto p : node->parents) {
        dir_entity* parent = dynamic_cast<dir_entity*>(p);
	if (parent == nullptr) continue;

    	for (auto c : parent->children) {
       	    if (c.second == node) names.push_back(c.first);
	}
    }
    return names;
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
			if (parent_path.empty() && tmp->parents.empty()) {
				// Root reached.
				continue;
			}
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
				auto parent_names = get_name(p);
				if (std::find(parent_names.begin(), parent_names.end(), parentName) != parent_names.end()) {
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
*/

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
	dir_entity* root = nullptr;
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

	if (root == nullptr)
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
			linkEntity(root, getPath(buf), getSecondPath(buf));
		if (op == "chdir") {
			auto split = splitPath(getPath(buf));
			auto tmp = dynamic_cast<dir_entity*>(traversePath(root, split, split.size()));
			if (tmp != nullptr) root = tmp;
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
