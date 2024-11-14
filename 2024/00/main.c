#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#ifdef _WIN32
#include <windows.h>
#ifndef NO_EXPORT
#define EXPORT
#endif
#endif

typedef struct test_t test_t;
struct test_t
{
  int foo;
  char *bar;
};

void print_struct(test_t *s)                    { printf("bar: %s\nfoo: %d\n", s->bar, s->foo); }

void

do_nothign(bool fart){printf("bar: \n"  ); }

test_t * make_struct()
{
  test_t *foo = malloc(sizeof *foo);

  // TODO replace some aoc solutions with this method of initialisation! long line comment for testing
  *foo = (test_t){.bar = "hello", .foo = 33};

  return foo;
}

int main() {
  fprintf(stderr, "This is stderr\n");
  printf("This is stdout\n");

  test_t* foo = make_struct();
  print_struct(foo);
  sleep(5);
  //while(1);
  do
  {
//
  }
  while(0);

  int thisVariable1=0,thatVariable1=0,thisVariable2=0,thatVariable2=0,thisVariable3=0,thatVariable3=0;
if (thisVariable1 == thatVariable1 || thisVariable2 == thatVariable2 || thisVariable3 == thatVariable3)
    int eee=3;


switch (foo->foo)
{
case 1:
break;

case 2:
{
  break;
}
}
  int a=111, b =1, c =3, d=4;
if (!foo->foo==( 2 + (    3 * 10   )  ))
    int e=((b-c)*a,d--);

  int blah[3][4] = {0};
  blah[   2][ 3] = 2;
  return 0;
}
void Foo(bool isFoo) {
    if (isFoo) {
    int i = 1;
    }
  else {
    int i = 1;
    }
}
