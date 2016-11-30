//Project #3 Parser & Code Generator for Tiny PL/0
//Group 29
//Alexander Dossantos
//-Brian Nguyen
//-Heather Connors
//-Ryan Hoeck


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

char lval_id[12];
int lval_value;
//lval.id = malloc(sizeof(char) * 12); // ident up to 11 chars

//counts synbols in symbol_table
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
int symbol_count = 0;

char *rawCode;
int rawCodeIndex = 0;
int rawCodeSize = 0;
int level;
int curLevel=0; //Alex: I added this variable to keep a count of the current level.
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
//int num_vars = 0;
void term();
void factor();
void error(int n);
char *createSourceNoComment(char *code, int fileLength);
int readNextToken(char *code, int i, int codeLength);
char* getNoCommentCode(char* filename);
void outputCode();




int main(int argc, char **argv){


	int i;

	if (argc < 3) {
		printf("Please incude file in and out to read\n");
		exit(1);
	} else {
		rawCode = getNoCommentCode(argv[1]);
		//printf("rawcode %s\n",rawCode );
		fout = fopen(argv[2],"w");
	}

	i = 0;
	if (rawCode != NULL) {
		while (rawCode[i] != '\0') {
			//printf("i\n", i);
			rawCodeSize++;
			i++;
		}
	}




	//initalize outcode to 0
	cx = 0;
	curLevel = 1;
	for(i = 0; i < CODE_SIZE; i++){
		code[i].op = 0;
		code[i].l = 0;
		code[i].m = 0;
	}



	//start our program call
	program();

	//if we get here, we parsed successfully!
	printf("No errors, program is syntactically correct\n");
	outputCode();


	fclose(fout);

	return 0;



}



void outputCode(){

	//fprintf(fout,"PM/0 Code\n");
	//fprintf(fout,"-=-=-=-=-=-=-=");
	//fprintf(fout,"\n\n");
	//fprintf(fout,"Line\tOP\tL\tM");
	//fprintf(fout,"\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	for(int i = 0; i< CODE_SIZE; i++)
		if (code[i].op == 0 && code[i].l == 0 && code[i].m == 0) {
			/* code */
		} else {
			fprintf(fout,"\t%d\t%d\t%d\n", code[i].op, code[i].l, code[i].m);
		}

}



symbol *get_symbol(char* name){
	for( int i = 0; i < symbol_count; i++){// for loop control
		if( strcmp(symbol_table[i].name, name)== 0 && curLevel >= symbol_table[i].level){// Alex: this now checks to make sure it's on the same level
			return &symbol_table[i];
		}
	}

	//printf("Getsymbol: token is %s\n", lval_id);
	error(11);
	return NULL;

}

void put_symbol(int kind, char* name, int num, int level, int modifier){
	int foundFlag =0, lastPos = 0;// foundFlag is if a member already exists, lastPos is for the position of the current
									//NULL member.



	for( int i = 0; i < symbol_count; i++){
		if(strcmp(symbol_table[i].name, name) == 0 && symbol_table[i].level == level){// Alex: same as getsymbol; checks for level.
			//printf("another one\n");
			foundFlag = 1;
		}
	}

	//printf("name: %s, level: %d\n",name, level );

	//add symbol or ERROR
	if(foundFlag == 0){
		symbol_table[symbol_count].kind = kind;
		strcpy(symbol_table[symbol_count].name, name);
		if(kind == 1){// if constant, save the number
			symbol_table[symbol_count].val = num;
		}
		else if (kind == 2){// if variable, save level and modifier.
			symbol_table[symbol_count].level = level;
			symbol_table[symbol_count].addr = modifier;
		}
	} else{
		//printf("putcheck: token is %s\n", lval_id);
		error(11);

	}

	symbol_count++;
}



void advance(){

	rawCodeIndex = readNextToken(rawCode,rawCodeIndex,rawCodeSize);
	if (rawCodeIndex == -1) {
		//printf("Exit");
		exit(0);
	}
	if (token == numbersym) {
		//printf("token %d\n",lval_value );
	} else {
		//printf("token %s\n",lval_id );
	}

}

void program(){
	advance();
	block();
	if (token != periodsym) {
		error(9);
	}
	emit(9,0,2);
}

void block(){

	int num_vars;
	curLevel++;
	char ident[12];
	int val;
	int jump = cx;
	emit(7,0,0); //emit jump

	if (token == constsym) {
		do {
			advance();
			if (token != identsym) {
				error(4);
			}

			strcpy(ident,lval_id); //hold onto current identsym

			advance();
			if (token != eqsym) {
				error(3);
			}

			advance();
			if (token != numbersym) {
				error(2);
			}

			//keep track of number,
			//add it all to the symbol table

			val = lval_value;
			put_symbol(1,ident,val,curLevel,0);

			advance();

		} while(token == commasym);

		if (token != semicolonsym) {
			error(5);
		}

		advance();
	}


	if (token == varsym) {
		num_vars = 0;

		do {
			advance();
			if (token != identsym) {
				error(4);
			}
			num_vars++;
			strcpy(ident,lval_id); //hold onto current identsym
			put_symbol(2,ident,0,curLevel ,3 + num_vars);

			advance();
		} while(token == commasym);

		if (token != semicolonsym) {
			error(5);
		}


		advance();
	}

	//emit(6,0,4+num_vars);

	while (token == procsym) {

		//printf("entered the procedure area\n");
		//printf("precheck: token is %s\n", lval_id);
		advance();
		//printf("token: %s \n",lval_id );
		if (token != identsym) {
			//printf("here\n");
			error(4);
		}

		strcpy(ident,lval_id); //hold onto current identsym
		put_symbol(3,ident,0,curLevel,jump);//NOTE

		//printf("post check: token is %s\n", lval_id);
		advance();
		if (token != semicolonsym) {
			//printf("bullseye: token is %s\n", lval_id);
			error(17);
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

	code[jump].m = cx;
	//printf("numbars %d \n", num_vars);
	emit(6,0,num_vars + 4);

	statement();
	emit(2,0,0);
	curLevel--;

}


void statement(){
	if (token == identsym) {

		symbol *tempSymbol = get_symbol(lval_id);
		//printf("got here\n");

		if (tempSymbol->kind != 2) {
			error(12);
		}


		advance();
		if (token != becomessym) {
			error(3);
		}

		advance();

		expression();

		emit(4,curLevel - tempSymbol->level,tempSymbol->addr);

	} else if (token == callsym){

		advance();
		if (token != identsym) {
			error(14);
		}

		symbol *tempSymbol = get_symbol(lval_id);

		if (tempSymbol->kind != 3) {
			error(14);
		}

		emit(5, tempSymbol->level ,tempSymbol->addr+1);//TODO

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
		//curLevel-=1;//NOTE



	} else if (token == ifsym) {
		advance();

		condition();

		if (token != thensym) {
			error(16);
		}


		int ctemp1 = cx;
		emit(8,0,0);
		advance();
		statement();
		if (token != elsesym) {
			code[ctemp1].m = cx;
		} else {
			int ctemp2 = cx;
			emit(7,0,0);
			code[ctemp1].m = cx;
			advance();
			statement();
			code[ctemp2].m = cx;
		}



	} else if (token == whilesym){
		int cx1 = cx;
		advance();
		condition();
		int cx2 = cx;
		emit(8,0,0);

		if (token != dosym) {
			error(18);
		} else {
			advance();
		}
		statement();
		emit(7,0,cx1);
		code[cx2].m = cx;

	} else if (token == readsym){

		advance();
		if (token != identsym) {
			error(14);
		}

		symbol *tempSymbol = get_symbol(lval_id);

		if (tempSymbol->kind != 2) {
			error(12);
		}
		emit(3,0,2);
		emit(4,curLevel - tempSymbol->level,tempSymbol->addr);



		advance();

	} else if (token == writesym){
		advance();
		if (token != identsym) {
			error(14);
		}

		symbol *tempSymbol = get_symbol(lval_id);

		if (tempSymbol->kind == 1) {
			emit(1,0,tempSymbol->val);
			emit(9,0,1);
		} else if (tempSymbol->kind == 2) {
			emit(3,curLevel - tempSymbol->level,tempSymbol->addr);
			emit(9,0,0);
		}

		advance();
	}
}


void condition(){

	if (token == oddsym) {
		advance();
		expression();
		emit(2,0,6); //odd
	} else {
		expression();

		int relop = token;


		//NOTE Can be shorted into a single if, however this improves readability
		if (token == eqsym) {
			/* code */
		} else if (token == neqsym){
			/* code */
		} else if (token == lessym){
			/* code */
		} else if (token == leqsym) {
			/* code */
		} else if (token == gtrsym) {
			/* code */
		} else if (token == geqsym) {
			/* code */
		} else {
			error(20);
		}

		advance();
		expression();

		if (relop == eqsym) {
			emit(2, 0, 8);
		} else if (relop == neqsym){
			emit(2, 0, 9);
		} else if (relop == lessym){
			emit(2, 0, 10);
		} else if (relop == leqsym) {
			emit(2, 0, 11);
		} else if (relop == gtrsym) {
			emit(2, 0, 12);
		} else if (relop == geqsym) {
			emit(2, 0, 13);
		} else {
			error(20);
		}

	}


}


void expression(){

	int oper;

	if (token == plussym || token == minussym) {
		oper = token;
		advance();
		term();
		if (oper == minussym) {
			emit(2,0,1); //negate
		}
	} else {
		term();
	}

	while (token == plussym || token == minussym) {
		oper = token;
		advance();
		term();
		if (oper == plussym) {
			emit(2,0,2); //add
		} else {
			emit(2,0,3); //subtraction
		}
	}

	if(token == multsym || token == slashsym)
	term();
}


void term(){

	int oper;

	factor();

	while (token == multsym || token == slashsym) {
		oper = token;
		advance();

		factor();

		if (oper == multsym) {
			emit(2,0,4);
			//printf("this is a test");
		} else {
			emit(2,0,5);
		}

	}

	//printf("done with term now\n");
}

/*
int kind; // constant = 1, variable = 2
string name; // name of constant or variable
int num; // number number is only set for constant
int level; // L level level and modifier are only set for variable,
int modifier; // M modifier but level is always 0 for tiny PL/0


	int kind; // const = 1, var = 2, proc = 3
	char name[12]; // name up to 11 chars
	int val; // number (ASCII value)
	int level; // L level
	int addr; // M address
*/
void factor(){
//printf("this is a test");
	if (token == identsym) {
		//TODO check if ident is already in symbol table
		//TODO get from mem or symbol table based on var or const
		//and emit based on above
		symbol *tempSymbol = get_symbol(lval_id);
		if (tempSymbol == NULL) {
			error(28);// this condition is met if tempSymbol is null: AKA not saved
					//put_symbol();
		}
		if(tempSymbol->kind == 1){
			emit(1,0,tempSymbol->val);
		} else if(tempSymbol->kind == 2){
			emit(3,curLevel - tempSymbol->level,tempSymbol->addr);
		}

		advance();
	} else if (token == numbersym) {
		advance();
		emit(1,0,lval_value);

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
	if (fp == NULL) {
		printf("Couldnt open file\n");
		exit(1);
	}

	int c;
	char *code, *real;
	int i = 0;
	int fileLength = 0;

	code = malloc(20000 * sizeof(char));



	while(fscanf(fp, "%c", &c) != EOF)
	{
		code[i] = c;
		i++;
		fileLength++;
	}

	//code[i] = '\0';
	//fileLength--;


	real = createSourceNoComment(code, fileLength);

	return real;

}

int readNextToken(char *rcode, int i, int codeLength)
{

	if (i >=codeLength) {
		printf("Got here\n" );
		return -1; //no more code
	} else {

		for (i; i<codeLength; i++)
		{

			while (rcode[i] != '\0' && rcode[i] == ' ') {
				i++;
			}
			if (i >=codeLength) {
				return -1; //no more code
			}

			if(isdigit(rcode[i])){
				token = numbersym;
				lval_id[0] = 's';
				lval_id[1] = '\0';
				lval_value = 0;



				if((isdigit(rcode[i+4])) && (isdigit(rcode[i+3])) && (isdigit(rcode[i+2])) && (isdigit(rcode[i+1])))
				{
					lval_value = (rcode[i+4] - '0') * 10000;
					lval_value += (rcode[i+3]- '0') * 1000;
					lval_value += (rcode[i+2]- '0') * 100;
					lval_value += (rcode[i+1]- '0') * 10;
					lval_value += (rcode[i]- '0') * 1;
					return i=i+5;
				}
				else if((isdigit(rcode[i+3])) && (isdigit(rcode[i+2]))&& (isdigit(rcode[i+1])))
				{
					lval_value += (rcode[i+3]- '0') * 1000;
					lval_value += (rcode[i+2]- '0') * 100;
					lval_value += (rcode[i+1]- '0') * 10;
					lval_value += (rcode[i]- '0') * 1;
					return i=i+4;
				}
				else if(isdigit(rcode[i+2]) && (isdigit(rcode[i+1])))
				{
					lval_value += (rcode[i+2]- '0') * 100;
					lval_value += (rcode[i+1]- '0') * 10;
					lval_value += (rcode[i]- '0') * 1;
					return i=i+3;
				}
				else if(isdigit(rcode[i+1]))
				{
					lval_value += (rcode[i+1]- '0') * 10;
					lval_value += (rcode[i]- '0') * 1;
					return i=i+2;
				}
				else{
					lval_value += (rcode[i]- '0') * 1;
					return i = i+1;
				}
			}

			else if((rcode[i] == 'n') && (rcode[i+1] == 'u') && (rcode[i+2] == 'l') && (rcode[i+3] == 'l')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = '\0';
				token = nulsym;
				return i+=4;
			}

			else if(rcode[i] == '+'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = plussym;
				return i+=1;
			}

			else if(rcode[i] == '-'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = minussym;
				return i+=1;
			}

			else if(rcode[i] == '*'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = multsym;
				return i+=1;
			}

			else if(rcode[i] == '/'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = slashsym;
				return i+=1;
			}

			else if((rcode[i] == 'o') && (rcode[i+2] == 'd') && (rcode[i+3] == 'd')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = '\0';
				token = oddsym;
				return i+=3;
			}

			else if((rcode[i] == '=')){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = eqsym;
				return i+=1;
			}

			else if((rcode[i] == '!') && (rcode[i+1] == '=')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = neqsym;
				return i+=2;
			}

			else if((rcode[i] == '>') && (rcode[i+1] == '=')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = geqsym;
				return i+=2;
			}

			else if((rcode[i] == '<')){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = lessym;
				return i+=1;
			}

			else if((rcode[i] == '>')){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = gtrsym;
				return i+=1;
			}

			else if((rcode[i] == '<') && (rcode[i+1] == '=')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = leqsym;
				return i+=2;
			}

			else if((rcode[i] == '(')){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = lparentsym;
				return i+=1;
			}

			else if((rcode[i] == ')')){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = rparentsym;
				return i+=1;
			}

			else if(rcode[i] == ','){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = commasym;
				return i+=1;
			}

			else if(rcode[i] == ';'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = semicolonsym;
				return i+=1;
			}

			else if(rcode[i] == '.'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = periodsym;
				return i+=1;
			}

			else if(rcode[i] == ':' && rcode[i+1] == '='){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = becomessym;
				return i=i+2;
			}

			else if((rcode[i] == 'b') && (rcode[i+1] == 'e') && (rcode[i+2] == 'g') &&
			(rcode[i+3] == 'i') && (rcode[i+4] == 'n')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = rcode[i+4];
				lval_id[5] = '\0';
				token = beginsym;
				return i=i+5;
			}

			else if((rcode[i] == 'e') && (rcode[i+1] == 'n') && (rcode[i+2] == 'd' )){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = '\0';
				token = endsym;
				return i=i+3;
			}

			else if((rcode[i] == 'i') && (rcode[i+1] == 'f')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = ifsym;
				return i+=2;
			}

			else if((rcode[i] == 't') && (rcode[i+1] == 'h') && (rcode[i+2] == 'e') && (rcode[i+3] == 'n')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = '\0';
				token = thensym;
				return i+=4;
			}

			else if((rcode[i] == 'w') && (rcode[i+1] == 'h') && (rcode[i+2] == 'i') && (rcode[i+3] == 'l') && (rcode[i+4] == 'e')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = rcode[i+4];
				lval_id[5] = '\0';
				token = whilesym;
				return i=i+5;
			}

			else if((rcode[i] == 'd') && (rcode[i+1] == 'o')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = '\0';
				token = dosym;
				return i+=2;
			}

			else if((rcode[i] == 'c') && (rcode[i+1] == 'a') && (rcode[i+2] == 'l') && (rcode[i+3] == 'l')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = '\0';
				token = callsym;
				return i+=4;
			}

			else if((rcode[i] == 'c') && (rcode[i+1] == 'o') && (rcode[i+2] == 'n') && (rcode[i+3] == 's') && (rcode[i+4] == 't')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = rcode[i+4];
				lval_id[5] = '\0';
				token = constsym;
				return i=i+5;
			}

			else if((rcode[i] == 'v') && (rcode[i+1] == 'a') && (rcode[i+2] == 'r')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = '\0';
				token = varsym;
				return i=i+3;
			}

			else if((rcode[i] == 'p') && (rcode[i+1] == 'r') && (rcode[i+2] == 'o') && (rcode[i+3] == 'c') && (rcode[i+4] == 'e') && (rcode[i+5] == 'd') && (rcode[i+6] == 'u') &&
			(rcode[i+7] == 'r') && (rcode[i+8] == 'e')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = rcode[i+4];
				lval_id[5] = rcode[i+5];
				lval_id[6] = rcode[i+6];
				lval_id[7] = rcode[i+7];
				lval_id[8] = rcode[i+8];
				lval_id[9] = '\0';
				token = procsym;
				return i+=9; //Alex: I  made a change here for the index.. was at 8
			}

			if((rcode[i] == 'r') && (rcode[i+1] == 'e') && (rcode[i+2] == 'a') && (rcode[i+3] == 'd')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = '\0';
				token = readsym;
				return i+=4;
			}

			if((rcode[i] == 'w') && (rcode[i+1] == 'r') && (rcode[i+2] == 'i') && (rcode[i+3] == 't') && (rcode[i+4] == 'e')){
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = rcode[i+4];
				lval_id[5] = '\0';
				token = writesym;
				return i=i+5;
			}

			else if((rcode[i] == 'e') && (rcode[i+1] == 'l') && (rcode[i+2] == 's') && (rcode[i+3] == 'e')) {
				lval_id[0] = rcode[i];
				lval_id[1] = rcode[i+1];
				lval_id[2] = rcode[i+2];
				lval_id[3] = rcode[i+3];
				lval_id[4] = '\0';
				token = elsesym;
				return i+=4;
			}

			else if(rcode[i] == ':'){
				lval_id[0] = rcode[i];
				lval_id[1] = '\0';
				token = colonsym;
				return i=i+2;
			}

			else if(isalpha(rcode[i])){
				//printf("%c\n", rcode[i]);
				int j;

				token = identsym;



				for (j = i; isalpha(rcode[j]) || isdigit(rcode[j]); j++) {
					lval_id[j - i] = rcode[j];
					//printf("char %d: %c\n",(j), rcode[j]);
				}

				lval_id[j-i] = '\0';

				return i =  j;

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
			while(rcode[i] != '*' && rcode[i+1] != '/'){
				i++;
			}
			i=i+2;
		}



		if(i < fileLength){
			//printf("%c", rcode[i]);
			real[i]=rcode[i];
		}
	}
	real[i+1] = '\0';
	//printf("\n\ntokens\n-------\n");

	return real;

}
