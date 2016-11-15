//Project #3 Parser & Code Generator for Tiny PL/0
//Group 29


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_SYMBOL_TABLE_SIZE 100
#define CODE_SIZE 400




typedef struct symbol {
	int kind; // const = 1, var = 2, proc = 3
	char name[12]; // name up to 11 chars
	int val; // number (ASCII value)
	int level; // L level
	int addr; // M address
} symbol;


// token type
typedef enum {
	nulsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
	gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym,
	whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym , elsesym
} token_type;

typedef struct{
	int op; //opcode
	int l;  //lexicographical level
	int m;  //modifier
} code_struct;

// semantic value
// either string (char *) for the name of a constant/an identifier
// or int for the value of the number
typedef union {
	char *id;
	int num;
} LVAL;


//global var holding current token type.
token_type token;

// global variable holding the semantic value of the current token
LVAL lval;

//counts synbols in symbol_table
int symbol_count;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

int cx;
code_struct code[CODE_SIZE];


// function advance that move to the next token and
// updates the above two global variables
void advance();


// function get_symbol that looks up a symbol in symbol table by name and
// returns pointer symbol if found and NULL if not found

symbol *get_symbol(char[] name);


FILE *fin;
FILE *fout;

// function put_symbol that puts a symbol into symbol table provided that
// a symbol with this name does not exist (calls error function if name already exists)
void put_symbol(int kind, char[] name, int num, int level, int modifier);

void emit(int op, int l, int m);
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
void error(int n);





int main(int argc, char **argv){

	if (argc < 3) {
		printf("Please incude file in and out to read\n");
		exit(1);
	} else {
		fin = fopen(argv[1],"r");
		fout = fopen(argv[2],"w")

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



}

symbol *get_symbol(char[] name){

}

void put_symbol(int kind, char[] name, int num, int level, int modifier){

}

void advance(){

	if (fscanf(fin,"%s",)) {
		/* code */
	}
}

void program(){
	advance();
	block();
	if (token !== periodsym)) {
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
			if (token != equalsym) {
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
			error(3)
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

		if () {
			error(20);
		}

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
