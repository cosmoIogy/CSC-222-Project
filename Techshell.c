//
// Members: Brandon Fortes, Blair Bourque
// Project: A c program that takes user input and executes it like a linux shell
//

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "stdbool.h"
#include "string.h"
#include <fcntl.h>

#define MAX_DIRECTORY 300    // Max character length of the directory
#define MAX_INPUT 1000 	     // Max character length of the user's input
#define MAX_ARGS 100	     // Max number of arguments in a user's command

// Struct used for storing individual commands
typedef struct {
	char **arguments;
	char *inputFile;
	char *outputFile;
} ShellCommand;

// Prints out the current directory, then returns the user's inputed command as a char array.
char *commandPrompt() {
	// Get the current dirrectory
	char cwd[MAX_DIRECTORY];
	getcwd(cwd, sizeof(cwd));

	// User's input can be up to MAX_INPUT characters
	char *input = malloc(MAX_INPUT * sizeof(char));

	// Print the current directory, then await user's input
	printf("%s$ ", cwd);
	fgets(input, MAX_INPUT * sizeof(char), stdin);

	// Cut off the null character at the end of the user's input
	input[strcspn(input, "\n")] = '\0';

	return input;
}

// Parse's the user's input into a Shell Command struct.
ShellCommand parseCommand(char* input) {
	// Make the array of arguments
	char **args = malloc(MAX_ARGS * sizeof(char));

	// Input and output files for redirection
	char *inputFile = NULL;
	char *outputFile = NULL;

	// Index tracker
	int argCount = 0;

	// Split the user's input into an array of char arrays separated by spaces.
	// Also store the first element of that char array.
	char *arg = strtok(input, " ");

	// Repeat until there are no more arguments
	while (arg != NULL) {

		// Account for input
		if (strcmp(arg, "<") == 0) {

			// The next argument should be the input file. Store it.
			arg = strtok(NULL, " ");

			// If there is actually a next argument, store it in inputFile
			if (arg != NULL) {
				inputFile = arg;
			}
			else {
				printf("Error: No input file given\n");
				break;
			}
		}

		// Account for output
		else if (strcmp(arg, ">") == 0) {

			// The next argument should be the output file. Store it.
			arg = strtok(NULL, " ");

			// If there is actually a next argument, store it in inputFile
			if (arg != NULL) {
				outputFile = arg;
			}
			else {
				printf("Error: No destination file given\n");
				break;
			}
		}

		// Handle any other case.
		else {
			// Store current argument into the arg array, then increment the index.
			args[argCount] = strdup(arg);
			argCount++;
		}

		// Move on to the next argument
		arg = strtok(NULL,  " ");


	}

	// The arg array needs to be null terminated, so do that.
	args[argCount] = NULL;

	// Initialize a new ShellCommand.
	ShellCommand command = {args, inputFile, outputFile};

	return command;
}

// Takes a ShellCommand and executes it.
void executeCommand(ShellCommand command) {

	// special case to handle cd since it doesnt work with execvp
	if (strcmp(command.arguments[0], "cd") == 0) {

		// Error handler in case there is no destination given
		if (command.arguments[1] == NULL) {
			printf("cd: missing arugment\n");
		}

		// Move to the destination directory. If it throws an error
		// that means either the directory was restricted or doesnt exist.
		else if (chdir(command.arguments[1]) != 0) {
			perror("ERROR 13");
		}

		return;
	}

	// Fork to create a child process
	pid_t pid = fork();

	// Only the child should be running the commands
	if (pid == 0) {

		// Check to see if an input file was given
		if (command.inputFile) {

			// Open a new file to read from
			int infile = open(command.inputFile, O_RDONLY);

			if (infile < 0) {
				printf("ERROR: no input file given\n");
			}

			// Store the text from the input file into stdin
			dup2(infile, STDIN_FILENO);

			// Close the file to prevent leakage
			close(infile);
		}

		// Check to see if an output file was given
		if (command.outputFile) {

			// Open a new file to write too.
			int outfile = open(command.outputFile, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);

			if (outfile < 0) {
				printf("ERROR: no output file given\n");
			}

			// Map stdout to the newly open file.
			dup2(outfile, STDOUT_FILENO);

			// Close the file to prevent leakage
			close(outfile);
		}


		// Default command handler
		execvp(command.arguments[0], command.arguments);

		// execvp will terminate this process and start a new one unless it fails.
		// This will print out if execvp fails for whatever reason.
		printf("ERROR 2 (No such file or directory) \n");
	}

	// Parent Code
	else {
		// Parent waits until child finishes.
		wait(NULL);
	}
}

int main() {

	// User's input
	char *input;

	// The command parsed from the user's input
	ShellCommand command;

	// Run until the user exits manually
	while(true) {

		// Reset input to prevent leakage into stdin
		input = NULL;

		// Get the user's input
		input = commandPrompt();

		// Parse the user's input into a command
		command = parseCommand(input);

		// Exit if requested,
		if (strcmp(command.arguments[0], "exit") == 0) {
			exit(0);
		}

		// Otherwise, execute the command
		else {
			executeCommand(command);
		}
	}

	exit(0);
}

