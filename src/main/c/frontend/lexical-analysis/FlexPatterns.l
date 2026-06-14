%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus EOFLexemeAction(void);
extern CompilationStatus IgnoredLexemeAction(void);
extern CompilationStatus IntegerLexemeAction(void);
extern CompilationStatus KeywordLexemeAction(TokenLabel label);
extern CompilationStatus StringLexemeAction(void);
extern CompilationStatus UnknownLexemeAction(void);

%}

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+
integer				[0-9]+
identifier			[[:alpha:]_][[:alnum:]_]*
string				\"([^\"\\]|\\[\"\\nrt])*\"

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"//"[^\n]*							{ return IgnoredLexemeAction(); }

"teacher"							{ return KeywordLexemeAction(TEACHER); }
"room"								{ return KeywordLexemeAction(ROOM); }
"capacity"							{ return KeywordLexemeAction(CAPACITY); }
"course"							{ return KeywordLexemeAction(COURSE); }
"students"							{ return KeywordLexemeAction(STUDENTS); }
"section"							{ return KeywordLexemeAction(SECTION); }
"of"								{ return KeywordLexemeAction(OF); }
"available"						{ return KeywordLexemeAction(AVAILABLE); }
"blocked"							{ return KeywordLexemeAction(BLOCKED); }
"assign"							{ return KeywordLexemeAction(ASSIGN); }
"to"								{ return KeywordLexemeAction(TO); }
"in"								{ return KeywordLexemeAction(IN); }
"at"								{ return KeywordLexemeAction(AT); }
"print"								{ return KeywordLexemeAction(PRINT); }
"schedule"							{ return KeywordLexemeAction(SCHEDULE); }

"monday"							{ return KeywordLexemeAction(MONDAY); }
"tuesday"							{ return KeywordLexemeAction(TUESDAY); }
"wednesday"							{ return KeywordLexemeAction(WEDNESDAY); }
"thursday"							{ return KeywordLexemeAction(THURSDAY); }
"friday"							{ return KeywordLexemeAction(FRIDAY); }
"saturday"							{ return KeywordLexemeAction(SATURDAY); }
"sunday"							{ return KeywordLexemeAction(SUNDAY); }

";"									{ return KeywordLexemeAction(SEMICOLON); }
":"									{ return KeywordLexemeAction(COLON); }
"-"									{ return KeywordLexemeAction(DASH); }

{integer}							{ return IntegerLexemeAction(); }
{string}							{ return StringLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
{identifier}						{ return UnknownLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
