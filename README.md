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

`+[tag]` or `[tag]`   adds tag `[tag]` to the target

`-[tag]`            removes tag `[tag]` from the target

`%[tag]`            removes all tags where `[tag]` is a substring 

`:[tag]`            removes all tags where `[tag]` is not a substring

*Here are the individual items that are buildable from the Makefile:*

    make init

Pull in all the external dependencies (QUnit, Sizzle) for the project.

    make jquery

The standard, uncompressed, jQuery code.  
Makes: `./dist/jquery.js`

    make min

A compressed version of jQuery (made the Closure Compiler).  
Makes: `./dist/jquery.min.js`

    make lint

Tests a build of jQuery against JSLint, looking for potential errors or bits of confusing code.

    make selector

Builds the selector library for jQuery from Sizzle.  
Makes: `./src/selector.js`

Finally, you can remove all the built files using the command:
  
    make clean

Building to a different directory
----------------------------------

If you want to build jQuery to a directory that is different from the default location, you can...

**Make only:** Specify the PREFIX directory, for example:
  
    make PREFIX=/home/john/test/ [command]
    
With this example, the output files would be contained in `/home/john/test/dist/`

**Rake only:** Define the DIST_DIR directory, for example:

    rake DIST_DIR=/home/john/test/ [command]
    
With this example, the output files would be contained in `/home/john/test/`

*In both examples, `[command]` is optional.*

**Ant only:** You cannot currently build to another directory when using Ant.

Questions?
----------

If you have any questions, please feel free to ask them on the jQuery
mailing list, which can be found here:  
[http://docs.jquery.com/Discussion](http://docs.jquery.com/Discussion)
