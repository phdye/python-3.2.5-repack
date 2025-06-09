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

typedef struct {
    char current_exe        [PATH_MAX];
    char bin_dir            [PATH_MAX];
    char target_exe         [PATH_MAX];
    char prefix             [PATH_MAX];
    char exe_name           [PATH_MAX];
    char env_var_name       [PATH_MAX];
    char env_var_pid        [PATH_MAX];
    char env_var_pid_value  [PATH_MAX]; // for setting the environment variable
    char pid_path           [PATH_MAX];
} context_t ;

int parent_pid = 0;
static int wrapper_verbose = 0;

#define VERBOSE_PRINT(...)\
    do {\
        if (wrapper_verbose) {\
            printf(__VA_ARGS__);\
        }\
    } while (0)

void identity(context_t * ctx);
int environment(context_t * ctx);
int locate_target_exe(context_t * ctx);

int is_executable(const char *path);
char *strtoupper(const char *str);
void perrorf(char * fmt, ...);
int unbuffer(FILE *fp);
void strip_extension(char *str);

int main(int argc, char *argv[]) {

    context_t ctx;

    char *env = getenv("WRAPPER_VERBOSE");
    wrapper_verbose = (env && atoi(env) != 0);

    // Make stdout unbuffered
    unbuffer(stdout); // continue even if it fails

    identity(&ctx); // exit on failure : realpath, setenv

    if (environment(&ctx) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Locate the target executable
    if (locate_target_exe(&ctx) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if ( strcmp(ctx.target_exe, ctx.current_exe) == 0 ) {
        fprintf(stderr, "Error: Target executable is the same as the wrapper executable:  '%s'\n",
            ctx.target_exe);
        return EXIT_FAILURE;
    }

    execvp(ctx.target_exe, argv);

    perrorf("execvp('%s', ...)", ctx.target_exe);
    return EXIT_FAILURE;
}

// Populate globals with program executable identity
//
void identity(context_t* ctx) {

    char parent[PATH_MAX];
    char tmp_path[PATH_MAX];

    // Get the path of the command executable
    if (realpath(PROC_SELF_EXE, ctx->current_exe) == NULL) {
        perrorf("realpath('%s') failed\n", PROC_SELF_EXE);
        exit(EXIT_FAILURE);
    }
    VERBOSE_PRINT("Current executable     :  '%s'\n", ctx->current_exe);

    strncpy(ctx->exe_name, basename(ctx->current_exe), PATH_MAX);
    VERBOSE_PRINT("Executable basename    :  '%s'\n", ctx->exe_name);

    VERBOSE_PRINT("Current executable     :  '%s'\n", ctx->current_exe);

    strncpy(tmp_path, ctx->current_exe, PATH_MAX);
    strncpy(ctx->bin_dir, dirname(tmp_path), PATH_MAX);
    VERBOSE_PRINT("Bin dir                :  '%s'\n", ctx->bin_dir);
    VERBOSE_PRINT("Current executable     :  '%s'\n", ctx->current_exe);

    strncpy(tmp_path, ctx->bin_dir, PATH_MAX);
    strncpy(ctx->prefix, dirname(tmp_path), PATH_MAX);
    VERBOSE_PRINT("Prefix                 :  '%s'\n", ctx->prefix);
    VERBOSE_PRINT("Current executable     :  '%s'\n", ctx->current_exe);

    snprintf(ctx->env_var_name, PATH_MAX, ENV_VAR_NAME_FMT, ctx->exe_name);
    snprintf(ctx->env_var_pid, PATH_MAX, ENV_VAR_PID_FMT, ctx->env_var_name);

    char *value = getenv(ctx->env_var_pid);
    parent_pid = (value != NULL) ? atoi(value) : 0;

    if (parent_pid < 0) {
        // Set PID for child process
        sprintf(ctx->env_var_pid_value, "%d", getpid());
        if (setenv(ctx->env_var_pid, ctx->env_var_pid_value, 1) != 0) {
            fprintf(stderr, "Error: setting environment variable '%s'\n", ctx->env_var_pid);
            perror("setenv");
            exit(EXIT_FAILURE);
        }
    }

    VERBOSE_PRINT("Current executable     :  '%s'\n", ctx->current_exe);
}

int locate_target_exe(context_t* ctx) {

    char candidate[PATH_MAX];

    snprintf(candidate, sizeof(candidate), "%s/actual/%s", ctx->bin_dir, ctx->exe_name);
    VERBOSE_PRINT("Candidate              :  '%s'\n", candidate);
    if (realpath(candidate, ctx->target_exe) != NULL && is_executable(ctx->target_exe)) {
        return 0; // Found <name>.exe
    }

    // Fatal error if no target is found
    fprintf(stderr, "Fatal Error: Could not locate a valid target executable for wrapped '%s'.\n",
        ctx->current_exe);
    return -1;
}

int environment(context_t * ctx) {
    char lib[PATH_MAX];
    char *current_path, *new_path;
    size_t size;

    snprintf(lib, PATH_MAX, "%s/lib", ctx->prefix);

    VERBOSE_PRINT("Library                :  '%s'\n", lib);
    current_path = getenv("LD_LIBRARY_PATH");
    if (current_path == NULL) {
        current_path = "";
    }

    size = strlen(current_path) + strlen(lib) + 2;
    new_path = (char*) malloc(size);
    if (!new_path) {
        fprintf(stderr, "wrapper:  Out of memory");
        abort();
    }
    snprintf(new_path, size, "%s:%s", lib, current_path);

    VERBOSE_PRINT("LD_LIBRARY_PATH        :  '%s'\n", new_path);

    if (setenv("LD_LIBRARY_PATH", new_path, 1) != 0) {
        fprintf(stderr, "Error: setting environment variable 'LD_LIBRARY_PATH'\n");
        perror("setenv");
        free(new_path);
        return EXIT_FAILURE;
    }
    free(new_path);

    if (setenv("PIP_DISABLE_PIP_VERSION_CHECK", "1", 1) != 0) {
        fprintf(stderr, "Error: setting environment variable 'PIP_DISABLE_PIP_VERSION_CHECK'\n");
        perror("setenv");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
