tag- Simple file tagging and searching
================================

What you need to build your own jQuery
---------------------------------------
* Make sure that you have Java installed (if you want to build a minified version of jQuery).  
If not, [go to this page](http://java.sun.com/javase/downloads/index.jsp) and download "Java Runtime Environment (JRE) 5.0"

Build Options
--------------

* **download**: Download the source to your computer

* **extract**: Unpack the tarball or zip archive

* **point**: `cd` inside of the unpacked directory

* **install**: Just run `make install` as root

How to use tag
-----------------------------

*Note: If you are using either `rake` or `ant`, substitute your chosen method in place of `make` in the examples below. They work identically for all intents and purposes. Quick reference is also available for `rake` by typing `rake -T` in the `jquery` directory.*

In the main directory of the distribution (the one that this file is in), type
the following to make all versions of jQuery:

    make

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
