tag- Simple file tagging and searching
================================

Build Options
--------------

* **download**: Download the source to your computer

* **extract**: Unpack the tarball or zip archive

* **point**: `cd` inside of the unpacked directory

* **install**: Just run `make install` as root

How to use tag
-----------------------------

Tagging a file or directory:

`tag -t [tags] /path/to/target`

tag can take 4 different modifiers for a tag:

`+[tag]` or `[tag]` adds tag `[tag]` to the target
    
`-[tag]` removes tag `[tag]` from the target
    
`%[tag]` removes all tags where `[tag]` is a substring 
    
`:[tag]` removes all tags where `[tag]` is not a substring
    
Searching for files or directories:

`tag -f [tags] /search/path/root/`

tag can take 4 different modifiers for a tag:

`+[tag]` or `[tag]` searches for targets that are tagged with `[tag]`
    
`-[tag]` searches for targets that are not tagged with `[tag]`
    
`%[tag]` searches for targets that contain `[tag]` as a substring
    
`:[tag]` searches for targets that do not contain `[tag]` as a substring
    
Querying a file / directory's tag(s):

`tag -q /path/to/target`

