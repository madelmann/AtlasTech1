#include "Main.h"

/* Assign a variable a value. */
void CBasicInterpreter::assignment()
{
	int var, value;

	/* get the variable name */
	get_token();
	if(!isalpha((unsigned char)*token))
	{
		serror(4);
		return;
	}

	var = toupper(*token) - 'A';

	/* get the equals sign */
	get_token();
	if(*token != '=')
	{
		serror(3);
		return;
	}

	/* get the value to assign to var */
	get_exp(&value);

	/* assign the value */
	variables[var] = value;
}

void CBasicInterpreter::call()
{
	char *loc;

	get_token();
	/* find the label to call */
	loc = find_label(token);
	if(loc == '\0')
		serror(7); /* label not defined */
	else
	{
		gpush(prog); /* save place to return to */
		prog = loc;  /* start program running at that loc */
	}
}

/* Execute a simple version of the BASIC PRINT statement */
void CBasicInterpreter::print()
{
	int answer;
	int len = 0, spaces;
	char last_delim = 0;
	char output[255] = "";

	do
	{
		get_token();						/* get next list item */
		if(tok == EOL || tok == FINISHED)
			break;
		
		if(token_type == QUOTE)				/* is string */
		{
			sprintf(output, "%s", token);
			len += strlen(token);
			get_token();
		}
		else								/* is expression */
		{
			putback();
			get_exp(&answer);
			get_token();
			len += sprintf(output, "%s%d", output, answer);
		}
		last_delim = *token; 

		if(*token == ';')					/* compute number of spaces to move to next tab */
		{
			spaces = 8 - (len % 8); 
			len += spaces;					/* add in the tabbing position */
			while(spaces)
			{ 
				sprintf(output, "%s ", output);
				spaces--;
			}
		}
		else if(*token == ',')				/* do nothing */;
		else if(tok != EOL && tok != FINISHED) serror(0); 
	} while (*token == ';' || *token == ',');

	if(tok == EOL || tok == FINISHED)
	{
		if(last_delim != ';' && last_delim != ',')
			sprintf(output, "%s\n", output);
	}
	else
		serror(0);							/* error is not , or ; */

	if(Console->getOutputMode() == 1)
		Console->OutputGraphic(output, locatex, locatey);
	else
		Console->Output(output);

	locatex = -1;
	locatey = -1;
}

/* BASIC comment */
void CBasicInterpreter::rem()
{
	find_eol();
}

/* Find all labels. */
void CBasicInterpreter::scan_labels()
{
	int addr;
	char *temp;

	label_init();  /* zero all labels */
	temp = prog;   /* save pointer to top of program */

	/* if the first token in the file is a label */
	get_token();
	if(token_type == NUMBER)
	{
		strcpy(label_table[0].name, token);
		label_table[0].p = prog;
	}

	find_eol();

	do
	{     
		get_token();
		if(token_type == NUMBER)
		{
			addr = get_next_label(token);
			if(addr == -1 || addr == -2)
			{
				(addr == -1) ?serror(5):serror(6);
			}
			strcpy(label_table[addr].name, token);
			label_table[addr].p = prog;  /* current point in program */
		}
		else if(token_type == SUB)
		{
			addr = get_next_label(token);
			if(addr == -1 || addr == -2)
			{
				(addr == -1) ?serror(5):serror(6);
			}

			// get SUB name
			char buffer[255] = "";
			prog++;

			while(!isdelim(*prog))
			{
				sprintf(buffer, "%s%c", buffer, *prog);
				prog++;
			}

			strcpy(label_table[addr].name, buffer);
			label_table[addr].p = prog;  /* current point in program */
		}
		/* if not on a blank line, find next line */
		if(tok != EOL)
			find_eol();
	} while(tok != FINISHED);

	prog = temp;  /* restore to original */
}

void CBasicInterpreter::SleepCMD()
{
	//char *loc;

	get_token();
	/* find the label to call */
	
	DWORD ms;
	sscanf(token, "%d", &ms);

	Sleep(ms);
}

/* Execute a DaVinci console command */
void CBasicInterpreter::System()
{
	int answer;
	int len = 0, spaces;
	char last_delim = 0;
	char output[255] = "";

	do
	{
		if(!bSystem)
			get_token();						/* get next list item */
		if(tok == EOL || tok == FINISHED)
			break;
		
		if(token_type == QUOTE || bSystem)				/* is string */
		{
			sprintf(output, "%s", token);
			len += strlen(token);
			
			if(strcmp(output, "SYSTEMEND") == 0)
				SystemEnd();

			get_token();
		}
		else								/* is expression */
		{
			putback();
			get_exp(&answer);
			get_token();
			len += sprintf(output, "%s%d", output, answer);
		}

		if(!bSystem)
		{
			last_delim = *token; 

			if(*token == ';')					/* compute number of spaces to move to next tab */
			{
				spaces = 8 - (len % 8); 
				len += spaces;					/* add in the tabbing position */
				while(spaces)
				{ 
					sprintf(output, "%s ", output);
					spaces--;
				}
			}
			else if(*token == ',')				/* do nothing */;
			else if(tok != EOL && tok != FINISHED) serror(0);
		}
	} while (*token == ';' || *token == ',');

	if(tok == EOL || tok == FINISHED)
	{
		if(last_delim != ';' && last_delim != ',')
			sprintf(output, "%s\n", output);
	}
	else
		serror(0);							/* error is not , or ; */

	//if(strcmp(output, "SYSTEMEND") != 0)
		ProcessCommand(output);

	if(Console->getOutputMode() == 1)
		Console->OutputGraphic(output, locatex, locatey);
	//else
	//	Console->Output(output);
}

/* Start executing commands in Atlas */
void CBasicInterpreter::SystemBegin()
{
	bSystem = true;
}

/* Stop executing commands in Atlas */
void CBasicInterpreter::SystemEnd()
{
	bSystem = false;
}

/* Find the start of the next line. */
void CBasicInterpreter::find_eol()
{
	while(*prog!='\n'  && *prog!='\0')
		++prog;

	if(*prog)
		prog++;
}

/* Return index of next free position in label array. 
   A -1 is returned if the array is full.
   A -2 is returned when duplicate label is found.
*/
int CBasicInterpreter::get_next_label(char *s)
{
	register int t;

	for(t = 0; t < NUM_LAB; ++t)
	{
		if(label_table[t].name[0] == 0)
			return t;
		if(!strcmp(label_table[t].name, s))
			return -2; /* dup */
	}

	return -1;
}

/* Find location of given label.  A null is returned if
   label is not found; otherwise a pointer to the position
   of the label is returned.
*/
char* CBasicInterpreter::find_label(char *s)
{
	register int t;

	for(t=0; t<NUM_LAB; ++t) 
		if(!strcmp(label_table[t].name, s))
			return label_table[t].p;

	return '\0'; /* error condition */
}

/* Execute a GOTO statement. */
void CBasicInterpreter::exec_goto()
{
	char *loc;

	get_token(); /* get label to go to */
	/* find the location of the label */
	loc = find_label(token);
	if(loc == '\0')
		serror(7); /* label not defined */
	else
		prog = loc;  /* start program running at that loc */
}

/* Initialize the array that holds the labels. 
   By convention, a null label name indicates that
   array position is unused.
*/
void CBasicInterpreter::label_init()
{
	register int t;

	for(t = 0; t < NUM_LAB; ++t)
		label_table[t].name[0] = '\0';
}

/* Execute an IF statement. */
void CBasicInterpreter::exec_if()
{
	int x , y, cond;
	char op;

	get_exp(&x); /* get left expression */

	get_token(); /* get the operator */
	if(!strchr("=<>", *token))
	{
		serror(0); /* not a legal operator */
		return;
	}
	
	op =* token;

	get_exp(&y); /* get right expression */

	/* determine the outcome */
	cond = 0;
	switch(op)
	{
	case '<':
		if(x < y)
			cond = 1;
		break;
	case '>':
		if(x > y)
			cond = 1;
		break;
	case '=':
		if(x == y)
			cond = 1;
		break;
	}
	if(cond)
	{ /* is true so process target of IF */
		get_token();
		if(tok != THEN)
		{
			serror(8);
			return;
		}/* else program execution starts on next line */
	}
	else
		find_eol(); /* find start of next line */
}

/* Execute a FOR loop. */
void CBasicInterpreter::exec_for()
{
	struct for_stack i;
	int value;

	get_token(); /* read the control variable */
	if(!isalpha((unsigned char)*token))
	{
		serror(4);
		return;
	}

	i.var = toupper(*token)-'A'; /* save its index */

	get_token(); /* read the equals sign */
	if(*token != '=')
	{
		serror(3);
		return;
	}

	get_exp(&value); /* get initial value */

	variables[i.var] = value;

	get_token();
	if(tok != TO)
		serror(9); /* read and discard the TO */

	get_exp(&i.target); /* get target value */

	/* if loop can execute at least once, push info on stack */
	if(value >= variables[i.var])
	{ 
		i.loc = prog;
		fpush(i);
	}
	else  /* otherwise, skip loop code altogether */
		while(tok != NEXT) get_token();
}

/* Execute a NEXT statement. */
void CBasicInterpreter::next()
{
	struct for_stack i;

	i = fpop(); /* read the loop info */

	variables[i.var]++; /* increment control variable */
	if(variables[i.var] > i.target)
		return;  /* all done */
	fpush(i);  /* otherwise, restore the info */
	prog = i.loc;  /* loop */
}

/* Push function for the FOR stack. */
void CBasicInterpreter::fpush(struct for_stack i)
{
	if(ftos > FOR_NEST)
		serror(10);

	fstack[ftos] = i;
	ftos++;
}

struct for_stack CBasicInterpreter::fpop()
{
	ftos--;
	if(ftos < 0)
		serror(11);

	return (fstack[ftos]);
}

/* Execute a simple form of the BASIC INPUT command */
void CBasicInterpreter::input()
{
	char /*str[80] = "", */var;
	int i;

	get_token(); /* see if prompt string is present */
	if(token_type == QUOTE)
	{
		printf(token); /* if so, print it and check for comma */
		get_token();
		if(*token != ',')
			serror(1);
		get_token();
	}
	else
		printf("? "); /* otherwise, prompt with / */

	var = char(toupper(*token) - 'A'); /* get the input var */

	scanf("%d", &i); /* read input */

	variables[var] = i; /* store it */
}

/* Execute a GOSUB command. */
void CBasicInterpreter::gosub()
{
	char *loc;

	get_token();
	/* find the label to call */
	loc = find_label(token);
	if(loc == '\0')
		serror(7); /* label not defined */
	else
	{
		gpush(prog); /* save place to return to */
		prog = loc;  /* start program running at that loc */
	}
}

/* Return from GOSUB. */
void CBasicInterpreter::greturn()
{
	prog = gpop();
}

/* GOSUB stack push function. */
void CBasicInterpreter::gpush(char *s)
{
	gtos++;

	if(gtos == SUB_NEST)
	{
		serror(12);
		return;
	}

	gstack[gtos] = s;
}

/* GOSUB stack pop function. */
char* CBasicInterpreter::gpop()
{
	if(gtos == 0)
	{
		serror(13);
		return 0;
	}

	return (gstack[gtos--]);
}

/* Entry point into parser. */
void CBasicInterpreter::get_exp(int *result)
{
	get_token();
	if(!*token)
	{
		serror(2);
		return;
	}
	level2(result);
	putback(); /* return last token read to input stream */
}


/* display an error message */
void CBasicInterpreter::serror(int error)
{
	static char *e[] = {   
		"syntax error", 
		"unbalanced parentheses", 
		"no expression present",
		"equals sign expected",
		"not a variable",
		"Label table full",
		"duplicate label",
		"undefined label",
		"THEN expected",
		"TO expected",
		"too many nested FOR loops",
		"NEXT without FOR",
		"too many nested GOSUBs",
		"RETURN without GOSUB"
	}; 
	
	char buffer[255] = "";
	sprintf(buffer, "ERROR[BASIC]: %s\n", e[error]); 
	Console->Output(buffer);
	longjmp(e_buf, 1); /* return to save point */
}

/* Get a token. */
char CBasicInterpreter::get_token()
{
	register char *temp;

	token_type = 0; tok = 0;
	temp = token;

	if(*prog == '\0')
	{ /* end of file */
		*token = 0;
		tok = FINISHED;
		return (token_type = DELIMITER);
	}

	while(iswhite(*prog))
		++prog;  /* skip over white space */

	if(*prog == '\r')
	{ /* crlf */
		++prog; ++prog;
		tok = EOL; *token = '\r';
		token[1] = '\n'; token[2] = 0;
		return (token_type = DELIMITER);
	}

	if(strchr("+-*^/%=;(),><", *prog))
	{ /* delimiter */
		*temp = *prog;
		prog++; /* advance to next position */
		temp++;
		*temp = 0; 
		return (token_type = DELIMITER);
	}
    
	if(*prog == '"' || bSystem)
	{ /* quoted string */
		if(*prog == '"')
			prog++;
		while(*prog!='"' && *prog!='\r')
			*temp++ = *prog++;
		if(*prog == '\r' && !bSystem)
			serror(1);
		if(*prog == '"')
			prog++;
		*temp = 0;
		return (token_type = QUOTE);
	}
  
	if(isdigit((unsigned char)*prog))
	{ /* number */
		while(!isdelim(*prog))
			*temp++ = *prog++;
		*temp = '\0';
		return (token_type = NUMBER);
	}

	/* var or command */
	if(isalpha((unsigned char)*prog))
	{
		char buffer[255] = "";

		while(!isdelim(*prog))
		{
			sprintf(buffer, "%s%c", buffer, *prog);
			*temp++ = *prog++;
		}
		sprintf(buffer, "%s\0", buffer);

		if(strcmp(buffer, "SUB") == 0)
			token_type = SUB;
		else
			token_type = STRING;
	}

	*temp = '\0';

	/* see if a string is a command or a variable */
	if(token_type == STRING)
	{
		tok = char(look_up(token)); /* convert to internal rep */
		if(!tok)
			token_type = VARIABLE;
		else
			token_type = COMMAND; /* is a command */
	}

	return token_type;
}


/* Return a token to input stream. */
void CBasicInterpreter::putback() 
{
	char *t; 

	t = token; 
	for(; *t; t++)
		prog--; 
}

/* Look up a a token's internal representation in the
   token table.
*/
int CBasicInterpreter::look_up(char *s)
{
	register int i/*, j*/ = 0;
	char *p;

	/* convert to lowercase */
	p = s;
	while(*p)
	{
		*p = char(tolower(*p));
		p++;
	}

	/* see if token is in table */
	for(i = 0; *table[i].command; i++)
	{
		if(!strcmp(table[i].command, s))
			return table[i].tok;
	}

	return 0; /* unknown command */
}

/* Return true if c is a delimiter. */
bool CBasicInterpreter::isdelim(char c)
{
	if(strchr(" ;,+-<>/*%^=()", c) || c == 9 || c == '\r' || c == 0) 
		return 1;

	return 0;
}

/* Return 1 if c is space or tab. */
bool CBasicInterpreter::iswhite(char c)
{
	if(c == ' ' || c == '\t')
		return 1;
	else
		return 0;
}



/*  Add or subtract two terms. */
void CBasicInterpreter::level2(int *result)
{
	register char  op; 
	int hold; 

	level3(result); 
	while((op = *token) == '+' || op == '-')
	{
		get_token(); 
		level3(&hold); 
		arith(op, result, &hold);
	}
}

/* Multiply or divide two factors. */
void CBasicInterpreter::level3(int *result)
{
	register char  op; 
	int hold;

	level4(result); 
	while((op = *token) == '*' || op == '/' || op == '%')
	{
		get_token(); 
		level4(&hold); 
		arith(op, result, &hold);
	}
}

/* Process integer exponent. */
void CBasicInterpreter::level4(int *result)
{
	int hold; 

	level5(result); 
	if(*token == '^')
	{
		get_token(); 
		level4(&hold); 
		arith('^', result, &hold); 
	}
}

/* Is a unary + or -. */
void CBasicInterpreter::level5(int *result)
{
	register char  op; 

	op = 0; 
	if((token_type == DELIMITER) && *token == '+' || *token == '-')
	{
		op = *token; 
		get_token(); 
	}
	level6(result); 
	if(op)
		unary(op, result); 
}

/* Process parenthesized expression. */
void CBasicInterpreter::level6(int *result)
{
	if((*token == '(') && (token_type == DELIMITER))
	{
		get_token(); 
		level2(result); 
		if(*token != ')')
			serror(1);
		get_token(); 
	}
	else
		primitive(result);
}

void CBasicInterpreter::locate()
{
	char last_delim;
	//char output[255] = "";

	while(iswhite(*prog))
		prog++;

	get_token();						/* get next list item */
	if(tok == EOL || tok == FINISHED)
		return;

	putback();							/* is expression */
	get_exp(&locatex);
	get_token();
	
	last_delim = *token;


	while(isdelim(*prog))
		prog++;

	while(iswhite(*prog))
		prog++;

	get_token();						/* get next list item */
	if(tok == EOL || tok == FINISHED)
		return;

	putback();
	get_exp(&locatey);
	get_token();
	
	last_delim = *token; 

	//find_eol();
}

/* Find value of number or variable. */
void CBasicInterpreter::primitive(int *result)
{
	switch(token_type)
	{
	case VARIABLE:
		*result = find_var(token);
		get_token(); 
		return; 
	case NUMBER:
		*result = atoi(token);
		get_token();
		return;
	default:
		serror(0);
	}
}

/* Perform the specified arithmetic. */
void CBasicInterpreter::arith(char o, int *r, int *h)
{
  register int t, ex;

  switch(o)
  {
    case '-':
      *r = *r-*h; 
      break; 
    case '+':
      *r = *r+*h; 
      break; 
    case '*':  
      *r = *r * *h; 
      break; 
    case '/':
      *r = (*r) / (*h);
      break; 
    case '%':
      t = (*r) / (*h); 
      *r = *r - (t*(*h)); 
      break; 
    case '^':
      ex = *r; 
      if(*h == 0)
	  {
        *r = 1; 
        break; 
      }
      for(t = *h-1; t>0; --t)
		  *r = (*r) * ex;
      break;       
  }
}

/* Reverse the sign. */
void CBasicInterpreter::unary(char o, int *r)
{
  if(o == '-')
	  *r = -(*r);
}

/* Find the value of a variable. */
int CBasicInterpreter::find_var(char *s)
{
	if(!isalpha((unsigned char)*s))
	{
		serror(4); /* not a variable */
		return 0;
	}

	return variables[toupper(*token) - 'A'];
}


CBasicInterpreter::CBasicInterpreter()
{
	for(int i = 0; i < 26; i += 1)
		variables[i] = 0;

	/* Commands must be entered lowercase in this table.*/
	// PRINT
	strcpy(table[0].command, "print");
	table[0].tok = PRINT;
	// INPUT
	strcpy(table[1].command, "input");
	table[1].tok = INPUT;
	// IF
	strcpy(table[2].command, "if");
	table[2].tok = IF;
	// THEN
	strcpy(table[3].command, "then");
	table[3].tok = THEN;
	// GOTO
	strcpy(table[4].command, "goto");
	table[4].tok = GOTO;
	// FOR
	strcpy(table[5].command, "for");
	table[5].tok = FOR;
	// NEXT
	strcpy(table[6].command, "next");
	table[6].tok = NEXT;
	// TO
	strcpy(table[7].command, "to");
	table[7].tok = TO;
	// GOSUB
	strcpy(table[8].command, "gosub");
	table[8].tok = GOSUB;
	// RETURN
	strcpy(table[9].command, "return");
	table[9].tok = RETURN;
	// REM
	strcpy(table[10].command, "rem");
	table[10].tok = REM;
	// FUNCTION
	strcpy(table[11].command, "function");
	table[11].tok = FUNCTION;
	// CALL
	strcpy(table[12].command, "call");
	table[12].tok = CALL;
	// LOCATE
	strcpy(table[13].command, "locate");
	table[13].tok = LOCATE;
	// SYSTEM
	strcpy(table[14].command, "system");
	table[14].tok = SYSTEM;
	// SLEEP
	strcpy(table[15].command, "sleep");
	table[15].tok = SLEEP;
	// SYSTEMBEGIN
	strcpy(table[16].command, "systembegin");
	table[16].tok = SYSTEMBEGIN;
	// SYSTEMEND
	strcpy(table[17].command, "systemend");
	table[17].tok = SYSTEMEND;
	// END
	strcpy(table[18].command, "end");
	table[18].tok = END;
	// END
	strcpy(table[19].command, "");
	table[19].tok = END;

	// SUB
	//strcpy(table[20].command, "sub");
	//table[20].tok = SUB;

	bSystem = false;

	locatex = -1;
	locatey = -1;
}

CBasicInterpreter::~CBasicInterpreter()
{
	for(int i = 0; i < 26; i += 1)
		variables[i] = 0;
}

bool CBasicInterpreter::LoadProgram(char *p, char *fname)
{
	FILE *pFile;
	int i = 0;
	char Filename[255] = "";

	sprintf(Filename, "%sMaps\\%s\\SCRIPTS\\%s.BAS", g_sGameName, Game.sMapName, RemoveFileExt(fname));

	fopen_s(&pFile, Filename, "rb");

	// Make sure we have a valid file pointer (we found the file)
	if(!pFile) 
	{
		sprintf(Filename, "%sSCRIPTS\\%s.BAS", g_sGameName, RemoveFileExt(fname));
		fopen_s(&pFile, Filename, "rb");

		if(!pFile)
		{
			// Display an error message and don't load anything if no file was found
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Basic.Load]: Unable to load File '%s'!\n", Filename);
			Console->Output(buffer);
			return false;
		}
	}

	i = 0;
	do
	{
		*p = char(getc(pFile));
		p++;
		i++;
	} while(!feof(pFile) && i < PROG_SIZE);

	*(p-2) = '\0'; /* null terminate the program */

	fclose(pFile);

	return true;
}

void CBasicInterpreter::Reset()
{
	for(int i = 0; i < 26; i += 1)
		variables[i] = 0;

	bSystem = false;
	locatex = -1;
	locatey = -1;
}

long CBasicInterpreter::Prepare(char *fname)
{
	char *p_buf = "";

	Reset();

	/* allocate memory for the program */
	if(!(p_buf = (char *)malloc(PROG_SIZE)))
	{
		printf("allocation failure");
		return false;
	}

	if(!LoadProgram(p_buf, fname))	/* load the program to execute */
		return false;

	if(setjmp(e_buf))
		return false;				/* initialize the long jump buffer */

	prog = p_buf;
	scan_labels();					/* find the labels in the program */
	ftos = 0;						/* initialize the FOR stack index */
	gtos = 0;						/* initialize the GOSUB stack index */

	do
	{
		token_type = get_token();

		if(token_type == VARIABLE)	/* check for assignment statement */
		{
			putback();				/* return the var to the input stream */
			assignment();			/* must be assignment statement */
		}
		else						/* is command */
			switch(tok)
			{
			case PRINT:
				print();
				break;
			case GOTO:
				exec_goto();
				break;
			case IF:
				exec_if();
				break;
			case FOR:
				exec_for();
				break;
			case NEXT:
				next();
				break;
			case INPUT:
				//input();
				break;
			case GOSUB:
				gosub();
				break;
			case RETURN:
				greturn();
				break;
			case REM:
				rem();
				break;
			case FUNCTION:
				//
				break;
			case SUB:
				//
				break;
			case CALL:
				call();
				break;
			case LOCATE:
				locate();
				break;
			case SYSTEM:
				System();
				break;
			case SYSTEMBEGIN:
				SystemBegin();
				rem();
				break;
			case SYSTEMEND:
				//SystemEnd();
				//rem();
				break;
			case SLEEP:
				SleepCMD();
				break;
			case END:
				return 0;
			default:
				if(bSystem)
					System();
				break;
			}
	} while (tok != FINISHED);

	return 0;
}

unsigned long ThreadEntry(LPVOID Data)
{
	CBasicInterpreter* interpreter = new CBasicInterpreter;
	interpreter->Prepare((char*)Data);

	return (unsigned long)Data;
}