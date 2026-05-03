#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum Day Day;
typedef enum StatementType StatementType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
typedef struct Statement Statement;
typedef struct StatementList StatementList;
typedef struct Time Time;
typedef struct TimeRange TimeRange;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum Day {
	DAY_MONDAY,
	DAY_TUESDAY,
	DAY_WEDNESDAY,
	DAY_THURSDAY,
	DAY_FRIDAY,
	DAY_SATURDAY,
	DAY_SUNDAY
};

enum StatementType {
	ASSIGNMENT_STATEMENT,
	AVAILABILITY_STATEMENT,
	BLOCKED_STATEMENT,
	COURSE_STATEMENT,
	PRINT_STATEMENT,
	ROOM_STATEMENT,
	SECTION_STATEMENT,
	TEACHER_STATEMENT
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Time {
	int hour;
	int minute;
};

struct TimeRange {
	Time * start;
	Time * end;
};

struct Statement {
	union {
		struct {
			char * sectionName;
			char * teacherName;
			char * roomName;
			Day day;
			TimeRange * timeRange;
		} assignment;
		struct {
			char * teacherName;
			Day day;
			TimeRange * timeRange;
		} availability;
		struct {
			char * name;
			int students;
		} course;
		struct {
			char * name;
			int capacity;
		} room;
		struct {
			char * name;
			char * courseName;
		} section;
		struct {
			char * name;
		} teacher;
	};
	StatementType type;
};

struct StatementList {
	Statement * statement;
	StatementList * next;
};

struct Program {
	StatementList * statements;

	/**
	 * Legacy calculator field kept while the backend is replaced by SchedLang.
	 */
	Expression * expression;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProgram(Program * program);
void destroyStatement(Statement * statement);
void destroyStatementList(StatementList * statementList);
void destroyTime(Time * time);
void destroyTimeRange(TimeRange * timeRange);

#endif
