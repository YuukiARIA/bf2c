#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDENT               ( "\t" )

static int g_indent_level = 1;

static
void print_indent() {
  int i;
  for (i = 0; i < g_indent_level; i++) printf(INDENT);
}

static
void emit_ptr_arith(int n, char op) {
  char prefix = '*';
  if (op >= '<') {
    prefix = ' ';
    op = '-' - (op - '<');
    printf("CHK");
  }
  if (n == 1)  printf("(%c%c(%cptr));", op, op, prefix);
  else         printf("((%cptr)%c=%d);", prefix, op, n);
}

static
void emit_code(void) {
  int c, n;
  char op;
  while ((c = getchar()) != EOF) {
    switch (c) {
    case '[':
      print_indent();
      printf("for(;*ptr;){");
      ++g_indent_level;
      break;
    case ']':
      if (--g_indent_level < 1) g_indent_level = 1;
      print_indent();
      putchar('}');
      break;
    case '+':
    case '-':
    case '>':
    case '<':
      op = (char)c;
      n = 1;
      for (;(c = getchar()) == op; n++);
      ungetc(c, stdin);
      print_indent();
      emit_ptr_arith(n, op);
      break;
    case '.':
      print_indent();
      printf("putchar(*ptr);");
      break;
    case ',':
      print_indent();
      printf("*ptr=getchar();");
      break;
	default:
      continue;
    }
    putchar('\n');
  }
}

static
void make_out_filename(char *name, const char *in_filename) {
  char *p;
  strcpy(name, in_filename);
  if ((p = strrchr(name, '.')) != NULL) *p = '\0';
  strcat(name, ".c");
}

int main(int argc, char *argv[]) {
  char out_filename[256];

  if (argc >= 2)
  {
    make_out_filename(out_filename, argv[1]);
    freopen(argv[1], "r", stdin);
    freopen(out_filename, "w", stdout);
  }

  puts("#include <stdio.h>");
  puts("#include <stdlib.h>");
  puts("#include <string.h>\n");

  puts("typedef unsigned char byte;");
  puts("static byte *ptr_start, *ptr_end, *ptr;\n");

  puts("#define MEM_SIZE   ( 32768 )");
  puts("#ifdef NOCHECK");
  puts("#define CHK(p)     ( p )");
  puts("#else");
  puts("#define CHK(p)     ( chk(p) )");
  puts("static void chk(byte *p){");
  puts("  if(p<ptr_start||ptr_end<=p){");
  puts("    fprintf(stderr,\"Error: Memory Out Of Bounds.\\n\");");
  puts("    exit(1);");
  puts("  }");
  puts("}");
  puts("#endif\n");

  puts("int main(int argc, char *argv[]){");
  puts("  int i=1,mem_size=MEM_SIZE;");
  puts("  for (; i < argc;){");
  puts("    if (strcmp(argv[i], \"-m\") == 0){");
  puts("      i++;");
  puts("      if (i < argc) mem_size=atoi(argv[i++]);");
  puts("    } else i++;");
  puts("  }");
  puts("  if (mem_size<1) mem_size=1;");
  puts("  ptr_start=(byte *)calloc(mem_size, 1);");
  puts("  ptr_end=ptr_start+mem_size;");
  puts("  ptr=ptr_start;");
  puts("// -- translated code --");
  emit_code();
  puts("// -- end --");
  puts("putchar('\\n');");
  puts("free(ptr_start);");
  puts("return 0;");
  puts("}");

  if (g_indent_level != 1) fprintf(stderr, "Warning: Incorrect Nesting.\n");

  fprintf(stderr, "-- '%s' created.\n", out_filename);
  return 0;
}
