// This file adds various scenarios within the file-system and how these should be
// addressed by the file system.
//
// The operations supported by the file system are
// * MakeDir
// * WriteFile
// * ReadFile
// * Ls
// * RemoveDir
// * DeleteFile
// * MoveDir
// * RenameFile
// * LinkEntity
// * ChangeDir (implicit)
//
// The file system also supports reading and writing contents to JSON format, which
// makes the file system portable.
//
// We add specs for each of the operations above.

#include "filesystem.hpp"
#include <iostream>

void assertEqual(std::string testName, int result, int expected) {

	if (result != expected) {
		std::cout << "Assertion failed! " << testName << ":" << result << "!= " << expected << std::endl;
	}
}

void assertEqualString(std::string testName, std::string result, std::string expected) {

	if (result != expected) {
		std::cout << "Assertion failed! " << testName << ":" << result << "!= " << expected << std::endl;
	}
}

// MakeDir:
// * Creates a new directory given by the path
void MakeDirTest() {
	entity* root = new dir_entity();

	std::string path = "test";

	int result = makeDir(root, path);

	assertEqual("MakeDirTest", result, 0);
}

// * The parent directory in the path should exist
void MakeDirParentDirNonexistFailure() {
	entity* root = new dir_entity();

	std::string path = "test/test1";

	int result = makeDir(root, path);

	assertEqual("MakeDirParentDirNonexistFailure", result, -1);
}

// * An existing entity with the name of the directory should not exist.
void MakeDirExistingEntityFailure() {
	entity* root = new dir_entity();

	std::string path = "test";

	int result = makeDir(root, path);

	assertEqual("MakeDirExistingEntityFailure", result, 0);

	// Second call with same path.
	result = makeDir(root, path);

	assertEqual("MakeDirExistingEntityFailure", result, -1);
}

// * The directory name should be alphanumeric (or consist of a period after the first character).
void MakeDirNonAlphaNumericNameFailure() {
	entity* root = new dir_entity();

	std::string path = "t#st";

	int result = makeDir(root, path);

	assertEqual("MakeDirNonAlphaNumericNameFailure", result, -1);
}


// WriteFile:
// * creates a new file given by the path.
void WriteFileTest() {
	entity* root = new dir_entity();

	std::string path = "test.txt";

	int result = writeFile(root, path, "Hello World");

	assertEqual("WriteFileTest", result, 0);
}
// * The parent directory in the path should exist.
void writeFileParentDirNonExistentFailure() {
	entity* root = new dir_entity();

	std::string path = "test/test1.txt";

	int result = writeFile(root, path, "Hello World");

	assertEqual("WriteFileParentDirNonExistentFailure", result, -1);
}
// * The existing entity with the name of the file should not exist.
void writeFileExistingEntityFailure() {
	entity* root = new dir_entity();

	std::string path = "test.txt";

	int result = writeFile(root, path, "Hello World");

	assertEqual("writeFileExistingEntityFailure", result, 0);

	result = writeFile(root, path, "Hello World");

	assertEqual("writeFileExistingEntityFailure", result, -1);
}
// * The file name should be alphanumeric (or consist of a period after the first character).
void WriteFileNonAlphanumericNameFailure() {
	entity* root = new dir_entity();

	std::string path = "t#$t.txt";
	int result = writeFile(root, path, "Hello World");
	assertEqual("WriteFileNonAlphanumericNameFailure", result, -1);
}


// ReadFile:
// * reads a file specified by the path.
void readFileTest() {
	entity* root = new dir_entity();

	std::string path = "test.txt";
	int result = writeFile(root, path, "Hello World");

	assertEqual("readFileTest", result, 0);

	auto output = readFile(root, path);
	assertEqualString("readFileTest", output, "Hello World");
}
// * The file should exist in the filesystem.
void readFileNonExistentTest() {
	entity* root = new dir_entity();

	std::string path = "test.txt";
	auto output = readFile(root, path);

	assertEqualString("readFileNonExistentTest", output,"");
}

// Ls:
// * reads the contents of a directory and outputs them in space-seperated string.
void LsTest() {
	entity* root = new dir_entity();

	makeDir(root, "test");
	writeFile(root, "testFile.txt", "Hello World");

	auto output = ls(root, "/");
	assertEqualString("LsTest", output, "test testFile.txt");

	writeFile(root, "test/testFile1.txt", "Hello World Again");
	output = ls(root, "test/");
	assertEqualString("LsTest", output, "testFile1.txt");
}
// * The directory should exist in the filesystem.
void LsNonExistentDirFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test1");
	auto output = ls(root, "test");

	assertEqualString("LsNonExistentDirFailure", output, "");
}

// RemoveDir:
// * Removes a directory given by the path.
void RemoveDirTest() {
	entity* root = new dir_entity();
	makeDir(root, "test");

	auto output = rmDir(root, "test");

	assertEqual("RemoveDirTest", output, 0);
}
// * The removed directory should not be the current directory.
void RemoveDirCurrentFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	auto output = rmDir(root, "/");

	assertEqual("RemoveDirCurrentFailure", output, -1);
}
// * A non-empty directory should not be removed.
void RemoveDirNonEmptyFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	writeFile(root, "test/testFile.txt", "Hello world");

	auto output = rmDir(root, "test");

	assertEqual("RemoveDirNonEmptyFailure", output, -1);
}
// * The directory should exist in the system.
void RemoveDirNonExistentFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	auto output = rmDir(root, "test1");

	assertEqual("RemoveDirNonExistentFailure", output, -1);
}

// DeleteFile:
// * Removes a file from the filesystem that is given by the path.
void RemoveFileTest() {
	entity* root = new dir_entity();

	writeFile(root,"test", "Hello world");

	auto output =removeFile(root, "test");

	assertEqual("RemoveFileTest", output, 0);
}
// * The file should exist in the filesystem.
void RemoveFileNonExistentFailure() {
	entity* root = new dir_entity();

	auto output = removeFile(root,"test");

	assertEqual("RemoveFileNonExistentFailure", output, -1);
}

// MoveDir:
// * The directory specified by the path is moved to a new path.
void MoveDirTest() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	makeDir(root, "child");

	auto output = moveDir(root, "child", "test");

	assertEqual("MoveDirTest", output, 0);

}
// * The directory should exist in the filesystem.
void MoveDirNonExistentFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	auto output = moveDir(root, "child", "test");

	assertEqual("MoveDirNonExistentFailure", output, -1);
}
// * The directory specified by the new path should exist.
void MoveDirNonExistentMoveDirFailure() {
	entity* root = new dir_entity();

	makeDir(root, "child");

	auto output = moveDir(root, "child", "test");

	assertEqual("MoveDirNonExistentMoveDirFailure", output, -1);
}
// * The directory specified by the new path should not have an entity with the name of the directory.
void MoveDirExistingEntityFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	makeDir(root, "child");

	writeFile(root, "test/child", "Hello World");

	auto output = moveDir(root, "child", "test");

	assertEqual("MoveDirExistingEntityFailure", output, -1);

}
// * The directory to be moved if specified by the path ending with (..) should be correctly moved.
void MoveDirPathWithElipses() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	makeDir(root, "child");

	// TODO replace with chDir.
	root = dynamic_cast<dir_entity*>(root)->children["child"];
	auto outupt = moveDir(root, "../child", "../test");

	assertEqual("MoveDirPathWithElipses", outupt, 0);
}

// RenameFile:
// * The file specified by the path is moved to a new path with a new name.
void RenameFileTest() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	writeFile(root, "testfile.txt", "Hello World");

	auto output = renameFile(root, "testfile.txt", "test/newfile.txt");

	auto result = readFile(root, "test/newfile.txt");

	assertEqualString("RenameFileTest", result, "Hello World");
}
// * The file should exist in the filesystem.
void RenameFileNonExistentFailure() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	auto output = renameFile(root, "testfile.txt", "test/newfile.txt");
	assertEqual("RenameFileNonExistentFailure", output, -1);
}
// * The entity with the new path should not exist in the filesystem.
void RenameFileExistingFailure() {
	entity* root = new dir_entity();

	writeFile(root, "testfile.txt", "Hello World");

	makeDir(root, "test");

	writeFile(root, "test/newfile.txt", "Hello World again");

	auto output = renameFile(root, "testfile.txt", "test/newfile.txt");
	assertEqual("RenameFileExistingFailure", output, -1);
}
// * The parent directory in the new path should exist in the file system.
void RenameFileNonExistentDirFailure() {
	entity* root = new dir_entity();

	writeFile(root, "testfile.txt", "Hello World");

	auto output = renameFile(root, "testfile.txt", "test/newfile.txt");
	assertEqual("RenameFileNonExistentDirFailure", output, -1);
}

// LinkEntity:
// * An entity is linked in the filesystem to another entity.
void LinkEntityTest() {
	entity* root = new dir_entity();

	makeDir(root, "test");

	writeFile(root, "testfile.txt", "Hello world");

	auto output = linkEntity(root, "testfile.txt", "test/newfile.txt");

	assertEqual("LinkEntityTest", output, 0);
}
// * The entity should exist in the filesystem.
void LinkEntityNonExistent() {
	entity* root = new dir_entity();

	makeDir(root,"test");

	auto output = linkEntity(root, "testfile.txt", "test/newfile.txt");

	assertEqual("LinkEntityNonExistent", output, -1);
}
// * The parent directory in the path to the other entity should exist in the filesystem.
void LinkEntityNonExistentLinkParent() {
	entity* root = new dir_entity();

	makeDir(root, "test1");

	writeFile(root, "testfile.txt", "Hello World");

	auto output = linkEntity(root, "testfile.txt", "test/newfile.txt");

	assertEqual("LinkEntityNonExistentLinkParent", output, -1);
}
// * The linking entity should not be already present in the filesystem.
void LinkEntityExistingLinkFailure() {
	entity* root = new dir_entity();

	makeDir(root,"test");

	writeFile(root, "testfile.txt", "Hello world");

	writeFile(root, "test/newfile.txt", "Hello world again");

	auto output = linkEntity(root, "testfile.txt", "test/newfile.txt");

	assertEqual("LinkEntityExistingLinkFailure", output, -1);
}


int main(int argc, char** argv) {
	// Run tests.
	MakeDirTest();
	MakeDirParentDirNonexistFailure();
	MakeDirExistingEntityFailure();
	MakeDirNonAlphaNumericNameFailure();
	WriteFileTest();
	writeFileParentDirNonExistentFailure();
	writeFileExistingEntityFailure();
	WriteFileNonAlphanumericNameFailure();
	readFileTest();
	readFileNonExistentTest();
	LsTest();
	LsNonExistentDirFailure();
	RemoveDirTest();
	RemoveDirCurrentFailure();
	RemoveDirNonEmptyFailure();
	RemoveDirNonExistentFailure();
	RemoveFileTest();
	RemoveFileNonExistentFailure();
	MoveDirTest();
	MoveDirNonExistentFailure();
	MoveDirNonExistentMoveDirFailure();
	MoveDirExistingEntityFailure();
	MoveDirPathWithElipses();
	RenameFileTest();
	RenameFileNonExistentFailure();
	RenameFileExistingFailure();
	RenameFileNonExistentDirFailure();
	LinkEntityTest();
	LinkEntityNonExistent();
	LinkEntityNonExistentLinkParent();
	LinkEntityExistingLinkFailure();

	return 0;
}
