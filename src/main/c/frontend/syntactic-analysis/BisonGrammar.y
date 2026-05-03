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

	Day day;
	Program * program;
	Statement * statement;
	StatementList * statementList;
	Time * time;
	TimeRange * timeRange;
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
%destructor { destroyStatement($$); } <statement>
%destructor { destroyStatementList($$); } <statementList>
%destructor { destroyTime($$); } <time>
%destructor { destroyTimeRange($$); } <timeRange>

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
%type <day> day
%type <program> program
%type <statement> assignment
%type <statement> availability_declaration
%type <statement> blocked_declaration
%type <statement> course_declaration
%type <statement> print_statement
%type <statement> room_declaration
%type <statement> section_declaration
%type <statement> statement
%type <statementList> statement_list
%type <string> string_literal
%type <statement> teacher_declaration
%type <time> time
%type <timeRange> time_range

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: statement_list										{ $$ = StatementListProgramSemanticAction($1); }
	;

statement_list: statement									{ $$ = SingleStatementListSemanticAction($1); }
	| statement_list statement								{ $$ = AppendStatementListSemanticAction($1, $2); }
	;

statement: teacher_declaration								{ $$ = $1; }
	| room_declaration										{ $$ = $1; }
	| course_declaration									{ $$ = $1; }
	| section_declaration									{ $$ = $1; }
	| availability_declaration								{ $$ = $1; }
	| blocked_declaration									{ $$ = $1; }
	| assignment											{ $$ = $1; }
	| print_statement										{ $$ = $1; }
	;

teacher_declaration: TEACHER string_literal[name] SEMICOLON	{ $$ = TeacherStatementSemanticAction($name); }
	;

room_declaration: ROOM string_literal[name] CAPACITY INTEGER[capacity] SEMICOLON	{ $$ = RoomStatementSemanticAction($name, $capacity); }
	;

course_declaration: COURSE string_literal[name] STUDENTS INTEGER[students] SEMICOLON	{ $$ = CourseStatementSemanticAction($name, $students); }
	;

section_declaration: SECTION string_literal[name] OF string_literal[courseName] SEMICOLON	{ $$ = SectionStatementSemanticAction($name, $courseName); }
	;

availability_declaration: AVAILABLE string_literal[teacherName] day[weekday] time_range[timeRange] SEMICOLON	{ $$ = AvailabilityStatementSemanticAction($teacherName, $weekday, $timeRange); }
	;

blocked_declaration: BLOCKED string_literal[teacherName] day[weekday] time_range[timeRange] SEMICOLON	{ $$ = BlockedStatementSemanticAction($teacherName, $weekday, $timeRange); }
	;

assignment: ASSIGN string_literal[sectionName] TO string_literal[teacherName] IN string_literal[roomName] AT day[weekday] time_range[timeRange] SEMICOLON	{ $$ = AssignmentStatementSemanticAction($sectionName, $teacherName, $roomName, $weekday, $timeRange); }
	;

print_statement: PRINT SCHEDULE SEMICOLON					{ $$ = PrintStatementSemanticAction(); }
	;

day: MONDAY													{ $$ = DaySemanticAction(DAY_MONDAY); }
	| TUESDAY												{ $$ = DaySemanticAction(DAY_TUESDAY); }
	| WEDNESDAY												{ $$ = DaySemanticAction(DAY_WEDNESDAY); }
	| THURSDAY												{ $$ = DaySemanticAction(DAY_THURSDAY); }
	| FRIDAY												{ $$ = DaySemanticAction(DAY_FRIDAY); }
	| SATURDAY												{ $$ = DaySemanticAction(DAY_SATURDAY); }
	| SUNDAY												{ $$ = DaySemanticAction(DAY_SUNDAY); }
	;

time_range: time[start] DASH time[end]						{ $$ = TimeRangeSemanticAction($start, $end); }
	;

time: INTEGER[hour] COLON INTEGER[minute]					{ $$ = TimeSemanticAction($hour, $minute); }
	;

string_literal: STRING										{ $$ = $1; }
	;

%%
