#include "fileSystem.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef BOOST
#include "boost/json.hpp"

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
