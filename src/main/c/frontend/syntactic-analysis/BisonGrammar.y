%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"
#include <stdlib.h>

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	signed int integer;
	char * string;
	TokenLabel token;

	/** Non-terminals. */

	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { free($$); } <string>

/** Terminals. */
%token <token> ASSIGN
%token <token> AT
%token <token> AVAILABLE
%token <token> BLOCKED
%token <token> CAPACITY
%token <token> COLON
%token <token> COURSE
%token <token> DASH
%token <token> FRIDAY
%token <token> IN
%token <integer> INTEGER
%token <token> MONDAY
%token <token> OF
%token <token> PRINT
%token <token> ROOM
%token <token> SATURDAY
%token <token> SCHEDULE
%token <token> SECTION
%token <token> SEMICOLON
%token <token> STUDENTS
%token <string> STRING
%token <token> SUNDAY
%token <token> TEACHER
%token <token> THURSDAY
%token <token> TO
%token <token> TUESDAY
%token <token> WEDNESDAY

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: statement_list										{ $$ = EmptyProgramSemanticAction(); }
	;

statement_list: statement									{ }
	| statement_list statement								{ }
	;

statement: teacher_declaration								{ }
	| room_declaration										{ }
	| course_declaration									{ }
	| section_declaration									{ }
	| availability_declaration								{ }
	| blocked_declaration									{ }
	| assignment											{ }
	| print_statement										{ }
	;

teacher_declaration: TEACHER string_literal SEMICOLON		{ }
	;

room_declaration: ROOM string_literal CAPACITY INTEGER SEMICOLON	{ }
	;

course_declaration: COURSE string_literal STUDENTS INTEGER SEMICOLON	{ }
	;

section_declaration: SECTION string_literal OF string_literal SEMICOLON	{ }
	;

availability_declaration: AVAILABLE string_literal day time_range SEMICOLON	{ }
	;

blocked_declaration: BLOCKED string_literal day time_range SEMICOLON	{ }
	;

assignment: ASSIGN string_literal TO string_literal IN string_literal AT day time_range SEMICOLON	{ }
	;

print_statement: PRINT SCHEDULE SEMICOLON					{ }
	;

day: MONDAY													{ }
	| TUESDAY												{ }
	| WEDNESDAY												{ }
	| THURSDAY												{ }
	| FRIDAY												{ }
	| SATURDAY												{ }
	| SUNDAY												{ }
	;

time_range: time DASH time									{ }
	;

time: INTEGER COLON INTEGER									{ }
	;

string_literal: STRING										{ free($1); }
	;

%%
