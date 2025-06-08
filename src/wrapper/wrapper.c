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
#include "whereami.h"

#define PROC_SELF_EXE             "/proc/self/exe"

#define ENV_VAR_NAME_FMT          "EXEC_WRAPPER__%s"    /* exe_name */
#define ENV_VAR_PID_FMT           "%s__PID"             /* <name> */
#define ENV_VAR_PID_PATH_FMT      "%s__PID_PATH"        /* <name> */

#define TRANSLATED_FILE           "translated.txt"

// populated by identity()
char current_dir[PATH_MAX];
char prefix[PATH_MAX];
char current_exe[PATH_MAX];
char exe_name[PATH_MAX];
char env_var_name[PATH_MAX];
char env_var_pid[PATH_MAX];
char env_var_pid_value[PATH_MAX]; // for setting the environment variable
char pid_path[PATH_MAX];
int parent_pid = 0;

void record_command_line(int argc, char *argv[]);
void identity();
void environment(char * prefix);
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

    environment(prefix);

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

    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        printf("Current working dir    :  '%s'\n", current_dir);
    } else {
        perror("getcwd() error");
        abort();
    }

    // Get the path of the command executable
    if (realpath(PROC_SELF_EXE, current_exe) == NULL) {
        perrorf("realpath('%s') failed\n", PROC_SELF_EXE);
        exit(EXIT_FAILURE);
    }
    printf("Current executable     :  '%s'\n", current_exe);

    strncpy(exe_name, basename(current_exe), PATH_MAX);
    printf("Executable basename    :  ''%s'\n", exe_name);

    strncpy(prefix, dirname(dirname(current_exe)), PATH_MAX);
    printf("Prefix                 :  '%s'\n", prefix);

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

void environment(char * prefix) {
    char lib[PATH_MAX];
    char *current_path, *new_path;
    size_t size;

    snprintf(lib, PATH_MAX, "%s/lib", prefix);

    printf("Library                :  '%s'\n", lib);
    current_path = getenv("LD_LIBRARY_PATH");

    size = strlen(current_path) + strlen(lib) + 2 ;
    new_path = (char*) malloc(size);
    if (!new_path) {
        fprintf(stderr,"wrapper:  Out of memory");
        abort();
    }
    snprintf(new_path, size, "%s:%s", lib, current_path);

    printf("LD_LIBRARY_PATH        :  '%s'\n", new_path);

    setenv("LD_LIBRARY_PATH", new_path, 1);
}

// export LD_LIBRARY_PATH="$PREFIX/lib:$LD_LIBRARY_PATH"

char * get_current_executable() {
    char* path = NULL;
    int length, dirname_length;
    int i;
  
    length = wai_getExecutablePath(NULL, 0, &dirname_length);
    if (length <= 0) {
        return NULL;
    }

    path = (char*)malloc(length + 1);
    if (!path) {
        fprintf(stderr,"wrapper:  Out of memory");
        abort();
    }

    wai_getExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    
    return path;
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
    printf("Candidate              :  '%s'\n", candidate);
    if (realpath(candidate, target_executable) != NULL && is_executable(target_executable)) {
        return 0; // Found <name>.exe
    }

    // Fatal error if no target is found
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
