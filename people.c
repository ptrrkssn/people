/*
** people.c                  A "pretty-print" cover program to rwho
**
** Copyright (c) 1993-1997 Peter Eriksson <pen@lysator.liu.se>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2, or (at your option)
** any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef PATH_RWHO
#define PATH_RWHO "/usr/ucb/rwho"
#endif

#include <stdio.h>
#include <pwd.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else
#  include <strings.h>
#endif

#ifdef HAVE_LOCALE_H
#  include <locale.h>
#endif

#include "getopt.h"


int header_flag = 0;


extern char *getenv();
static char *ct = NULL;


struct option longopts[] =
{
    { "version", 0, 0, 'v' },
    { "help", 0, 0, 'h' },
    { "with-header", 0, 0, 'H' },
    { "all", 0, 0, 'a' },
    { 0, 0, 0, 0 }
};

extern char version[];

char usage[] = "Usage: %s [-avhH] [--help] [--version] [--all] [--with-header]\n";


unsigned char *fixname(n)
unsigned char *n;
{
    static unsigned char ub[1024];
    unsigned char *cp;

    
    if (ct && (strcmp(ct, "swedish") == 0 || strcmp(ct, "iso_8859_1") == 0))
    {
	cp = ub;
	while (*n)
	{
	    switch (*n)
	    {
	      case '}':  *cp = 'å'; break;
	      case '{':  *cp = 'ä'; break;
	      case '|':  *cp = 'ö'; break;
	      case ']':  *cp = 'Å'; break;
	      case '[':  *cp = 'Ä'; break;
	      case '\\': *cp = 'Ö'; break;
	      default:   *cp = *n; break;
	    }
	    ++n;
	    ++cp;
	}

	*cp = '\0';
	return ub;
    }
    else
	return n;
    
}


int people_main(argc,argv)
  int argc;
  char *argv[];
{
  FILE *fp, *popen();
  int optc;
  char buf[2048];
  char user[32];
  char machine[32];
  char tty[32];
  char rest[200];
  char old_user[32];
  char old_machine[32];
  char *rn, *p;
  struct passwd *pw;
  int i, flag;
  int aflag = 0;
  extern char *strchr();

  srand((unsigned int) time(NULL));
  
  if (getenv("IC_CTYPE") && !(rand()&3))
  {
      fprintf(stderr,
	  "Repeat after me 100 times: There is no such thing as IC_CTYPE.\n");
      exit(1);
  }
  
  ct = getenv("LC_CTYPE");  
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL, "");
#endif

  while ((optc = getopt_long (argc, argv, "avhH", longopts, (int *) 0)) != EOF)
  {
      switch (optc)
      {
	case 'v':
	  printf("%s\n", version);
	  exit(0);

	case 'h':
	  printf("This is %s, a pretty-printing frontend to Rwho.\n\n",
	  version);
	  printf(usage, argv[0]);
	  puts("  -h, --help\t\t\tPrint a summary of the options");
	  puts("  -v, --version\t\t\tPrint the version number");
	  puts("  -a, --all\t\t\tDisplay all users");
	  puts("  -H, --with-header\t\tDisplay a table header");
	  exit(0);

	case 'a':
	  aflag = 1;
	  break;

	case 'H':
	  header_flag = 1;
	  break;

	default:
	  fprintf(stderr, usage, argv[0]);
	  exit(1);
      }
  }
#if 0  
  if (optind < argc)
  {
      fprintf(stderr, usage, argv[0]);
      exit(1);
  }
#endif

  if (optind < argc)
      sprintf(buf, "%s%s %s", PATH_RWHO, aflag ? " -a" : "", argv[optind]);
  else	  
      sprintf(buf, "%s%s", PATH_RWHO, aflag ? " -a" : "");
	  
  
  fp = popen(buf, "r");  
  if (!fp)
  {
    perror("people: executing 'rwho'");
    exit(1);
  }

  if (header_flag)
  {
    printf("%-8s  %-30s  Machine(s)\n", "User", "Name");
    printf("--------- ------------------------------- -------------------------------\n");
  }
    
  old_user[0] = old_machine[0] = 0;
  while (fgets(buf, sizeof(buf)-1, fp))
  {
    sscanf(buf, "%s %[^:]:%s %[^\n]", user, machine, tty, rest);
    if (strcmp(user, old_user))
    {
      if (old_user[0])
	putchar('\n');
      printf("%-8s  ", user);
      pw = getpwnam(user);
      if (!pw)
	rn = "";
      else
	rn = pw->pw_gecos;
      if (rn == NULL)
	  rn = "{null}";
      
      p = strchr(rn, ',');
      if (p)
	*p = '\0';
      printf("%-30s  ", fixname(rn));
      old_machine[0] = 0;
    }
    
    if (strcmp(machine, old_machine))
    {
      if (old_machine[0])
	printf(", ");
      printf("%s", machine);
      strcpy(old_machine, machine);
    }

    strcpy(old_user, user);
  }

  putchar('\n');
  pclose(fp);

  return 0;
}



int fingerd_main(argc, argv)
    int argc;
    char *argv[];
{
    char buf[2048];
    char *cp;
    int len;
    int islong = 0;


    
    if (fgets(buf,sizeof(buf)-1, stdin) == NULL)
	exit(1);

    len = strlen(buf);
    for (cp = buf+len-1; cp >= buf && isspace(*cp); --cp)
	;
    *++cp = '\0';
    
    for (cp = buf; *cp && isspace(*cp); ++cp)
	;

    if (strncmp(cp, "/W ", 3) == 0)
    {
	islong = 1;
	cp += 3;
    }

    if (*cp)
    {
	if (islong)
	    execl("/usr/ucb/finger", "finger", "-l", cp, NULL);
	else
	    execl("/usr/ucb/finger", "finger", cp, NULL);
    }
    else
    {
	char *argv[3];

	argv[0] = "people";
	argv[1] = "--with-header";
	argv[2] = NULL;
	
	return people_main(2, argv);
    }
    
    exit(0);
}


int main(argc,argv)
  int argc;
  char *argv[];
{
    char *cp;

    if (argv[0])
    {
	cp = strrchr(argv[0], '/');
	if (cp)
	    ++cp;
	else
	    cp =argv[0];
    }

    if (cp &&
	(strcmp(cp, "fingerd") == 0 ||
	 strcmp(cp, "in.fingerd") == 0) ||
	 strcmp(cp, "pfingerd") == 0)
	return fingerd_main(argc,argv);
    else
	return people_main(argc,argv);
}
