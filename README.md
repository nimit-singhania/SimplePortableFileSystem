## Simple Portable FileSystem

This implementation presents a simple filesystem that allows operations like create a directory, write a file,
read directory contents and read files. The implementation also supports reading a filesystem stored as JSON and
writing back the updated filesystem as JSON. By enabling JSON support, the filesystem is portable, can be transfered
to an alternate location and recreated. The operations supported by the filesystem include:

* MakeDir(path): this creates a directory given by the `path`. The operation succeeds if the parent directory is already
  created and an entity with the name of the directory is not already present.
* WriteFile(path, data): this creates a file and writes contents given by `data`. We note that the operation assumes that the parent directory is already created and an entity with the name of the file is not already present.
* Ls(path): this enables reading a directory and print immediate contents of the directory in a space-separated string of names. It returns empty string if the directory is not present or the returned entity is not a directory.
* ReadFile(path): this operation reads contents of a file.

To compile the filesystem:
```
  g++ -std=c++11 filesystem.cpp -o filesystem -lboost_json
```

To run the filesystem:
```
  ./filesystem
```

To run the filesystem and load contents from JSON (and also write back final contents to JSON):
```
  ./filesystem <fs.json>
```

The filesystem accepts the following operations:

* Make directory (note absolute path prefixed with `/` must be provided)
```
   $makedir <absolute_path>

   example:
   $makedir /test1
```
* Write a file
```
  $write_file <absolute_path> <contents>

  example:
  $write_file /test1/testfile.txt Hello World
```

* Show contents of a directory.
```
  $ls <dir_path>

  example:
  $ls /test1
```

* Display contents of a file.
```
  $read_file <absolute_path>

  example:
  $read_file /test1/testfile.txt
```

* Exit the filesystem (and store contents in JSON if available)
```
  $exit
```
