
// token type
typedef enum {
	nulsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
	gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
	periodsym, becomessym, beginsym, endsym, ifsym, thensym,
	whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
	readsym , elsesym, colonsym
} token_type;

typedef struct{
	int op; //opcode
	int l;  //lexicographical level
	int m;  //modifier
} code_struct;

// semantic value
// either string (char *) for the name of a constant/an identifier
// or int for the value of the number
typedef struct {
	char *id;
	int num;
} LVAL;

int readNextToken(char *code, int i, int codeLength);

char* getNoCommentCode(char* filename);
