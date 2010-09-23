TAG(1)                           User Commands                          TAG(1)

testing this

NAME
       tag - tag and search for files with keywords


SYNOPSIS
       tag -t [[MODIFIER]KEYWORD]... FILE|DIRECTORY
       tag -f [[MODIFIER]KEYWORD]... DIRECTORY
       tag -q FILE|DIRECTORY
       tag LONG-OPTION


DESCRIPTION
       -t     tag a file or directory with the specified keywords

       -f     search  for files and directories that have been tagged with the
              specified keywords

       -q     query what keywords are associated with a file or directory

       --help display this help and exit

       --version
              output version information and exit


KEYWORD MODIFIERS
       When performing file tagging and searching, keywords can optionally  be
       prefixed  by  a modifier character. These modifiers will change how tag
       interprets the keyword for the specified operation.


   File Tagging
       +KEYWORD (or just KEYWORD)
              Will add the keyword KEYWORD to the specified file  only  if  it
              has not already been added.

       -KEYWORD
              Will  remove the keyword KEYWORD from the specified file only if
              it has already been added.

       :KEYWORD
              Will remove any keyword that contains KEYWORD as a substring.

       .KEYWORD
              Will remove any keyword that does not contains KEYWORD as a sub-
              string.


   File Searching
       +KEYWORD (or just KEYWORD)
              Will return files that contain the keyword KEYWORD

       -KEYWORD
              Will filter out returned files that contain the keyword KEYWORD

       :KEYWORD
              Will  return  files  that  contain the keyword KEYWORD as a sub-
              string.

       .KEYWORD
              Will filter out returned files that contain the keyword  KEYWORD
              as a substring.


   Useful Tricks
       tag -f : DIRECTORY
              Will return all files that have been tagged, inside of DIRECTORY

       tag -f + DIRECTORY
              Will return all files that can hold more tags, inside of  DIREC-
              TORY

       tag -f : - DIRECTORY
              Will  return  all  files  that  cannot hold more tags, inside of
              DIRECTORY

       tag -t : TARGET
              Will remove all keywords associated with TARGET


AUTHOR
       Written by Josh Komoroske.


REPORTING BUGS
       Report bugs to jdkomo.dev@gmail.com
       tag source code repository: <http://github.com/sigint/tag>


COPYRIGHT
       Copyright © 2010 Josh Komoroske License BSD:
       THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
       IS"  AND  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
       TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-
       ULAR  PURPOSE  ARE  DISCLAIMED.  IN  NO EVENT SHALL JOSHUA KOMOROSKE BE
       LIABLE FOR ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
       CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF
       SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  OR  BUSI-
       NESS  INTERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY THEORY OF LIABILITY,
       WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
       OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
       ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.




                                September 2010                          TAG(1)
