/*
  $Id: compile.y,v 1.85 2001/03/10 15:56:16 paf Exp $
*/

%{
#define YYSTYPE  Array/*<op>*/ *
#define YYPARSE_PARAM  pc
#define YYLEX_PARAM  pc
#define YYDEBUG  1
#define YYERROR_VERBOSE
#define yyerror(msg)  real_yyerror((parse_control *)pc, msg)
#define YYPRINT(file, type, value)  yyprint(file, type, value)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compile_tools.h"
#include "pa_value.h"
#include "pa_request.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "core.h"

#define SELF_ELEMENT_NAME "self"
#define USE_CONTROL_METHOD_NAME "USE"

int real_yyerror(parse_control *pc, char *s);
static void yyprint(FILE *file, int type, YYSTYPE value);
int yylex(YYSTYPE *lvalp, void *pc);


// local convinient inplace typecast & var
#define PC  ((parse_control *)pc)
#define POOL  *PC->pool
#undef NEW
#define NEW new(POOL)
%}

%pure_parser

%token EON
%token STRING
%token BOGUS

%token BAD_STRING_COMPARISON_OPERATOR

%token LAND "&&"
%token LOR "||"
%token LXOR "##"

%token NLE "<="
%token NGE ">="
%token NEQ "=="
%token NNE "!="

%token SLT "lt"
%token SGT "gt"
%token SLE "le"
%token SGE "ge"
%token SEQ "eq"
%token SNE "ne"

%token DEF "def"
%token IN "in"
%token FEXISTS "-f"

/* logical */
%left "lt" "gt" "le" "ge"
%left "eq" "ne"
%left '<' '>' "<=" ">=" "##"
%left "==" "!="
%left "||"
%left "&&"
%left "def" "in" "-f"
%left '!'

/* bitwise */
%left '#'
%left '&' '|'
%left '~'

/* numerical */
%left '-' '+'
%left '*' '/' '%'
%left NEG     /* negation: unary - */

%%

all: /* TODO: � ^execute ���������� ������ �����-�� name, ��. 'RUN' */
	one_big_piece {
	Method& method=*NEW Method(POOL, 
		*main_method_name, 
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		$1/*parser_code*/, 0/*native_code*/);
	PC->vclass->add_method(*main_method_name, method);
}
|	methods;

methods: method | methods method;
one_big_piece: maybe_codes;

method: control_method | code_method;

control_method: '@' STRING '\n' 
				control_strings {
	const String& command=*SLA2S($2);
	YYSTYPE strings_code=$4;
	if(strings_code->size()<1*2) {
		strcpy(PC->error, "@");
		strcat(PC->error, command.cstr());
		strcat(PC->error, " is empty");
		YYERROR;
	}
	if(command==CLASS_NAME) {
		if(PC->vclass!=&PC->request->root_class) { // already changed from default?
			strcpy(PC->error, "class already have a name '");
			strncat(PC->error, PC->vclass->name().cstr(), 100);
			strcat(PC->error, "'");
			YYERROR;
		}
		if(strings_code->size()==1*2) {
			// new class' name
			const String *name=SLA2S(strings_code);
			// creating the class
			PC->vclass=NEW VClass(POOL);
			PC->vclass->set_name(*name);
			// defaulting base. may change with @BASE
			PC->vclass->set_base(PC->request->root_class);
			// append to request's classes
			PC->request->classes_array()+=PC->vclass;
			PC->request->classes().put(*name, PC->vclass);
		} else {
			strcpy(PC->error, "@"CLASS_NAME" must contain sole name");
			YYERROR;
		}
	} else {
		if(command==USE_CONTROL_METHOD_NAME) {
			for(int i=0; i<strings_code->size(); i+=2) {
				String file(*SLA2S(strings_code, i));
				file.APPEND_CONST(".p");
				PC->request->use(file.cstr(), 0);
			}
		} else if(command==BASE_NAME) {
			if(PC->vclass->base()!=&PC->request->root_class) { // already changed from default?
				strcpy(PC->error, "there must be only one @"BASE_NAME);
				YYERROR;
			}
			if(strings_code->size()==1*2) {
				// TODO: ���������� self � ����������� base
				const String& base_name=*SLA2S(strings_code);
				VClass *base=static_cast<VClass *>(
					PC->request->classes().get(base_name));
				if(!base) {
					strcpy(PC->error, base_name.cstr());
					strcat(PC->error, ": undefined class in @"BASE_NAME);
					YYERROR;
				}
				PC->vclass->set_base(*base);
			} else {
				strcpy(PC->error, "@"BASE_NAME" must contain sole name");
				YYERROR;
			}
		} else {
			strcpy(PC->error, command.cstr());
			strcat(PC->error, ": invalid special name. valid names are "
				CLASS_NAME", "USE_CONTROL_METHOD_NAME" and "BASE_NAME);
			YYERROR;
		}
	}
};
control_strings: control_string | control_strings control_string { $$=$1; P($$, $2) };
control_string: maybe_string '\n';
maybe_string: empty | STRING;

code_method: '@' STRING bracketed_maybe_strings maybe_bracketed_strings maybe_comment '\n' 
			maybe_codes {
	const String *name=SLA2S($2);

	YYSTYPE params_names_code=$3;
	Array *params_names=0;
	if(int size=params_names_code->size()) {
		params_names=NEW Array(POOL);
		for(int i=0; i<size; i+=2)
			*params_names+=SLA2S(params_names_code, i);
	}

	YYSTYPE locals_names_code=$4;
	Array *locals_names=0;
	if(int size=locals_names_code->size()) {
		locals_names=NEW Array(POOL);
		for(int i=0; i<size; i+=2)
			*locals_names+=SLA2S(locals_names_code, i);
	}

	Method& method=*NEW Method(POOL, 
		*name, 
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		$7, 0);
	PC->vclass->add_method(*name, method);
};

maybe_bracketed_strings: empty | bracketed_maybe_strings;
bracketed_maybe_strings: '[' maybe_strings ']' {$$=$2};
maybe_strings: empty | strings;
strings: STRING | strings ';' STRING { $$=$1; P($$, $3) };

maybe_comment: empty | STRING;

/* codes */

maybe_codes: empty | codes;

codes: code | codes code { 
	$$=$1; 
	P($$, $2);
};
code: write_string | action;
action: get | put | with | call;

/* get */

get: get_value {
	$$=$1; /* stack: resulting value */
	O($$, OP_WRITE); /* value=pop; wcontext.write(value) */
};
get_value: '$' get_name_value { $$=$2 }
get_name_value: name_without_curly_rdive EON | name_in_curly_rdive;
name_in_curly_rdive: '{' name_without_curly_rdive '}' { $$=$2 };
name_without_curly_rdive: 
	name_without_curly_rdive_read 
|	name_without_curly_rdive_root
|	name_without_curly_rdive_class;
name_without_curly_rdive_read: name_without_curly_rdive_code {
	$$=N(POOL); 
	Array *diving_code=$1;
	const String *first_name=SLA2S(diving_code);
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O($$, OP_WITH_SELF); /* stack: starting context */
		P($$, diving_code, 
			/* skip over... */
			diving_code->size()>2?3/*OP_+string+get_element*/:2/*OP_+string*/);
	} else {
		O($$, OP_WITH_READ); /* stack: starting context */
		P($$, diving_code);
	}
	/* diving code; stack: current context */
};
name_without_curly_rdive_root: ':' name_without_curly_rdive_code {
	$$=N(POOL); 
	O($$, OP_WITH_ROOT); /* stack: starting context */
	P($$, $2); /* diving code; stack: current context */
};
name_without_curly_rdive_class: class_prefix name_without_curly_rdive_code { $$=$1; P($$, $2) };
name_without_curly_rdive_code: name_advance2 | name_path name_advance2 { $$=$1; P($$, $2) };

/* put */

put: '$' name_expr_wdive construct {
	$$=$2; /* stack: context,name */
	P($$, $3); /* stack: context,name,constructor_value */
};
name_expr_wdive: 
	name_expr_wdive_write
|	name_expr_wdive_root
|	name_expr_wdive_class;
name_expr_wdive_write: name_expr_dive_code {
	$$=N(POOL);
	Array *diving_code=$1;
	const String *first_name=SLA2S(diving_code);
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O($$, OP_WITH_SELF); /* stack: starting context */
		P($$, diving_code, 
			/* skip over... */
			diving_code->size()>2?3/*OP_+string+get_element*/:2/*OP_+string*/);
	} else {
		O($$, OP_WITH_WRITE); /* stack: starting context */
		P($$, diving_code);
	}
	/* diving code; stack: current context */
};
name_expr_wdive_root: ':' name_expr_dive_code {
	$$=N(POOL); 
	O($$, OP_WITH_ROOT); /* stack: starting context */
	P($$, $2); /* diving code; stack: context,name */
};
name_expr_wdive_class: class_prefix name_expr_dive_code { $$=$1; P($$, $2) };

construct: construct_by_code | construct_by_expr;
construct_by_code: '[' any_constructor_code_value ']' {
	$$=$2; /* stack: context, name, value */
	O($$, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
}
;
construct_by_expr: '(' any_expr ')' { 
	$$=$2; /* stack: context, name, value */
	O($$, OP_CONSTRUCT_EXPR); /* value=pop; name=pop; context=pop; construct(context,name,value) */
}
;
any_constructor_code_value: 
	empty_string_value /* optimized $var[] case */
|	STRING /* optimized $var[STRING] case */
|	constructor_code_value /* $var[something complex] */
;
constructor_code_value: constructor_code {
	$$=N(POOL); 
	O($$, OP_CREATE_EWPOOL); /* stack: empty write context */
	P($$, $1); /* some code that writes to that context */
	O($$, OP_REDUCE_EWPOOL); /* context=pop; stack: context.value() */
};
constructor_code: codes__excluding_sole_str_literal;
codes__excluding_sole_str_literal: action | code codes { $$=$1; P($$, $2) };

/* call */

call: call_value {
	$$=$1; /* stack: value */
	O($$, OP_WRITE); /* value=pop; wcontext.write(value) */
};
call_value: '^' call_name store_params EON { /* ^field.$method{vasya} */
	$$=$2; /* with_xxx,diving code; stack: context,method_junction */
	O($$, OP_GET_METHOD_FRAME); /* stack: context,method_frame */
	P($$, $3); /* filling method_frame.store_params */
	O($$, OP_CALL); /* method_frame=pop; ncontext=pop; call(ncontext,method_frame) stack: value */
};

call_name: name_without_curly_rdive;

store_params: store_param | store_params store_param { $$=$1; P($$, $2) };
store_param: 
	store_square_param
|	store_round_param
|	store_curly_param
;
store_square_param: '[' store_code_param_parts ']' {$$=$2};
store_round_param: '(' store_expr_param_parts ')' {$$=$2};
store_curly_param: '{' maybe_codes '}' {
	$$=N(POOL); 
	PCA($$, $2);
};
store_code_param_parts:
	store_code_param_part
|	store_code_param_parts ';' store_code_param_part { $$=$1; P($$, $3) }
;
store_expr_param_parts:
	store_expr_param_part
|	store_expr_param_parts ';' store_expr_param_part { $$=$1; P($$, $3) }
;
store_code_param_part: 
	empty /* optimized [] case */
|	STRING { /* optimized [STRING] case */
	$$=$1;
	O($$, OP_STORE_PARAM);
}
|	constructor_code_value { /* [something complex] */
	$$=$1;
	O($$, OP_STORE_PARAM);
}
;
store_expr_param_part: write_expr_value {
	$$=N(POOL); 
	PCA($$, $1);
};
write_expr_value: any_expr {
	$$=$1;
	O($$, OP_WRITE);
};

/* name */

name_expr_dive_code: name_expr_value | name_path name_expr_value { $$=$1; P($$, $2) };

name_path: name_step | name_path name_step { $$=$1; P($$, $2) };
name_step: name_advance1 '.';
name_advance1: name_expr_value {
	/* stack: context */
	$$=$1; /* stack: context,name */
	O($$, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
};
name_advance2: name_expr_value {
	/* stack: context */
	$$=$1; /* stack: context,name */
	O($$, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
}
|	STRING BOGUS
;
name_expr_value: 
	STRING /* subname_is_const */
|	name_expr_subvar_value /* $subname_is_var_value */
|	name_expr_with_subvar_value /* xxx$part_of_subname_is_var_value[$...] */
;
name_expr_subvar_value: '$' subvar_ref_name_rdive {
	$$=$2;
	O($$, OP_GET_ELEMENT);
};
name_expr_with_subvar_value: STRING subvar_get_writes {
	$$=N(POOL); 
	O($$, OP_CREATE_EWPOOL);
	P($$, $1);
	O($$, OP_WRITE);
	P($$, $2);
	O($$, OP_REDUCE_EWPOOL);
};
subvar_ref_name_rdive: subvar_ref_name_rdive_read | subvar_ref_name_rdive_root;
subvar_ref_name_rdive_read: STRING {
	$$=N(POOL); 
	O($$, OP_WITH_READ);
	P($$, $1);
};
subvar_ref_name_rdive_root: ':' STRING {
	$$=N(POOL); 
	O($$, OP_WITH_ROOT);
	P($$, $2);
};
subvar_get_writes: subvar__get_write | subvar_get_writes subvar__get_write { $$=$1; P($$, $2) };
subvar__get_write: '$' subvar_ref_name_rdive {
	$$=$2;
	O($$, OP_GET_ELEMENT__WRITE);
};

class_prefix: STRING ':' {
	$$=$1; // stack: class name string
	O($$, OP_GET_CLASS);
};


/* with */

with: '$' name_without_curly_rdive '{' codes '}' {
	$$=$2;
	O($$, OP_CREATE_RWPOOL);
	P($$, $4);
	O($$, OP_REDUCE_RWPOOL);
	O($$, OP_WRITE);
};

/* expr */

any_expr:
	empty_double_value /* optimized $var() case */
|	expr_value /* $var(something) */
;
expr_value: expr {
	if(($$=$1)->size()==2) // only one string literal in there?
		change_string_literal_to_double_literal($$); // make that string literal Double
};
expr: 
	STRING
|	get_value
|	call_value
|	'"' string_inside_quotes_value '"' { $$ = $2; }
|	'(' expr ')' { $$ = $2; }
/* stack: operand // stack: @operand */
|	'-' expr %prec NEG { $$=$2;  O($$, OP_NEG) }
|	'~' expr { $$=$2;	 O($$, OP_INV) }
|	'!' expr { $$=$2;  O($$, OP_NOT) }
|	"def" expr { $$=$2;  O($$, OP_DEF) }
|	"in" expr { $$=$2;  O($$, OP_IN) }
|	"-f" expr { $$=$2;  O($$, OP_FEXISTS) }
/* stack: a,b // stack: a@b */
|	expr '-' expr {	$$=$1;  P($$, $3);  O($$, OP_SUB) }
|	expr '+' expr { $$=$1;  P($$, $3);  O($$, OP_ADD) }
|	expr '*' expr { $$=$1;  P($$, $3);  O($$, OP_MUL) }
|	expr '/' expr { $$=$1;  P($$, $3);  O($$, OP_DIV) }
|	expr '%' expr { $$=$1;  P($$, $3);  O($$, OP_MOD) }
|	expr '&' expr { $$=$1; 	P($$, $3);  O($$, OP_BIN_AND) }
|	expr '|' expr { $$=$1;  P($$, $3);  O($$, OP_BIN_OR) }
|	expr '#' expr { $$=$1;  P($$, $3);  O($$, OP_BIN_XOR) }
|	expr "&&" expr { $$=$1;  P($$, $3);  O($$, OP_LOG_AND) }
|	expr "||" expr { $$=$1;  P($$, $3);  O($$, OP_LOG_OR) }
|	expr "##" expr { $$=$1;  P($$, $3);  O($$, OP_LOG_XOR) }
|	expr '<' expr { $$=$1;  P($$, $3);  O($$, OP_NUM_LT) }
|	expr '>' expr { $$=$1;  P($$, $3);  O($$, OP_NUM_GT) }
|	expr "<=" expr { $$=$1;  P($$, $3);  O($$, OP_NUM_LE) }
|	expr ">=" expr { $$=$1;  P($$, $3);  O($$, OP_NUM_GE) }
|	expr "==" expr { $$=$1;  P($$, $3);  O($$, OP_NUM_EQ) }
|	expr "!=" expr { $$=$1;  P($$, $3);  O($$, OP_NUM_NE) }
|	expr "lt" expr { $$=$1;  P($$, $3);  O($$, OP_STR_LT) }
|	expr "gt" expr { $$=$1;  P($$, $3);  O($$, OP_STR_GT) }
|	expr "le" expr { $$=$1;  P($$, $3);  O($$, OP_STR_LE) }
|	expr "ge" expr { $$=$1;  P($$, $3);  O($$, OP_STR_GE) }
|	expr "eq" expr { $$=$1;  P($$, $3);  O($$, OP_STR_EQ) }
|	expr "ne" expr { $$=$1;  P($$, $3);  O($$, OP_STR_NE) }
;

string_inside_quotes_value: maybe_codes {
	$$=N(POOL);
	O($$, OP_CREATE_SWPOOL); /* stack: empty write context */
	P($$, $1); /* some code that writes to that context */
	O($$, OP_REDUCE_SWPOOL); /* context=pop; stack: context.get_string() */
};

/* basics */

write_string: STRING {
	// optimized from OP_STRING+OP_WRITE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal($$=$1)
};

empty_double_value: /* empty */ { $$=VL(NEW VDouble(POOL, 0)) };
empty_string_value: /* empty */ { $$=VL(NEW VString(POOL)) };
empty: /* empty */ { $$=N(POOL) };

%%

/*
    	000$111(2222)00 
		000$111{3333}00
    	$,^: push,=0
    	1:( { break=pop
    	2:( )  pop
    	3:{ }  pop

    	000^111(2222)4444{33333}4000
    	$,^: push,=0
    	1:( { break=pop
    	2:( )=4
    	3:{ }=4
		4:[^({]=pop
*/

int yylex(YYSTYPE *lvalp, void *pc) {
	#define lexical_brackets_nestage PC->brackets_nestages[PC->sp]
	#define RC {result=c; goto break2; }

    register int c;
    int result;
	
	if(PC->pending_state) {
		result=PC->pending_state;
		PC->pending_state=0;
		return result;
	}
	
	char *begin=PC->source;
	char *end;
	int begin_line=PC->line;
	int skip_analized=0;
	while(true) {
		c=*(end=(PC->source++));

		if(c=='\n') {
			PC->line++;
			PC->col=0;
		} else
			PC->col++;

		// todo: # in 0+1 column comment

		// escaping: ^^ ^$ ^; ^) ^} ^( ^{ ^"
		if(c=='^') 
			switch(*PC->source) {
			case '^': case '$': case ';':
			case '[': case ']':
			case '{': case '}':
			case '"':
				if(end!=begin) {
					// append piece till ^
					PC->string->APPEND(begin, end-begin, PC->file, begin_line);
				}
				// reset piece 'begin' position & line
				begin=PC->source; // ^
				begin_line=PC->line;
				// skip over ^ and _
				PC->source++;  PC->col++;
				// skip analysis = forced literal
				continue;
			}
		switch(PC->ls) {

		// USER'S = NOT OURS
		case LS_USER:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '@':
				if(PC->col==0+1) {
					push_LS(PC, LS_DEF_NAME);
					RC;
				}
				break;
			}
			break;
			
		// STRING IN EXPRESSION
		case LS_EXPRESSION_STRING:
			switch(c) {
			case '"':
				pop_LS(PC); //"abc".
				RC;
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			}
			break;

		// METHOD DEFINITION
		case LS_DEF_NAME:
			switch(c) {
			case '[':
				PC->ls=LS_DEF_PARAMS;
				RC;
			case '\n':
				PC->ls=LS_DEF_SPECIAL_BODY;
				RC;
			}
			break;

		case LS_DEF_PARAMS:
			switch(c) {
			case ';':
				RC;
			case ']':
				PC->ls=*PC->source=='['?LS_DEF_LOCALS:LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(PC);
				RC;
			}
			break;

		case LS_DEF_LOCALS:
			switch(c) {
			case '[':
			case ';':
				RC;
			case ']':
				PC->ls=LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(PC);
				RC;
			}
			break;

		case LS_DEF_COMMENT:
			if(c=='\n') {
				pop_LS(PC);
				RC;
			}
			break;

		case LS_DEF_SPECIAL_BODY:
			if(c=='\n') {
				switch(*PC->source) {
				case '@': case 0: // end of special_code
					pop_LS(PC);
					break;
				}
				RC;
			}
			break;

		// (EXPRESSION)
		case LS_VAR_ROUND:
		case LS_METHOD_ROUND:
			switch(c) {
			case ')':
				if(--lexical_brackets_nestage==0)
					if(PC->ls==LS_METHOD_ROUND) // method round param ended
						PC->ls=LS_METHOD_AFTER; // look for method end
					else // PC->ls==LS_VAR_ROUND // variable constructor ended
						pop_LS(PC); // return to normal life
				RC;
			case '$':
				push_LS(PC, LS_EXPRESSION_VAR_NAME);				
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '(':
				lexical_brackets_nestage++;
				RC;
			case '-':
				if(*PC->source=='f') { // -f
					skip_analized=1;
					result=FEXISTS;
				} else
					result=c;
				goto break2;
			case '+': case '*': case '/': case '%': 
			case '~':
			case ';':
				RC;
			case '&': case '|':  case '#':
				if(*PC->source==c) { // && ||
					result=c=='#'?LXOR:c=='&'?LAND:LOR;
					skip_analized=1;
				} else
					result=c;
				goto break2;
			case '<': case '>': case '=': case '!': 
				if(*PC->source=='=') { // <= >= == !=
					skip_analized=1;
					switch(c) {
					case '<': result=NLE; break;
					case '>': result=NGE; break;
					case '=': result=NEQ; break;
					case '!': result=NNE; break;
					}
				} else
					result=c;
				goto break2;
			case '"':
				push_LS(PC, LS_EXPRESSION_STRING);
				RC;
			case 'l': case 'g': case 'e': case 'n':
				if(end==begin) // right after whitespace
					switch(*PC->source) {
//					case '?': // ok [and bad cases, yacc would bark at them]
					case 't': // lt gt [et nt]
						result=c=='l'?SLT:c=='g'?SGT:BAD_STRING_COMPARISON_OPERATOR;
						skip_analized=1;
						goto break2;
					case 'e': // le ge ne [ee]
						result=c=='l'?SLE:c=='g'?SGE:c=='n'?SNE:BAD_STRING_COMPARISON_OPERATOR;
						skip_analized=1;
						goto break2;
					case 'q': // eq [lq gq nq]
						result=c=='e'?SEQ:BAD_STRING_COMPARISON_OPERATOR;
						skip_analized=1;
						goto break2;
					}
				break;
			case 'i':
				if(end==begin) // right after whitespace
					if(PC->source[0]=='n') { // in
						skip_analized=1;
						result=IN;
						goto break2;
					}
				break;
			case 'd':
				if(end==begin) // right after whitespace
					if(PC->source[0]=='e' && PC->source[1]=='f') { // def
						skip_analized=2;
						result=DEF;
						goto break2;
					}
				break;
			case ' ': case '\t': case '\n':
				if(end!=begin) { // there were a string after previous operator?
					result=0; // return that string
					goto break2;
				}
				// that's a leading|traling space or after-operator-space
				// ignoring it
				// reset piece 'begin' position & line
				begin=PC->source; // after whitespace char
				begin_line=PC->line;
				continue;
			}
			break;

		// VARIABLE GET/PUT/WITH
		case LS_VAR_NAME_SIMPLE:
		case LS_EXPRESSION_VAR_NAME:
			if(PC->ls==LS_EXPRESSION_VAR_NAME) {
				// name in expr ends also before binary operators 
				switch(c) {
				case '+': case '-': case '*': case '/': case '%': 
				case '&': case '|': 
				case '=': case '!':
					pop_LS(PC);
					PC->source--;  if(--PC->col<0) { PC->line--;  PC->col=-1; }
					result=EON;
					goto break2;
				}
			}
			switch(c) {
			case 0:
			case ' ': case '\t': case '\n':
			case ';':
			case ']': case '}': case ')': case '"':
			case '<': case '>':  // these stand for HTML brackets and expression binary ops
				pop_LS(PC);
				PC->source--;  if(--PC->col<0) { PC->line--;  PC->col=-1; }
				result=EON;
				goto break2;
			case '[':
				PC->ls=LS_VAR_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				if(begin==end) { // ${name}, no need of EON, switching LS
					PC->ls=LS_VAR_NAME_CURLY; 
				} else {
					PC->ls=LS_VAR_CURLY;
					lexical_brackets_nestage=1;
				}

				RC;
			case '(':
				PC->ls=LS_VAR_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
				RC;
			}
			break;

		case LS_VAR_NAME_CURLY:
			switch(c) {
			case '}': // ${name} finished, restoring LS
				pop_LS(PC);
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
				RC;
			}
			break;

		case LS_VAR_SQUARE:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
					RC;
				}
				break;
			case ';': // operator_or_fmt;value delim
				RC;
			case '[':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_VAR_CURLY:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
					RC;
				}
				break;
			case '{':
				lexical_brackets_nestage++;
				break;
			}
			break;

		// METHOD CALL
		case LS_METHOD_NAME:
			switch(c) {
			case '[':
				PC->ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				PC->ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			case '(':
				PC->ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim 
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
				RC;
			}
			break;

		case LS_METHOD_SQUARE:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ';': // param delim
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					PC->ls=LS_METHOD_AFTER;
					RC;
				}
				break;
			case '[':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_METHOD_CURLY:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					PC->ls=LS_METHOD_AFTER;
					RC;
				}
				break;
			case '{':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_METHOD_AFTER:
			if(c=='[') {/* ][ }[ )[ */
				PC->ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='{') {/* ]{ }{ ){ */
				PC->ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='(') {/* ]( }( )( */
				PC->ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			}					   
			pop_LS(PC);
			PC->source--;  if(--PC->col<0) { PC->line--;  PC->col=-1; }
			result=EON;
			goto break2;
		}
		if(c==0) {
			result=-1;
			break;
		}
	}

break2:
	if(end!=begin) { // there is last piece?
		if((c=='@' || c==0) && end[-1]=='\n') { // we are before LS_DEF_NAME or EOF?
			// strip last \n
			end--;
		}
		if(end!=begin) { // last piece still alive?
			// append it
			PC->string->APPEND(begin, end-begin, PC->file, begin_line/*, start_col*/);
		}
	}
	if(PC->string->size()) { // something accumulated?
		// create STRING value: array of OP_VALUE+vstring
		*lvalp=VL(NEW VString(*PC->string));
		// new pieces storage
		PC->string=NEW String(POOL);
		// make current result be pending for next call, return STRING for now
		PC->pending_state=result;  result=STRING;
	}
	if(skip_analized) {
		PC->source+=skip_analized;  PC->col+=skip_analized;
	}
	return result;
}

int real_yyerror(parse_control *pc, char *s)  /* Called by yyparse on error */
     {
       //fprintf(stderr, "[%s]\n", s);

	   strncpy(pc->error, s, MAX_STRING); // TODO: ������������� � ��������� ��������� ������
	   return 1;
     }

static void
     yyprint(
          FILE *file,
          int type,
          YYSTYPE value)
     {
       if(type==STRING)
         fprintf(file, " \"%s\"", SLA2S(value)->cstr());
     }

