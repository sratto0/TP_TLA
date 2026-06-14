#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule(void) {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);
static Statement * _createStatement(StatementType type);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/**
 * Creates a statement node tagged with its concrete syntactic kind.
 */
static Statement * _createStatement(StatementType type) {
	Statement * statement = calloc(1, sizeof(Statement));
	statement->type = type;
	return statement;
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * EmptyProgramSemanticAction(void) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Day DaySemanticAction(Day day) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return day;
}

Program * StatementListProgramSemanticAction(StatementList * statementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->statements = statementList;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

StatementList * SingleStatementListSemanticAction(Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StatementList * statementList = calloc(1, sizeof(StatementList));
	statementList->statement = statement;
	return statementList;
}

StatementList * AppendStatementListSemanticAction(StatementList * statementList, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StatementList * cursor = statementList;
	while (cursor->next != NULL) {
		cursor = cursor->next;
	}
	cursor->next = calloc(1, sizeof(StatementList));
	cursor->next->statement = statement;
	return statementList;
}

Statement * AssignmentStatementSemanticAction(char * sectionName, char * teacherName, char * roomName, Day day, TimeRange * timeRange) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(ASSIGNMENT_STATEMENT);
	statement->assignment.sectionName = sectionName;
	statement->assignment.teacherName = teacherName;
	statement->assignment.roomName = roomName;
	statement->assignment.day = day;
	statement->assignment.timeRange = timeRange;
	return statement;
}

Statement * AvailabilityStatementSemanticAction(char * teacherName, Day day, TimeRange * timeRange) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(AVAILABILITY_STATEMENT);
	statement->availability.teacherName = teacherName;
	statement->availability.day = day;
	statement->availability.timeRange = timeRange;
	return statement;
}

Statement * BlockedStatementSemanticAction(char * teacherName, Day day, TimeRange * timeRange) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(BLOCKED_STATEMENT);
	statement->availability.teacherName = teacherName;
	statement->availability.day = day;
	statement->availability.timeRange = timeRange;
	return statement;
}

Statement * CourseStatementSemanticAction(char * name, int students) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(COURSE_STATEMENT);
	statement->course.name = name;
	statement->course.students = students;
	return statement;
}

Statement * PrintStatementSemanticAction(void) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _createStatement(PRINT_STATEMENT);
}

Statement * RoomStatementSemanticAction(char * name, int capacity) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(ROOM_STATEMENT);
	statement->room.name = name;
	statement->room.capacity = capacity;
	return statement;
}

Statement * SectionStatementSemanticAction(char * name, char * courseName) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(SECTION_STATEMENT);
	statement->section.name = name;
	statement->section.courseName = courseName;
	return statement;
}

Statement * TeacherStatementSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = _createStatement(TEACHER_STATEMENT);
	statement->teacher.name = name;
	return statement;
}

Time * TimeSemanticAction(int hour, int minute) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Time * time = calloc(1, sizeof(Time));
	time->hour = hour;
	time->minute = minute;
	return time;
}

TimeRange * TimeRangeSemanticAction(Time * start, Time * end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TimeRange * timeRange = calloc(1, sizeof(TimeRange));
	timeRange->start = start;
	timeRange->end = end;
	return timeRange;
}
