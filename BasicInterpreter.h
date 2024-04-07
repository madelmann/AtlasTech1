/* A tiny BASIC interpreter */

#ifndef _BASICINTERPRETER_H
#define _BASICINTERPRETER_H

// Library includes
#include "stdio.h"
#include "setjmp.h"
#include "math.h"
#include "ctype.h"
#include "stdlib.h"

// Project includes


#define NUM_LAB 100
#define LAB_LEN 10 
#define FOR_NEST 25
#define SUB_NEST 25
#define PROG_SIZE 10000

#define DELIMITER  1
#define VARIABLE  2
#define NUMBER    3
#define COMMAND   4
#define STRING	  5
#define QUOTE	  6

#define PRINT 1
#define INPUT 2
#define IF    3
#define THEN  4
#define FOR   5
#define NEXT  6
#define TO    7
#define GOTO  8
#define EOL   9
#define FINISHED  10
#define GOSUB 11
#define RETURN 12
#define REM	13
#define	FUNCTION 14
#define SUB 15
#define CALL 16
#define LOCATE 17
#define SYSTEM 18
#define SYSTEMBEGIN 19
#define SYSTEMEND 20
#define SLEEP 21
#define END 22


struct commands			/* keyword lookup table */
{
  char command[20];
  char tok;
};

struct label
{
  char name[LAB_LEN];
  char *p;				/* points to place to go in source file*/
};


struct for_stack		/* stack for FOR/NEXT loop */
{
  int var;				/* counter variable */
  int target;			/* target value */
  char *loc;			/* points to place to go in source file*/
};



class CBasicInterpreter
{
public:
	CBasicInterpreter();
	~CBasicInterpreter();
	
	bool LoadProgram(char *p, char *fname);
	void Reset();
	long Prepare(char *fname);

private:
	void assignment();
	void arith(char o, int *r, int *h);
	void call();
	void exec_for();
	void exec_goto();
	void exec_if();
	void find_eol();
	char* find_label(char *s);
	int find_var(char *s);
	void fpush(struct for_stack i);
	void get_exp(int *result);
	int get_next_label(char *s);
	char get_token();
	void gosub();
	void gpush(char *s);
	void greturn();
	void input();
	bool isdelim(char c);
	bool iswhite(char c);
	void label_init();
	void level2(int *result);
	void level3(int *result);
	void level4(int *result);
	void level5(int *result);
	void level6(int *result);
	void locate();
	int look_up(char *s);
	void next();
	void primitive(int *result);
	void print();
	void putback();
	void rem();
	void scan_labels();
	void serror(int error);
	void SleepCMD();
	void System();
	void SystemBegin();
	void SystemEnd();
	void unary(char o, int *r);

	commands table[23];

	char *prog;  /* holds expression to be analyzed */
	jmp_buf e_buf; /* hold environment for longjmp() */

	int variables[26];

	char token[80];
	char token_type, tok;

	label label_table[NUM_LAB];

	char *find_label();
	char *gpop();

	for_stack fstack[FOR_NEST];

	for_stack fpop();
	char *gstack[SUB_NEST];	/* stack for gosub */

	int ftos;  /* index to top of FOR stack */
	int gtos;  /* index to top of GOSUB stack */

	bool bSystem;

	int locatex;
	int locatey;

	//char* Filename;
};

unsigned long ThreadEntry(void* Data);

#endif