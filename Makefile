all: fileSystem fileSystemSpec

fileSystem: fileSystem.cpp driver.cpp fileSystem.hpp
	g++ -std=c++11 fileSystem.cpp driver.cpp -o fileSystem

fileSystemJson: fileSystem.cpp driver.cpp fileSystem.hpp
	g++ -DBOOST -std=c++11 fileSystem.cpp driver.cpp -o fileSystem -lboost_json

fileSystemSpec: fileSystem.cpp fileSystemSpec.cpp fileSystem.hpp
	g++ -std=c++11 fileSystem.cpp fileSystemSpec.cpp -o fileSystemSpec

clean:
	rm fileSystem fileSystemSpec
