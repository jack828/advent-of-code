#include <argp.h>
#include <fcntl.h>
#include <glob.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <uv.h>
#define ANSI_COLOUR_RED "\x1b[31m"
#define ANSI_COLOUR_GREEN "\x1b[32m"
#define ANSI_COLOUR_YELLOW "\x1b[33m"
#define ANSI_COLOUR_BLUE "\x1b[34m"
#define ANSI_COLOUR_MAGENTA "\x1b[35m"
#define ANSI_COLOUR_CYAN "\x1b[36m"
#define ANSI_COLOUR_RESET "\x1b[0m"

#define UV_CHECK(r, msg)                                                       \
  if (r < 0) {                                                                 \
    fprintf(stderr, "%s: %s\n", msg, uv_strerror(r));                          \
    exit(1);                                                                   \
  }
#define GET_T_ID syscall(SYS_gettid)

/*
 * Parts of this file was taken with love from
 * https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html
 * Thanks to the authors of GNU libc!
 */

#define ONE_MS_IN_US 1000
#define ONE_S_IN_US 1000000
#define ONE_M_IN_US 60000000

static uv_loop_t *uv_loop;
static uv_fs_event_t *fs_watcher = NULL;
static uv_timer_t *program_start_timer = NULL;
static uv_timer_t *program_end_timer = NULL;
static uv_timer_t *src_debounce_timer = NULL;
size_t src_directory_length = 1024;
static uv_timer_t *exe_debounce_timer = NULL;
static const uint64_t DEBOUNCE_TIMEOUT_MS = 150;

typedef struct exe_context_t exe_context_t;

struct exe_context_t {
  uv_work_t *req;
  char *dir;
  pid_t child_pid;
  pid_t thread_pid;
  volatile atomic_bool cancel_flag;
};

exe_context_t *ctx;

char *formatTime(long time) {
  char *buf = calloc(24, sizeof(char));
  if (time == 0) {
    sprintf(buf, "%s", "--");
  } else if (time < ONE_MS_IN_US) {
    sprintf(buf, "%ld µs", time);
  } else if (time < ONE_S_IN_US) {
    sprintf(buf, "%ld.%02ld ms", time / 1000, (time % 1000) / 10);
  } else if (time < ONE_M_IN_US) {
    sprintf(buf, "%ld.%03ld s", time / ONE_S_IN_US,
            (time % ONE_S_IN_US) / 1000);
  } else {
    sprintf(buf, "%ld:%02ld.%03ld m", time / ONE_M_IN_US,
            (time % ONE_M_IN_US) / ONE_S_IN_US, (time % ONE_S_IN_US) / 1000);
  }
  return buf;
}

static void on_compile_exit(uv_process_t *req, int64_t exit_status,
                            int term_signal) {
  if (exit_status) {
    fprintf(stderr,
            ANSI_COLOUR_RED
            "Failed to compile, exit status %ld for PID %d\n" ANSI_COLOUR_RESET,
            exit_status, req->pid);
    fflush(stderr);
  }
  uv_close((uv_handle_t *)req, (uv_close_cb)free);
}

void terminate_child(int signum) {
  // signal handler is required for wait4 to be interrupted by SIGCHLD
}

void work_fn(uv_work_t *req) {
  exe_context_t *context = ((exe_context_t *)req->data);
  int64_t tid = GET_T_ID;
  // fprintf(stderr, ">thread pid %ld\n", tid);
  context->thread_pid = tid;
  context->child_pid = fork();

  char target_path[src_directory_length];
  sprintf(target_path, "%smain.out", context->dir);
  struct timeval start, end;
  struct rusage usage;
  gettimeofday(&start, NULL);

  if (context->child_pid == 0) {
    // Child process
    execl(target_path, target_path, NULL);
    _exit(EXIT_FAILURE); // die if it gets here, above line replaces program
                         // image
  } else if (context->child_pid > 0) {
    // In the parent process (worker thread)
    // fprintf(stderr, ">child_pid %d\n", context->child_pid);

    // allegedly how wait4 can be interrupted, but i never got it working
    struct sigaction sa = {0};
    sa.sa_handler = terminate_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    int status;
    pid_t pid;
    while (1) {
      pid = wait4(context->child_pid, &status, 0, &usage);
      gettimeofday(&end, NULL);

      // printf(">loop %d, errno %d\n", pid, errno);
      if (pid == -1 && errno == EINTR) {
        // TODO never works with this, just hangs
        // does not get here
        if (context->cancel_flag) {
          printf(">Cancellation requested, terminating child\n");
          kill(context->child_pid, SIGKILL);
          break;
        }
        // Signal received, reattempt wait4 if needed
        continue;
      } else if (pid == context->child_pid) {
        if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
          printf(ANSI_COLOUR_GREEN
                 "%s exited with status 0\n" ANSI_COLOUR_RESET,
                 target_path);
        } else {
          printf(ANSI_COLOUR_RED
                 "%s exited abnormally with status %d \n" ANSI_COLOUR_RESET,
                 target_path, WEXITSTATUS(status));
        }
        fflush(stdout);
        long s = end.tv_sec - start.tv_sec;
        long us = end.tv_usec - start.tv_usec;
        long total = (s * 1000000) + us;

        char *runtime = formatTime(total);
        printf(ANSI_COLOUR_MAGENTA "Time:\t%s\n", runtime);
        printf("Memory:\t%ld KB\n" ANSI_COLOUR_RESET, usage.ru_maxrss);
        fflush(stdout);
        free(runtime);

        break;
      } else if (errno) {
        perror("Error in wait4");
        break;
      }
    }
    context->cancel_flag = 0;
  } else {
    // Error
    perror("fork() failed");
    exit(EXIT_FAILURE);
  }
}

void after_work_cb(uv_work_t *req, int status) { free(req); }

static void on_exe_debounce_timer(uv_timer_t *handle) {
  printf(ANSI_COLOUR_BLUE "Running program...\n" ANSI_COLOUR_RESET);
  fflush(stdout);
  if (ctx->req) {
    printf("killing active thread %d child %d\n", ctx->thread_pid,
           ctx->child_pid);
    if (ctx->child_pid < 1) {
      fprintf(stderr, "safety net engaged, would have been bad!\n");
    } else {
      kill(ctx->child_pid, SIGKILL);
    }
    ctx->cancel_flag = 1;
    while (ctx->cancel_flag) {
      uv_sleep(100);
    }
  }
  uv_work_t *req = malloc(sizeof(uv_work_t));
  req->data = ctx;
  ctx->req = req;
  ctx->dir = (char *)handle->data;
  ctx->child_pid = -1;
  ctx->cancel_flag = 0;

  uv_queue_work(uv_loop, req, work_fn, after_work_cb);
}

static void on_src_debounce_timer(uv_timer_t *handle) {
  // TODO mutex probably cool/correct to restrict this?
  // https://docs.libuv.org/en/stable/guide/threads.html#mutexes
  printf(ANSI_COLOUR_CYAN "Recompiling source...\n" ANSI_COLOUR_RESET);
  fflush(stdout);
  // compile the source
  uv_process_t *compile_process = malloc(sizeof(uv_process_t));
  uv_process_options_t options = {0};
  char *args[] = {"gcc",
                  "-Wall",
                  "-Wextra",
                  "-Werror=pedantic",
                  "-Wundef",
                  "-Wshadow",
                  "-Wformat=2",
                  "-Wfloat-equal",
                  "-Wunreachable-code",
                  "-Wpedantic",
                  "-std=gnu17",
                  "-Wno-unused-parameter",
                  "-O3",
                  "-lm",
                  "main.c",
                  "-o",
                  "main.out",
                  NULL};
  options.file = args[0];
  options.args = args;
  options.cwd = (char *)handle->data;
  options.stdio_count = 3;
  uv_stdio_container_t child_stdio[3];
  child_stdio[0].flags = UV_IGNORE;     // stdin
  child_stdio[1].flags = UV_INHERIT_FD; // stdout
  child_stdio[1].data.fd = 1;
  child_stdio[2].flags = UV_INHERIT_FD; // stderr
  child_stdio[2].data.fd = 2;
  options.stdio = child_stdio;
  options.exit_cb = on_compile_exit;
  int ret = uv_spawn(uv_loop, compile_process, &options);
  UV_CHECK(ret, "compile process spawn");
}

static void program_start(uv_timer_t *timer) {
  uv_timer_stop(timer);
  uv_close((uv_handle_t *)timer, NULL);
  printf("program_start!\n");
  // TODO trigger callbacks
}

static void program_end(uv_timer_t *timer) {
  uv_timer_stop(timer);
  uv_close((uv_handle_t *)timer, NULL);

  if (ctx->req && ctx->child_pid > 1) {
    ctx->cancel_flag = 1;
    // kill(ctx->child_pid, SIGCHLD);
    usleep(100000);
    // kill(ctx->child_pid, SIGKILL);
  }
}

static void on_walk_cleanup(uv_handle_t *handle, void *data) {
  if (!uv_is_closing(handle)) {
    uv_close(handle, NULL);
  }
}

static void on_close() {
  uv_timer_start(program_end_timer, program_end, 0, 0);
  uv_run(uv_loop, UV_RUN_ONCE);
  uv_stop(uv_loop);
  uv_run(uv_loop, UV_RUN_DEFAULT);
  uv_walk(uv_loop, on_walk_cleanup, NULL);
  uv_run(uv_loop, UV_RUN_DEFAULT);
  int ret = uv_loop_close(uv_loop);
  if (ret != 0) {
    fprintf(stderr, "uv_loop_close did not return 0!\n");
  }
  free(program_start_timer);
  free(program_end_timer);
  free(src_debounce_timer->data);
  free(src_debounce_timer);
  free(exe_debounce_timer->data);
  free(exe_debounce_timer);
  free(fs_watcher);
}

static void on_signal(uv_signal_t *signal, int signum) {
  if (fs_watcher && uv_is_active((uv_handle_t *)&fs_watcher)) {
    uv_fs_event_stop(fs_watcher);
  }
  uv_signal_stop(signal);
  if (fs_watcher) {
    uv_close((uv_handle_t *)fs_watcher, on_close);
  } else {
    on_close();
  }
}

static void on_fs_event(uv_fs_event_t *handle, const char *filename, int events,
                        int status) {
  UV_CHECK(status, "FS Event failure");
  // Vim tmp file write or not a change event
  if (strcmp(filename, "4913") == 0 || !(events & UV_CHANGE)) {
    // ignore them
    return;
  }
  if (strcmp(filename, "example.txt") == 0 ||
      strcmp(filename, "input.txt") == 0 || strcmp(filename, "main.c") == 0) {
    uv_fs_event_getpath(handle, src_debounce_timer->data,
                        &src_directory_length);
    // i get multiple calls per write, wait for it to settle
    uv_timer_stop(src_debounce_timer);
    uv_timer_start(src_debounce_timer, on_src_debounce_timer,
                   DEBOUNCE_TIMEOUT_MS, 0);
  }

  if (strcmp(filename, "main.out") == 0) {
    uv_fs_event_getpath(handle, exe_debounce_timer->data,
                        &src_directory_length);
    // i get MANY MANY calls per compile, wait for it to settle
    uv_timer_stop(exe_debounce_timer);
    uv_timer_start(exe_debounce_timer, on_exe_debounce_timer,
                   DEBOUNCE_TIMEOUT_MS, 0);
  }

  // otherwise, we don't care
}

const char *argp_program_version = "aoc development monitor 1.0";
const char *argp_program_bug_address = "<me@jackburgess.dev>";

static char doc[] = "Advent of Code Development Utility (C only)";

static char args_doc[] = "";

static struct argp_option options[] = {
    {.name = "year",
     .key = 'y',
     .arg = "YEAR",
     // .flags = OPTION_ARG_OPTIONAL,
     .doc = "Year e.g. 2022",
     .group = 0},
    {.name = "day", .key = 'd', .arg = "DAY", .doc = "Day e.g. 20", .group = 1},
    {0}};

struct arguments {
  char *year;
  char *day;
  char *path;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'y':
    arguments->year = arg;
    break;
  case 'd':
    arguments->day = arg;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, NULL, NULL, NULL};

int main(int argc, char **argv) {
  struct arguments arguments;
  char year[5];
  char day[3];
  time_t rawtime;
  struct tm *timeInfo;

  time(&rawtime);
  timeInfo = localtime(&rawtime);

  strftime(year, sizeof(year), "%Y", timeInfo);
  strftime(day, sizeof(day), "%d", timeInfo);
  /* Default values */
  arguments.year = year;
  arguments.day = day;
  arguments.path = "./";

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  printf(" -- Advent of Code Development Utility --\n");

  printf("Running: %s Day %s\n", arguments.year, arguments.day);

  // please don't make your paths this long...
  char path[8192] = {0};
  strcat(path, arguments.path);
  strcat(path, arguments.year);
  strcat(path, "/");
  if (strlen(arguments.day) < 2) {
    strcat(path, "0");
  }
  strcat(path, arguments.day);
  strcat(path, "/");

  glob_t files;
  glob(path, 0, NULL, &files);
  u_int64_t fileCount = files.gl_pathc;

  printf("Path: %s with %lu files\n", path, fileCount);

  if (fileCount == 0) {
    printf("No solutions found. Go solve some!\n");
    exit(1);
  }

  globfree(&files);
  uv_loop = uv_default_loop();
  int status;

  uv_signal_t sigint, sigterm;
  uv_signal_init(uv_loop, &sigint);
  uv_signal_start(&sigint, on_signal, SIGINT);
  uv_signal_init(uv_loop, &sigterm);
  uv_signal_start(&sigterm, on_signal, SIGTERM);

  fs_watcher = malloc(sizeof(uv_udp_t));

  status = uv_fs_event_init(uv_loop, fs_watcher);
  UV_CHECK(status, "fs_watcher init");

  status = uv_fs_event_start(fs_watcher, on_fs_event, path, 0);
  UV_CHECK(status, "fs_watcher start");

  program_start_timer = malloc(sizeof(uv_timer_t));
  status = uv_timer_init(uv_loop, program_start_timer);
  UV_CHECK(status, "program_start timer_init");
  status = uv_timer_start(program_start_timer, program_start, 0, 0);
  UV_CHECK(status, "program_start timer_start");

  program_end_timer = malloc(sizeof(uv_timer_t));
  status = uv_timer_init(uv_loop, program_end_timer);
  UV_CHECK(status, "program_end timer_init");

  src_debounce_timer = malloc(sizeof(uv_timer_t));
  src_debounce_timer->data = calloc(src_directory_length, sizeof(char));
  status = uv_timer_init(uv_loop, src_debounce_timer);
  UV_CHECK(status, "src_debounce_timer timer_init");

  exe_debounce_timer = malloc(sizeof(uv_timer_t));
  exe_debounce_timer->data = calloc(src_directory_length, sizeof(char));
  status = uv_timer_init(uv_loop, exe_debounce_timer);
  UV_CHECK(status, "exe_debounce_timer timer_init");

  ctx = malloc(sizeof(exe_context_t));
  *ctx = (exe_context_t){.req = NULL, .child_pid = -1, .cancel_flag = 0};

  return uv_run(uv_loop, UV_RUN_DEFAULT);
}
