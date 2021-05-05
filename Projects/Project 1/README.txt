Ryan Coslove rmc326

Testing strategy:

1. For Part 1 I used varying text files within the directory. I would compile the program and run the files as the 2nd argument. 
I checked to make sure it was a positive integer for the 1st argument, and then checked for a file to execute with. Test cases 
like -5 or a letter in the first argument allowed exit failure. 2nd argument I tested invalid files and directories. The text files 
I used varies in spacing, paragraphs and new lines. I also used text files with long words to see if it would land on its own line, 
etc. Punctuation was treated similarly to words.

2. For Part 2, I created directories within the program's directory. I created text files within the new directories, as well as
directories within those directories. I tested to see if the text files would gain the "wrap" prefix, and also tested that any 
file already containing with a "wrap" prefix would be overwritten. Assuming all conditions were met of a valid directory and existing
files within that directory, test to see if the wrap version of the files were created and correct.
