#include <argp.h>
#include <fcntl.h>
#include <glob.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/*
 * Parts of this file was taken with love from
 * https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html
 * Thanks to the authors of GNU libc!
 *
 * Other parts (namely the lovely tabular output) taken with love from:
 * https://github.com/Slash-by-Zero/AoC
 * Thanks to Slash-by-Zero!
 */

#define ONE_MS_IN_US 1000
#define ONE_S_IN_US 1000000
#define ONE_M_IN_US 60000000

const char *argp_program_version = "aoc benchmark 1.0";
const char *argp_program_bug_address = "<me@jackburgess.dev>";

static char doc[] = "Advent of Code Benchmarking Utility (C only)";

static char args_doc[] = "";

static struct argp_option options[] = {
    {.name = "year",
     .key = 'y',
     .arg = "YEAR",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Year e.g. 2022. If not provided uses current year.",
     .group = 0},
    {.name = "day",
     .key = 'd',
     .arg = "DAY",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Day e.g. 20. If not provided shows all days in the year.",
     .group = 1},
    {.name = "style",
     .key = 's',
     .arg = "STYLE",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Style can be one of utf16, utf8, csv. Default utf16.",
     .group = 2},
    {.name = "path",
     .key = 'p',
     .arg = "PATH",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Path to AoC solutions. Default \"./\".",
     .group = 3},
    {.name = "runs",
     .key = 'r',
     .arg = "RUNS",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Number of times to run each solution. Default 1024.",
     .group = 4},
    {.name = "timeLimit",
     .key = 't',
     .arg = "SECONDS",
     .flags = OPTION_ARG_OPTIONAL,
     .doc = "Maximum time (seconds) allowed for each file to complete [RUNS] "
            "runs. Default 30s.",
     .group = 5},
    {0}};

typedef struct runtime_t {
  long min;
  long max;
  long avg;
  long total;
  long runs;
  long *times;
  char *path;
} runtime_t;

typedef struct table_style_t {
  char *hdash1, *hdash2, *vdash, *hvdash1, *hvdash2;
  bool hline;
} table_style_t;

static struct table_style_t style_utf16 = {.hline = true,
                                           .hdash1 = "\u2500",
                                           .hdash2 = "\u2550",
                                           .vdash = "\u2502",
                                           .hvdash1 = "\u253c",
                                           .hvdash2 = "\u256a"},
                            style_utf8 = {.hline = true,
                                          .hdash1 = "-",
                                          .hdash2 = "=",
                                          .vdash = "|",
                                          .hvdash1 = "+",
                                          .hvdash2 = "‡"},
                            style_csv = {.hline = true, .vdash = ";"};

struct arguments {
  char *year;
  char *day;
  table_style_t *style;
  char *path;
  int runs;
  long maxTime;
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
  case 's':
    if (strcmp(arg, "utf16") == 0) {
      arguments->style = &style_utf16;
    } else if (strcmp(arg, "utf8") == 0) {
      arguments->style = &style_utf8;
    } else if (strcmp(arg, "csv") == 0) {
      arguments->style = &style_csv;
    } else {
      return ARGP_ERR_UNKNOWN;
    }
  case 'p':
    arguments->path = arg;
    break;
  case 'r':
    arguments->runs = atoi(arg);
    break;
  case 't':
    arguments->maxTime = atoi(arg) * ONE_S_IN_US;
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

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

void printResults(struct runtime_t **runtimes, int runtimeCount,
                  struct table_style_t *style) {
  int maxPathLength = 4; // "PATH" in header
  int maxTimeLength = 1; // "TIME" in header, surely?
  int maxRunsLength = 3; // "RUNS" in header

  // Iterate every runtime and find the longest path + runs + runtime
  for (int i = 0; i < runtimeCount; i++) {
    runtime_t *runtime = runtimes[i];
    if (strlen(runtime->path) > maxPathLength) {
      maxPathLength = strlen(runtime->path);
    }
    if (log10(runtime->runs) > maxRunsLength) {
      maxRunsLength = log10(runtime->runs);
    }
    char *formattedTime = formatTime(runtime->total);
    int length = strlen(formattedTime) + 1;
    if (length > maxTimeLength) {
      maxTimeLength = length;
    }
  }

  // wtf is this
  maxTimeLength += 1;
  // table width
  int hline_length = maxPathLength + maxRunsLength + (4 * maxTimeLength) + 16;

  // Print the header line
  fprintf(stdout, "%-*s %s ", maxPathLength, "PATH", style->vdash);
  fprintf(stdout, "%-*s %s ", maxRunsLength + 1, "RUNS", style->vdash);
  fprintf(stdout, "%-*s %s ", maxTimeLength, "TOTAL", style->vdash);
  fprintf(stdout, "%-*s %s ", maxTimeLength, "AVG", style->vdash);
  fprintf(stdout, "%-*s %s ", maxTimeLength, "MAX", style->vdash);
  fprintf(stdout, "%-*s\n", maxTimeLength, "MIN");

  // column end indexes
  int ref[5];
  ref[0] = maxPathLength + 1;
  ref[1] = ref[0] + maxRunsLength + 4;
  for (int i = 2; i < 5; i++) {
    ref[i] = ref[i - 1] + maxTimeLength + 3;
  }

  // Line between header and results
  if (style->hline) {
    for (int i = 0; i < hline_length; i++) {
      for (int j = 0; j < 5; j++) {
        if (ref[j] == i) {
          fprintf(stdout, "%s", style->hvdash2);
          i++;
        }
      }
      fprintf(stdout, "%s", style->hdash2);
    }
    fprintf(stdout, "\n");
  }

  // print results
  for (int run = 0; run < runtimeCount; run++) {
    runtime_t *runtime = runtimes[run];
    // skip horizontal line on first loop
    if (run != 0 && style->hline) {
      for (int i = 0; i < hline_length; i++) {
        for (int j = 0; j < 5; j++) {
          if (ref[j] == i) {
            fprintf(stdout, "%s", style->hvdash1);
            i++;
          }
        }
        fprintf(stdout, "%s", style->hdash1);
      }
      fputc('\n', stdout);
    }
    fprintf(stdout, "%*s %s ", maxPathLength, runtime->path, style->vdash);
    fprintf(stdout, "%*ld %s ", maxRunsLength + 1, runtime->runs, style->vdash);
    fprintf(stdout, "%*s %s ", maxTimeLength, formatTime(runtime->total),
            style->vdash);
    fprintf(stdout, "%*s %s ", maxTimeLength, formatTime(runtime->avg),
            style->vdash);
    fprintf(stdout, "%*s %s ", maxTimeLength, formatTime(runtime->max),
            style->vdash);
    fprintf(stdout, "%*s\n", maxTimeLength, formatTime(runtime->min));
  }
}

/* Take the path to an executable and return the average runtime of max
 * iterationTimes times
 */
runtime_t *timeFileExecution(struct arguments *arguments, char *path) {
  runtime_t *runtime = malloc(sizeof(runtime_t));
  memset(runtime, 0, sizeof(runtime_t));
  runtime->times = calloc(arguments->runs, sizeof(long));
  runtime->min = LONG_MAX;
  runtime->path = path;

  int null_fd = open("/dev/null", O_WRONLY);
  for (int i = 0; i < arguments->runs; i++) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t pid = fork();
    if (pid == 0) {
      // Child process
      dup2(null_fd, STDOUT_FILENO);
      dup2(null_fd, STDERR_FILENO);
      struct rlimit rl;
      rl.rlim_cur = arguments->maxTime / ONE_S_IN_US;
      rl.rlim_max = arguments->maxTime / ONE_S_IN_US;
      setrlimit(RLIMIT_CPU, &rl);
      execl(path, path, NULL);
      _exit(EXIT_FAILURE); // die if it gets here, above line replaces program
                           // image
    } else if (pid > 0) {
      // Parent process
      int status;
      waitpid(pid, &status, 0);
      gettimeofday(&end, NULL);

      if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
        // exit OK
      } else if (WIFSIGNALED(status) && (WTERMSIG(status) == SIGXCPU)) {
        // TODO never gets in here
        printf("%s killed, out of time\n", path);
        break;
      } else {
        printf("%s exited abnormally with status %d (%d)\n", path,
               WEXITSTATUS(status), status);
        break;
      }
      long s = end.tv_sec - start.tv_sec;
      long us = end.tv_usec - start.tv_usec;
      long total = (s * 1000000) + us;

      runtime->total += total;
      runtime->times[runtime->runs++] = total;
      if (total > runtime->max) {
        runtime->max = total;
      }
      if (total < runtime->min) {
        runtime->min = total;
      }

      if (runtime->total > arguments->maxTime) {
        printf("%s stopping, out of time\n", path);
        break;
      }
    } else {
      // Error
      perror("fork() failed");
      exit(EXIT_FAILURE);
    }
  }
  close(null_fd);

  if (runtime->runs > 0) {
    runtime->avg = runtime->total / runtime->runs;
  } else {
    runtime->min = 0;
  }
  return runtime;
}

int main(int argc, char **argv) {
  struct arguments arguments;
  char year[5];
  time_t rawtime;
  struct tm *timeInfo;

  time(&rawtime);
  timeInfo = localtime(&rawtime);

  strftime(year, sizeof(year), "%Y", timeInfo);
  /* Default values */
  arguments.year = year;
  arguments.day = NULL;
  arguments.style = &style_utf16;
  arguments.path = "./";
  arguments.runs = 1024;
  arguments.maxTime = 30 * ONE_S_IN_US;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  printf(" -- Advent of Code Benchmark --\n");

  printf("YEAR = %s\n", arguments.year);

  if (arguments.day != NULL) {
    printf("DAY  = %s\n", arguments.day);
  } else {
    printf("DAY  = 1-25\n");
  }

  // please don't make your paths this long...
  char path[8192] = {0};
  strcat(path, arguments.path);
  strcat(path, arguments.year);
  strcat(path, "/");
  if (arguments.day != NULL) {
    strcat(path, arguments.day);
  } else {
    strcat(path, "*");
  }
  strcat(path, "/main.out");

  glob_t files;
  glob(path, 0, NULL, &files);
  u_int64_t fileCount = files.gl_pathc;

  printf("Path: %s with %lu files\n", path, fileCount);

  if (fileCount == 0) {
    printf("No solutions found. Go solve some!\n");
    exit(0);
  }
  runtime_t **runtimes = malloc(fileCount * sizeof(runtime_t *));
  for (int i = 0; i < fileCount; i++) {
    // for (int i = 0; i < 2; i++) {
    char *filePath = files.gl_pathv[i];
    printf("Testing: %s\n", filePath);

    runtime_t *runtime = timeFileExecution(&arguments, filePath);
    runtimes[i] = runtime;
  }

  printResults(runtimes, fileCount, arguments.style);
  globfree(&files);
  exit(0);
}
