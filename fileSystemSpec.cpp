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


// MakeDir:
// * Creates a new directory given by the path
// * The parent directory in the path should exist
// * An existing entity with the name of the directory should not exist.
// * The directory name should be alphanumeric (or consist of a period after the first character).


// WriteFile:
// * creates a new file given by the path.
// * The parent directory in the path should exist.
// * The existing entity with the name of the file should not exist.
// * The file name should be alphanumeric (or consist of a period after the first character).


// ReadFile:
// * reads a file specified by the path.
// * The file should exist in the filesystem.


// Ls:
// * reads the contents of a directory and outputs them in space-seperated string.
// * The directory should exist in the filesystem.

// RemoveDir:
// * Removes a directory given by the path.
// * The removed directory should not be the current directory.
// * A non-empty directory should not be removed.
// * The directory should exist in the system.

// DeleteFile:
// * Removes a file from the filesystem that is given by the path.
// * The file should exist in the filesystem.

// MoveDir:
// * The directory specified by the path is moved to a new path.
// * The directory should exist in the filesystem.
// * The directory specified by the new path should exist.
// * The directory specified by the new path should not have an entity with the name of the directory.
// * The directory to be moved if specified by the path ending with (..) should be correctly moved.

// RenameFile:
// * The file specified by the path is moved to a new path with a new name.
// * The file should exist in the filesystem.
// * The entity with the new path should not exist in the filesystem.
// * The parent directory in the new path should exist in the file system.

// LinkEntity:
// * An entity is linked in the filesystem to another entity.
// * The entity should exist in the filesystem.
// * The parent directory in the path to the other entity should exist in the filesystem.
// * The linking entity should not be already present in the filesystem.
