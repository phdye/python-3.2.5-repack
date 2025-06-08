#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>     // EXIT_SUCCESS (0), EXIT_FAILURE (1), setenv(), ...
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>     // For PATH_MAX and realpath
#include <errno.h>      // For errno
#include <ctype.h>      // For toupper()
#include <sys/stat.h>   // For stat()

#define PROC_SELF_EXE             "/proc/self/exe"

#define ENV_VAR_NAME_FMT          "EXEC_WRAPPER__%s"    /* exe_name */
#define ENV_VAR_PID_FMT           "%s__PID"             /* <name> */
#define ENV_VAR_PID_PATH_FMT      "%s__PID_PATH"        /* <name> */

#define TRANSLATED_FILE           "translated.txt"

// populated by identity()
char current_dir[PATH_MAX];
char command_exe[PATH_MAX];
char current_exe[PATH_MAX];
char exe_name_w_ext[PATH_MAX];
char exe_name[PATH_MAX];
char env_var_name[PATH_MAX];
char env_var_pid[PATH_MAX];
char env_var_pid_value[PATH_MAX]; // for setting the environment variable
char pid_path[PATH_MAX];
int parent_pid = 0;

void record_command_line(int argc, char *argv[]);
void identity();
int locate_target_executable(char *target_executable);
int is_executable(const char *path);
char *strtoupper(const char *str);
void perrorf(char * fmt, ...);
int unbuffer(FILE *fp);
void strip_extension(char *str);

int main(int argc, char *argv[]) {

    record_command_line(argc, argv); // fopen failure will exit

    // Make stdout unbuffered
    unbuffer(stdout); // continue even if it fails

    identity(); // exit on failure : realpath, setenv

    // Locate the target executable
    char target_executable[PATH_MAX];
    if (locate_target_executable(target_executable) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if ( strcmp(target_executable, current_exe) == 0 ) {
        fprintf(stderr, "Error: Target executable is the same as the wrapper executable:  '%s'\n",
            target_executable);
        return EXIT_FAILURE;
    }

    execvp(target_executable, argv);

    perrorf("execvp('%s', ...)", target_executable);
    return EXIT_FAILURE;
}

// Populate globals with program executable identity
//
void identity() {
        // Get the path of the command executable
        if (realpath(PROC_SELF_EXE, command_exe) == NULL) {
            perrorf("realpath('%s') failed\n", PROC_SELF_EXE);
            exit(EXIT_FAILURE);
        }
    
        // Resolve it 'just in case'
        if (realpath(command_exe, current_exe) == NULL) {
            fprintf(stderr, "Error: realpath('%s') failed\n", command_exe);
            perror("realpath");
            exit(EXIT_FAILURE);
        }
    
        strncpy(exe_name_w_ext, basename(current_exe), PATH_MAX);
        strncpy(exe_name, exe_name_w_ext, PATH_MAX);
        strip_extension(exe_name);
    
        snprintf(env_var_name, PATH_MAX, ENV_VAR_NAME_FMT, exe_name);
        snprintf(env_var_pid, PATH_MAX, ENV_VAR_PID_FMT, env_var_name);
    
        char *value = getenv(env_var_pid);
        parent_pid = (value != NULL) ? atoi(value) : 0;
    
        if (parent_pid < 0) {
            // Set PID for child process
            sprintf(env_var_pid_value, "%d", getpid());
            if (setenv(env_var_pid, env_var_pid_value, 1) != 0) {
                fprintf(stderr, "Error: setting environment variable '%s'\n", env_var_pid);
                perror("setenv");
                exit(EXIT_FAILURE);
            }
        }
    }

// Write command line to TRANSLATED_FILE
void record_command_line(int argc, char *argv[]) {
    FILE * fp = fopen(TRANSLATED_FILE, "a");
    if (fp == NULL) {
        perrorf("fopen('%s', 'a')", TRANSLATED_FILE);
        exit(EXIT_FAILURE);
    }
    fputs("Command line:", fp);
    for (int i = 0; i < argc; ++i) {
        fprintf(fp, " '%s'", argv[i]);
    }
    fprintf(fp, "\n\n");
    fclose(fp);
}

int locate_target_executable(char *target_executable) {

    char candidate[PATH_MAX];
    char subdir[PATH_MAX];

    snprintf(subdir, sizeof(subdir), "%s/actual", current_dir);

    snprintf(candidate, sizeof(candidate), "%s/%s", subdir, exe_name);
    printf("candidate: %s\n", candidate);
    if (realpath(candidate, target_executable) != NULL && is_executable(target_executable)) {
        strip_extension(target_executable);
        return 0; // Found <name>.exe
    }

    // Step 4: Fatal error if no target is found
    fprintf(stderr, "Fatal Error: Could not locate a valid target executable for wrapped '%s'.\n",
        current_exe);
    return -1;
}

int is_executable(const char *path) {
    return access(path, X_OK) == 0;
}

char *strtoupper(const char *str) {
    static char buffer[PATH_MAX];
    char temp[PATH_MAX];
    strcpy(temp, str);

    size_t len = strlen(temp);
    for (size_t i = 0; i < len; ++i) {
        buffer[i] = toupper((unsigned char)temp[i]);
    }
    buffer[len] = '\0';

    return buffer;
}

void perrorf(char * fmt, ...) {
    char buffer[PATH_MAX];
    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, PATH_MAX, fmt, args);
    perror(buffer);
    va_end(args);
}

// Set fp to be unbuffered
int unbuffer(FILE *fp) {

    if (setvbuf(fp, NULL, _IONBF, 0) != 0) {
        char * name = (fp == stdout) ? "STDOUT" : (fp == stderr ? "STDERR" : "<fp>");
        perrorf("Failed to make %s unbuffered", name);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void strip_extension(char *str) {
    char *dot = strrchr(str, '.');
    if (dot) {
        *dot = '\0';
    }
}
