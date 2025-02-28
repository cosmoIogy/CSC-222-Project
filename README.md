# Techshell

Names : Blair Bourque and Brandon Fortes

How it works:
Our CSC 222 Project is a coding project to which a C-Program will act as a Linux Terminal and execute Linux commands.
 The program is called Techshell.c, and once executed will run given Linux Commands repeatedly. 

Implementation:
The program has three steps: Input, Parsing, and Execution. 
Input:
    In the input step, the goal is to have the user input a command and then store it as a char array.
    Additionally, it also has to present as a shell.
    First, we store the current working directory in a char array.
    Then we print out the current working directory from the array.
    Then the user can input a command, and it is stored in a char array using fgets to also include spaces.
    Finally it returns the input.

Parsing:
    In the parsing step, the goal is to convert the input into a format more convenient for execution.
    For this a ShellCommand struct was made. It has 3 variables: 
        A char pointer for input files.
        A char pointer for output files.
        And an array of char arrays for every other argument
    
    The parsing method will first split the user's input by spaces using strtok().
        This also returns the first argument, which is stored.
    Next, a loop is entered and will loop until there are no more arguments.
    If the current argument is a < or >, it will store the next argument in either inputFile or outputFile, respectively.
    Otherwise, it will store the argument into the ShellCommand's arguments array.
    It uses strtok at the end of each loop to move on to the next argument.
    After the loop finishes, it adds a NULL to the next item in the array, as execvp needs the arguments to be null terminated.

Execution:
    In the execution step, the goal is to interpret the ShellCommand into whichever task needs to be executed.
    First, the first element of the arguments is checked to see whether or not it is a cd.
    If it is the code will run chdir() to change the directory.
    This is necessary because cd does not work with execvp.
    After this check, the code will fork.
    In the child process, it first checks to see whether an input file and/or an output file were given.
    If they were, it opens a new file and then links either stdin or stdout to it.
    This handles < and >, as the commands themselves only check stdin and stdout, so changing what they are mapped to will change where the data goes.
    After this, it uses execvp to execute the command. The parent process waits for the child to finish so that they dont desync.
    The execvp method takes in a string for the command and a string array for the arguments, which is why we stored the arguments in the way that we did.
    Thus, the execvp method can take as many arguments as the project limits. Thus, for example, you can remove multiple files with one command.

The project will loop infinitely until the user inputs "exit", at which point the program will end

What's Missing:
The only thing that was not implemented was piping.
Oh yeah also the jobs command wasn't implemented, but there isn't an easy workaround for that one unlike cd.
# Names : Blair Bourque and Brandon Fortes

How it works:
Our CSC 222 Project is a coding project to which a C-Program will act as a Linux Terminal and execute Linux commands. The program is called Techshell.c, and once executed will run given Linux Commands repeatedly. 

Implementation: