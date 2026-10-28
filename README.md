## Simple Portable FileSystem

This implementation presents a simple filesystem that allows operations like create a directory, write a file,
read directory contents and read files. The implementation also supports reading a filesystem stored as JSON and
writing back the updated filesystem as JSON. By enabling JSON support, the filesystem is portable, can be transfered
to an alternate location and recreated. The operations supported by the filesystem include:

* MakeDir(path): this creates a directory given by the `path` relative to the current directory. The operation succeeds if the parent directory is already
  present and an entity with the name of the directory is not already present.
* WriteFile(path, data): this creates a file and writes contents given by `data`. We note that the operation assumes that the parent directory is already present and an entity with the name of the file is not already present.
* Ls(path): this enables reading a directory and print immediate contents of the directory in a space-separated string of names. It returns empty string if the directory is not present or the returned entity is not a directory.
* ReadFile(path): this operation reads contents of a file.
* RmDir(path): this removes an empty directory given by the `path` relative to the current path. The operation succeeds if the directory is already
  created and the directory is empty.
* DeleteFile(path): this operation removes a file given by the `path` relative to the current path. The operation succeeds if the file is already
  present.
* MoveDir(path, dst): this operation moves a directory given by `path` to a new directory `dst` relative to the current path. Note the destination
  directory should already be present and the path should represent a valid directory to be moved. All contents of the directory are also moved.
* RenameFile(path, dstPath): this operation moves and renames a file from `path` to `dstPath` relative to the current path. Note that both `path` and `dstPath` represent complete paths, and the file with `path` should already be present. Also, the parent directory in `dstPath` should already be created.
* Link(path, newpath): this operation links an existing entity with location `path` to a new entity with location `newpath`. We assume that the parent directory in the `newpath` is already created. After the link is established, both paths can be used to access the original entity.
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

* Make directory
```
   $makedir <path>

   example:
   $makedir test1
```
* Write a file
```
  $write_file <path> <contents>

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
  $read_file <path>

  example:
  $read_file /test1/testfile.txt
```

* Remove a directory.
```
  $rmdir <dir_path>

  example:
  $rmdir /test1
```

* Remove a file.
```
  $delete_file <file_path>

  example:
  $delete_file hello.txt
```

* Move a directory to a new location.
```
  $mvdir <path> <dst>

  example:
  $mvdir child_dir new_parent_dir
```

* Rename a file and move the file to a new location.
```
  $rename_file <path> <dst_path>

  example:
  $rename_file test/hello.txt test1/newHello.txt
```

* Link an entity to a new location.
```
  $link <path> <dst_path>

  example:
  $link test/hello.txt test/newHello.txt
```
* Change directory, specified by a path. All subsequent operations are performed relative to this path.
```
  $chdir <path>

  example:
  $chdir /test
```

* Exit the filesystem (and store contents in JSON if available)
```
  $exit
```
