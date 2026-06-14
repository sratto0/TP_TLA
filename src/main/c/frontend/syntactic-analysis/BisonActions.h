#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * EmptyProgramSemanticAction(void);
Program * ExpressionProgramSemanticAction(Expression * expression);

Day DaySemanticAction(Day day);
Program * StatementListProgramSemanticAction(StatementList * statementList);
StatementList * SingleStatementListSemanticAction(Statement * statement);
StatementList * AppendStatementListSemanticAction(StatementList * statementList, Statement * statement);
Statement * AssignmentStatementSemanticAction(char * sectionName, char * teacherName, char * roomName, Day day, TimeRange * timeRange);
Statement * AvailabilityStatementSemanticAction(char * teacherName, Day day, TimeRange * timeRange);
Statement * BlockedStatementSemanticAction(char * teacherName, Day day, TimeRange * timeRange);
Statement * CourseStatementSemanticAction(char * name, int students);
Statement * PrintStatementSemanticAction(void);
Statement * RoomStatementSemanticAction(char * name, int capacity);
Statement * SectionStatementSemanticAction(char * name, char * courseName);
Statement * TeacherStatementSemanticAction(char * name);
Time * TimeSemanticAction(int hour, int minute);
TimeRange * TimeRangeSemanticAction(Time * start, Time * end);

#endif
