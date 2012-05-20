#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDENT               ( "        " )

static int g_indent_level = 0;

static
void print_indent() {
  int i;
  for (i = 0; i <= g_indent_level; i++) printf(INDENT);
}

static
void print_code(const char *code) {
  print_indent();
  puts(code);
}

static
void emit_ptr_arith(int n, int op) {
  char prefix = '*';
  print_indent();
  if (op >= '<') {
    prefix = ' ';
    op = '-' - (op - '<');
    printf("CHK");
  }
  if (n == 1)  printf("(%c%c(%cptr));", op, op, prefix);
  else         printf("((%cptr)%c=%d);", prefix, op, n);
  putchar('\n');
}

static
int get_index_of(const char *s, int c) {
  char *p = strchr(s, (char)c);
  return p ? (int)(p - s) : -1;
}

static
void emit_code(void) {
  int c, n, op, id, commented = 0;
  c = getchar();
  while (c != EOF) {
    id = get_index_of("[]+-<>.,", c);
    if (commented && id != -1) {
      printf(" */\n");
      commented = 0;
    }
    if (id == 0) { /* '[' */
      print_code("for(;*ptr;){");
      ++g_indent_level;
      c = getchar();
    }
    else if (id == 1) { /* ']' */
      if (--g_indent_level < 0) {
        fprintf(stderr, "Error: Extra ']' found.\n");
        return;
      }
      print_code("}");
      c = getchar();
    }
    else if (2 <= id && id <= 5) { /* '+', '-', '<', '>' */
      op = c;
      n = 0;
      for (;c == op; n++) c = getchar();
      emit_ptr_arith(n, op);
    }
    else if (id == 6) { /* '.' */
      print_code("putchar(*ptr);");
      c = getchar();
    }
    else if (id == 7) { /* ',' */
      if (commented) printf(" */\n"), commented = 0;
      print_code("*ptr=getchar();");
      c = getchar();
    }
    else { /* others */
      if (c != '\r' && c != '\n') {
        if (!commented) {
          printf("/* ");
          commented = 1;
        }
        putchar(c);
      }
      c = getchar();
    }
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

  if (argc >= 2) {
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
  puts("/* -- translated code -- */");
  emit_code();
  while (g_indent_level > 0) {
    print_code("break;");
    --g_indent_level;
    print_code("}");
    fprintf(stderr, "Warning: Incorrect Nesting.\n");
  }
  puts("/* -- end -- */");
  puts("putchar('\\n');");
  puts("free(ptr_start);");
  puts("return 0;");
  puts("}");
  return 0;
}
