//Project #3 Parser & Code Generator for Tiny PL/0
//Group 29


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"


#define MAX_SYMBOL_TABLE_SIZE 100
#define CODE_SIZE 400




typedef struct symbol {
	int kind; // const = 1, var = 2, proc = 3
	char name[12]; // name up to 11 chars
	int val; // number (ASCII value)
	int level; // L level
	int addr; // M address
} symbol;


// semantic value
// either string (char *) for the name of a constant/an identifier
// or int for the value of the number



//global var holding current token type.
token_type token;

// global variable holding the semantic value of the current token
LVAL lval;
//lval.id = malloc(sizeof(char) * 12); // ident up to 11 chars

//counts synbols in symbol_table
int symbol_count;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

char *rawCode;
int rawCodeIndex = 0;
int rawCodeSize = 0;

int cx;
code_struct code[CODE_SIZE];


// function advance that move to the next token and
// updates the above two global variables
void advance();


// function get_symbol that looks up a symbol in symbol table by name and
// returns pointer symbol if found and NULL if not found

symbol *get_symbol(char* name);


FILE *fin;
FILE *fout;

// function put_symbol that puts a symbol into symbol table provided that
// a symbol with this name does not exist (calls error function if name already exists)
void put_symbol(int kind, char* name, int num, int level, int modifier);

void emit(int op, int l, int m);
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
void error(int n);
char *createSourceNoComment(char *code, int fileLength);
int readNextToken(char *code, int i, int codeLength);
char* getNoCommentCode(char* filename);





int main(int argc, char **argv){

	int i;

	if (argc < 3) {
		printf("Please incude file in and out to read\n");
		exit(1);
	} else {
		rawCode = getNoCommentCode(argv[1]);
		fout = fopen(argv[2],"w");
	}

	while (rawCode[rawCodeSize] != '\0') {
		rawCodeSize++;
	}



	//initalize outcode to 0
	for(i = 0; i < CODE_SIZE; i++){
		code[i].op = 0;
		code[i].l = 0;
		code[i].m = 0;
	}



	//start our program call
	program();

	//if we get here, we parsed successfully!
	printf("No errors, program is syntactically correct\n");



	fclose(fin);
	fclose(fout);

	return 0;



}



symbol *get_symbol(char[] name){ 
	
	for( int i = 0; i < symbol_count; i++)// for loop control
	if( strcmp(symbol_table[i].name, name))// If there's a match
	return symbol_table[i];
	
	return NULL;

}




void put_symbol(int kind, char[] name, int num, int level, int modifier){

   int foundFlag =0, lastPos = 0;// foundFlag is if a member already exists, lastPos is for the position of the current 
				 //NULL member.

	for( int i = 0; i < symbol_count; i++)
	 if( strcmp(symbol_table[i].name, name))
	        foundFlag = 1;


	//add symbol or ERROR
	if(foundFlag == 0){
		symbol_table[symbol_count].kind = kind;
		strcpy(symbol_table[symbol_count].name, name);
		if(kind == 1)// if constant, save the number
        	  symbol_table[symbol_count].val = num;
    		else if (kind == 2){// if variable, save level and modifier.
		symbol_table[symbol_count].level = level
		symbol_table[symbol_count].addr = modifier;
		}
	}else
	error(27);

	
	symbol_count++;
}




void advance(){

	rawCodeIndex = readNextToken(rawCode,rawCodeIndex,rawCodeSize);


}

void program(){
	advance();
	block();
	if (token != periodsym) {
		error(9);
	}
}

void block(){

	char ident[12];
	int val;

	if (token == constsym) {
		do {
			advance();
			if (token != identsym) {
				error(4);
			}

			//TODO get symbol from token

			advance();
			if (token != eqsym) {
				error(3);
			}

			advance();
			if (token != numbersym) {
				error(2);
			}

			//TODO get val from token, update symbol table

			advance();

		} while(token != commasym);

		if (token != semicolonsym) {
			error(5);
		}

		advance();
	}

	if (token == varsym) {
		do {
			advance();
			if (token != identsym) {
				error(4);
			}

			//TODO get symbol from token
			//TODO create symbol entry in table

			advance();
		} while(token != commasym);

		if (token != semicolonsym) {
			error(5);
		}

		advance();
	}

	while (token = procsym) {
		advance();
		if (token != identsym) {
			error(4);
		}

		//TODO get symbol from token
		//TODO create symbol entry in table

		advance();
		block();
		if (token != semicolonsym) {
			error(17);
		}

		advance();

	}

	//TODO emit
	statement();

}


void statement(){
	if (token == identsym) {

		//TODO check if our identsym has been declared in the table




		advance();
		if (token != becomessym) {
			error(3);
		}

		advance();

		expression();

		//TODO emit

	} else if (token == callsym){

		advance();
		if (token != identsym) {
			error(14);
		}

		//TODO check if identsym has been declared



		advance();

	} else if (token == beginsym){
		advance();
		statement();

		while (token == semicolonsym) {
			advance();
			statement();
		}

		if (token != endsym) {
			error(30);
		}
		advance();

	} else if (token = ifsym) {
		advance();

		condition();

		if (token != thensym) {
			error(16);
		}

		advance();

		//TODO emit
		statement();

	} else if (token == whilesym){
		advance();
		condition();
		//TODO emit

		if (token != dosym) {
			error(18);
		}

		advance();
		statement();
		//TODO emit
	}
}


void condition(){

	if (token == oddsym) {
		advance();
		expression();
		//TODO emit;
	} else {
		expression();
		//TODO get relational op

		//if () {
		//	error(20);
		//}

		advance();
		expression();
		//TODO emit;

	}


}


void expression(){

	int oper;

	if (token == plussym || token == minussym) {
		oper = token;
		advance();
		term();
		//TODO emit if neg

		while (token == plussym || token == minussym) {
			oper = token;
			advance();
			term();
			//TODO emit if oper is add or sub
		}
	}

}


void term(){

	int oper;

	factor();

	while (token == multsym || token == slashsym) {
		oper = token;
		advance();

		factor();

		//TODO emit if oper is mult or div

	}

}


void factor(){
	if (token == identsym) {
		//TODO check if ident is already in symbol table
		//TODO get from mem or symbol table based on var or const
		//and emit based on above
		advance();
	} else if (token == numbersym) {
		//TODO emit
		advance();
	} else if (token == lparentsym) {
		advance();
		expression();

		if (token != rparentsym) {
			error(22);
		}

		advance();
	} else {
		error(23);
	}
}



void emit(int op, int l, int m) {
	if(cx > CODE_SIZE){
		error(26);
	} else {
		code[cx].op = op; // opcode
		code[cx].l = l; // lexicographical level
		code[cx].m = m; // modifier
		cx++;
	}
}


void error(int error){
	printf("Error number %d, ", error);
	switch (error) {
		case 1:
			printf("Use = instead of :=.");
			break;
		case 2:
			printf("= must be followed by a number.");
			break;
		case 3:
			printf("Identifier must be followed by =.");
			break;
		case 4:
			printf("const, var, procedure must be followed by identifier.");
			break;
		case 5:
			printf("Semicolon or comma missing.");
			break;
		case 6:
			printf("Incorrect symbol after procedure declaration.");
			break;
		case 7:
			printf("Statement expected.");
			break;
		case 8:
			printf("Incorrect symbol after statement part in block.");
			break;
		case 9:
			printf("Period expected.");
			break;
		case 10:
			printf("Semicolon between statements missing.");
			break;
		case 11:
			printf("Undeclared identifier.");
			break;
		case 12:
			printf("Assignment to constant or procedure is not allowed.");
			break;
		case 13:
			printf("Assignment operator expected.");
			break;
		case 14:
			printf("call must be followed by an identifier.");
			break;
		case 15:
			printf("Call of a constant or variable is meaningless.");
			break;
		case 16:
			printf("then expected.");
			break;
		case 17:
			printf("Semicolon or } expected.");
			break;
		case 18:
			printf("do expected");
			break;
		case 19:
			printf("Incorrect symbol following statement.");
			break;
		case 20:
			printf("Relational operator expected.");
			break;
		case 21:
			printf("Expression must not contain a procedure identifier.");
			break;
		case 22:
			printf("Right parenthesis missing.");
			break;
		case 23:
			printf("The preceding factor cannot begin with this symbol.");
			break;
		case 24:
			printf("An expression cannot begin with this symbol.");
			break;
		case 25:
			printf("This number is too large.");
			break;
		case 26:
			printf("Code too long");
			break;
		default:
			printf("Other error.");
			break;
	}

	printf("\n");
	exit(1);
}

char* getNoCommentCode(char* filename){
    FILE *fp = fopen(filename, "r");

    int c;
    char *code, *real;
    int i = 0, fileLength;

    code = malloc(20000 * sizeof(char));

    while(fscanf(fp, "%c", &c) != EOF)
    {
        code[i] = c;
        i++;
        fileLength++;
    }

    real = createSourceNoComment(code, fileLength);

	return real;

}

int readNextToken(char *rcode, int i, int codeLength)
{

	if (i >=codeLength) {
		return -1; //no more code
	} else {

		for (i; i<codeLength; i++)
		{
			while (rcode[i] == ' ') {
				i++;
			}

			if(isdigit(rcode[i])){
				token = numbersym;


				if((isdigit(rcode[i+4])) && (isdigit(rcode[i+3])) && (isdigit(rcode[i+2])) && (isdigit(rcode[i+1])))
				{
					lval.num = (rcode[i+4] - '0') * 10000;
					lval.num += (rcode[i+3]- '0') * 1000;
					lval.num += (rcode[i+2]- '0') * 100;
					lval.num += (rcode[i+1]- '0') * 10;
					lval.num += (rcode[i]- '0') * 1;
					return i=i+5;
				}
				else if((isdigit(rcode[i+3])) && (isdigit(rcode[i+2]))&& (isdigit(rcode[i+1])))
				{
					lval.num += (rcode[i+3]- '0') * 1000;
					lval.num += (rcode[i+2]- '0') * 100;
					lval.num += (rcode[i+1]- '0') * 10;
					lval.num += (rcode[i]- '0') * 1;
					return i=i+4;
				}
				else if(isdigit(rcode[i+2]) && (isdigit(rcode[i+1])))
				{
					lval.num += (rcode[i+2]- '0') * 100;
					lval.num += (rcode[i+1]- '0') * 10;
					lval.num += (rcode[i]- '0') * 1;
					return i=i+3;
				}
				else if(isdigit(rcode[i+1]))
				{
					lval.num += (rcode[i+1]- '0') * 10;
					lval.num += (rcode[i]- '0') * 1;
					return i=i+2;
				}
				else{
					lval.num += (rcode[i]- '0') * 1;
					return i = i+1;
				}
			}

			else if((rcode[i] == 'n') && (rcode[i+1] == 'u') && (rcode[i+2] == 'l') && (rcode[i+3] == 'l')) {
				token = nulsym;
				i+=4;
			}

			else if(rcode[i] == '+'){
				token = plussym;
				i+=1;
			}

			else if(rcode[i] == '-'){
				token = minussym;
				i+=1;
			}

			else if(rcode[i] == '*'){
				token = multsym;
				i+=1;
			}

			else if(rcode[i] == '/'){
				token = slashsym;
				i+=1;
			}

			else if((rcode[i] == 'o') && (rcode[i+2] == 'd') && (rcode[i+3] == 'd')) {
				token = oddsym;
				i+=3;
			}

			else if((rcode[i] == '=')){
				token = eqsym;
				i+=1;
			}

			else if((rcode[i] == '!') && (rcode[i+1] == '=')){
				token = neqsym;
				i+=2;
			}

			else if((rcode[i] == '>') && (rcode[i+1] == '=')){
				token = geqsym;
				i+=2;
			}

			else if((rcode[i] == '<')){
				token = lessym;
				i+=1;
			}

			else if((rcode[i] == '>')){
				token = gtrsym;
				i+=1;
			}

			else if((rcode[i] == '<') && (rcode[i+1] == '=')){
				token = leqsym;
				i+=2;
			}

			else if((rcode[i] == '(')){
				token = lparentsym;
				i+=1;
			}

			else if((rcode[i] == ')')){
				token = rparentsym;
				i+=1;
			}

			else if(rcode[i] == ','){
				token = commasym;
				i+=1;
			}

			else if(rcode[i] == ';'){
				token = semicolonsym;
				i+=1;
			}

			else if(rcode[i] == '.'){
				token = periodsym;
				i+=1;
			}

			else if(rcode[i] == ':' && rcode[i+1] == '='){
				token = becomessym;
				i=i+2;
			}

			else if((rcode[i] == 'b') && (rcode[i+1] == 'e') && (rcode[i+2] == 'g') &&
			(rcode[i+3] == 'i') && (rcode[i+4] == 'n')){
				token = beginsym;
				i=i+5;
			}

			else if((rcode[i] == 'e') && (rcode[i+1] == 'n') && (rcode[i+2] == 'd' )){
				token = endsym;
				i=i+3;
			}

			else if((rcode[i] == 'i') && (rcode[i+1] == 'f')){
				token = ifsym;
				i+=2;
			}

			else if((rcode[i] == 't') && (rcode[i+1] == 'h') && (rcode[i+2] == 'e') && (rcode[i+3] == 'n')) {
				token = thensym;
				i+=4;
			}

			else if((rcode[i] == 'w') && (rcode[i+1] == 'h') && (rcode[i+2] == 'i') && (rcode[i+3] == 'l') && (rcode[i+4] == 'e')){
				token = whilesym;
				i=i+5;
			}

			else if((rcode[i] == 'd') && (rcode[i+1] == 'o')){
				token = dosym;
				i+=2;
			}

			else if((rcode[i] == 'c') && (rcode[i+1] == 'a') && (rcode[i+2] == 'l') && (rcode[i+3] == 'l')) {
				token = callsym;
				i+=4;
			}

			else if((rcode[i] == 'c') && (rcode[i+1] == 'o') && (rcode[i+2] == 'n') && (rcode[i+3] == 's') && (rcode[i+4] == 't')){
				token = constsym;
				i=i+5;
			}

			else if((rcode[i] == 'v') && (rcode[i+1] == 'a') && (rcode[i+2] == 'r')){
				token = varsym;
				return i=i+3;
			}

			else if((rcode[i] == 'p') && (rcode[i+1] == 'r') && (rcode[i+2] == 'o') && (rcode[i+3] == 'c') && (rcode[i+4] == 'e') && (rcode[i+5] == 'd') && (rcode[i+6] == 'u') &&
			(rcode[i+7] == 'r') && (rcode[i+8] == 'e')){
				token = procsym;
				i+=8;
			}

			if((rcode[i] == 'r') && (rcode[i+1] == 'e') && (rcode[i+2] == 'a') && (rcode[i+3] == 'd')) {
				token = readsym;
				i+=4;
			}

			if((rcode[i] == 'w') && (rcode[i+1] == 'r') && (rcode[i+2] == 'i') && (rcode[i+3] == 't') && (rcode[i+4] == 'e')){
				token = writesym;
				i=i+5;
			}

			else if((rcode[i] == 'e') && (rcode[i+1] == 'l') && (rcode[i+2] == 's') && (rcode[i+3] == 'e')) {
				token = elsesym;
				i+=4;
			}

			else if(rcode[i] == ':'){
				token = colonsym;
				i=i+2;
			}

			else if(isalpha(rcode[i])){
				int j;

				token = identsym;

				for (j = i; isalpha(rcode[j]); j++) {
					lval.id[j - i] = rcode[i];
				}

				i = i + j;

			}
		}
	}

}

char *createSourceNoComment(char *rcode, int fileLength)
{
    int i;
    char *real;

    real = malloc(20000 * sizeof(char));

    //printf("source rcode without comments:\n-----------------------------\n");
    for(i=0; i < fileLength; i++)
    {
        if(rcode[i] == '/' && rcode[i+1] == '*')
        {
            //skip ahead 2 spaces.
            i=i+2;
            while(rcode[i] != '*' && rcode[i+1] != '/')
                i++;
        }

        if(rcode[i] != '*' && rcode[i] != '/')
        {
            //printf("%c", rcode[i]);
            real[i]=rcode[i];
        }
    }
    //printf("\n\ntokens\n-------\n");

    return real;

}
