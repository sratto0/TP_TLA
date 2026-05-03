#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyStatementList(program->statements);
		destroyExpression(program->expression);
		free(program);
	}
}

void destroyStatement(Statement * statement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (statement != NULL) {
		switch (statement->type) {
			case ASSIGNMENT_STATEMENT:
				free(statement->assignment.sectionName);
				free(statement->assignment.teacherName);
				free(statement->assignment.roomName);
				destroyTimeRange(statement->assignment.timeRange);
				break;
			case AVAILABILITY_STATEMENT:
			case BLOCKED_STATEMENT:
				free(statement->availability.teacherName);
				destroyTimeRange(statement->availability.timeRange);
				break;
			case COURSE_STATEMENT:
				free(statement->course.name);
				break;
			case PRINT_STATEMENT:
				break;
			case ROOM_STATEMENT:
				free(statement->room.name);
				break;
			case SECTION_STATEMENT:
				free(statement->section.name);
				free(statement->section.courseName);
				break;
			case TEACHER_STATEMENT:
				free(statement->teacher.name);
				break;
		}
		free(statement);
	}
}

void destroyStatementList(StatementList * statementList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (statementList != NULL) {
		StatementList * next = statementList->next;
		destroyStatement(statementList->statement);
		free(statementList);
		statementList = next;
	}
}

void destroyTime(Time * time) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (time != NULL) {
		free(time);
	}
}

void destroyTimeRange(TimeRange * timeRange) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (timeRange != NULL) {
		destroyTime(timeRange->start);
		destroyTime(timeRange->end);
		free(timeRange);
	}
}
