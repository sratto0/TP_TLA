#include "SemanticAnalyzer.h"
#include "../domain-specific/ScheduleModel.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdbool.h>
#include <string.h>

static Logger * _logger = NULL;

static bool _analyzeAssignment(ScheduleModel * model, const Statement * statement);
static bool _analyzeAvailability(ScheduleModel * model, const Statement * statement, bool blocked);
static bool _analyzeCourse(ScheduleModel * model, const Statement * statement);
static bool _analyzeRoom(ScheduleModel * model, const Statement * statement);
static bool _analyzeSection(ScheduleModel * model, const Statement * statement);
static bool _analyzeStatement(ScheduleModel * model, const Statement * statement);
static bool _analyzeTeacher(ScheduleModel * model, const Statement * statement);
static bool _contains(const ScheduleTeacherWindow * window, Day day, int startMinute, int endMinute);
static bool _hasValidName(const char * entity, const char * name);
static bool _overlaps(Day leftDay, int leftStart, int leftEnd, Day rightDay, int rightStart, int rightEnd);
static bool _timeRangeToMinutes(const TimeRange * timeRange, const char * context, int * startMinute, int * endMinute);

static void _shutdownSemanticAnalyzerModule(void) {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSemanticAnalyzerModule(void) {
	_logger = createLogger("SemanticAnalyzer");
	return _shutdownSemanticAnalyzerModule;
}

static bool _hasValidName(const char * entity, const char * name) {
	if (name[0] != '\0') {
		return true;
	}
	logError(_logger, "%s names cannot be empty.", entity);
	return false;
}

static bool _timeRangeToMinutes(const TimeRange * timeRange, const char * context, int * startMinute, int * endMinute) {
	const Time * start = timeRange->start;
	const Time * end = timeRange->end;
	const bool validStart = 0 <= start->hour && start->hour <= 23 && 0 <= start->minute && start->minute <= 59;
	const bool validEnd = 0 <= end->hour && end->hour <= 23 && 0 <= end->minute && end->minute <= 59;
	if (!validStart || !validEnd) {
		logError(_logger, "%s uses an invalid time. Hours must be 00-23 and minutes 00-59.", context);
		return false;
	}

	*startMinute = start->hour * 60 + start->minute;
	*endMinute = end->hour * 60 + end->minute;
	if (*startMinute >= *endMinute) {
		logError(_logger, "%s must start before it ends.", context);
		return false;
	}
	return true;
}

static bool _overlaps(Day leftDay, int leftStart, int leftEnd, Day rightDay, int rightStart, int rightEnd) {
	return leftDay == rightDay && leftStart < rightEnd && rightStart < leftEnd;
}

static bool _contains(const ScheduleTeacherWindow * window, Day day, int startMinute, int endMinute) {
	return window->day == day && window->startMinute <= startMinute && endMinute <= window->endMinute;
}

static bool _analyzeTeacher(ScheduleModel * model, const Statement * statement) {
	const char * name = statement->teacher.name;
	if (!_hasValidName("Teacher", name)) {
		return false;
	}
	if (findScheduleTeacher(model, name) != NULL) {
		logError(_logger, "Teacher \"%s\" is declared more than once.", name);
		return false;
	}
	if (addScheduleTeacher(model, name) == NULL) {
		logCritical(_logger, "Not enough memory to register teacher \"%s\".", name);
		return false;
	}
	return true;
}

static bool _analyzeRoom(ScheduleModel * model, const Statement * statement) {
	const char * name = statement->room.name;
	if (!_hasValidName("Room", name)) {
		return false;
	}
	if (findScheduleRoom(model, name) != NULL) {
		logError(_logger, "Room \"%s\" is declared more than once.", name);
		return false;
	}
	if (statement->room.capacity <= 0) {
		logError(_logger, "Room \"%s\" must have a positive capacity.", name);
		return false;
	}
	if (addScheduleRoom(model, name, statement->room.capacity) == NULL) {
		logCritical(_logger, "Not enough memory to register room \"%s\".", name);
		return false;
	}
	return true;
}

static bool _analyzeCourse(ScheduleModel * model, const Statement * statement) {
	const char * name = statement->course.name;
	if (!_hasValidName("Course", name)) {
		return false;
	}
	if (findScheduleCourse(model, name) != NULL) {
		logError(_logger, "Course \"%s\" is declared more than once.", name);
		return false;
	}
	if (statement->course.students <= 0) {
		logError(_logger, "Course \"%s\" must have a positive student count.", name);
		return false;
	}
	if (addScheduleCourse(model, name, statement->course.students) == NULL) {
		logCritical(_logger, "Not enough memory to register course \"%s\".", name);
		return false;
	}
	return true;
}

static bool _analyzeSection(ScheduleModel * model, const Statement * statement) {
	const char * name = statement->section.name;
	if (!_hasValidName("Section", name)) {
		return false;
	}
	if (findScheduleSection(model, name) != NULL) {
		logError(_logger, "Section \"%s\" is declared more than once.", name);
		return false;
	}
	ScheduleCourse * course = findScheduleCourse(model, statement->section.courseName);
	if (course == NULL) {
		logError(
			_logger,
			"Section \"%s\" references course \"%s\" before it is declared.",
			name,
			statement->section.courseName
		);
		return false;
	}
	if (addScheduleSection(model, name, course) == NULL) {
		logCritical(_logger, "Not enough memory to register section \"%s\".", name);
		return false;
	}
	return true;
}

static bool _analyzeAvailability(ScheduleModel * model, const Statement * statement, bool blocked) {
	const char * teacherName = statement->availability.teacherName;
	int startMinute;
	int endMinute;
	const char * context = blocked ? "Blocked window" : "Availability";
	if (!_timeRangeToMinutes(statement->availability.timeRange, context, &startMinute, &endMinute)) {
		return false;
	}
	ScheduleTeacher * teacher = findScheduleTeacher(model, teacherName);
	if (teacher == NULL) {
		logError(_logger, "%s references teacher \"%s\" before it is declared.", context, teacherName);
		return false;
	}
	ScheduleTeacherWindow ** windows = blocked ? &model->blockedWindows : &model->availabilities;
	if (addScheduleTeacherWindow(windows, teacher, statement->availability.day, startMinute, endMinute) == NULL) {
		logCritical(_logger, "Not enough memory to register %s for teacher \"%s\".", context, teacherName);
		return false;
	}
	return true;
}

static bool _analyzeAssignment(ScheduleModel * model, const Statement * statement) {
	int startMinute;
	int endMinute;
	if (!_timeRangeToMinutes(statement->assignment.timeRange, "Assignment", &startMinute, &endMinute)) {
		return false;
	}

	ScheduleSection * section = findScheduleSection(model, statement->assignment.sectionName);
	ScheduleTeacher * teacher = findScheduleTeacher(model, statement->assignment.teacherName);
	ScheduleRoom * room = findScheduleRoom(model, statement->assignment.roomName);
	bool valid = true;
	if (section == NULL) {
		logError(_logger, "Assignment references section \"%s\" before it is declared.", statement->assignment.sectionName);
		valid = false;
	}
	if (teacher == NULL) {
		logError(_logger, "Assignment references teacher \"%s\" before it is declared.", statement->assignment.teacherName);
		valid = false;
	}
	if (room == NULL) {
		logError(_logger, "Assignment references room \"%s\" before it is declared.", statement->assignment.roomName);
		valid = false;
	}
	if (!valid) {
		return false;
	}

	if (room->capacity < section->course->students) {
		logError(
			_logger,
			"Room \"%s\" has capacity %d, but section \"%s\" requires %d seats.",
			room->name,
			room->capacity,
			section->name,
			section->course->students
		);
		valid = false;
	}

	bool insideAvailability = false;
	for (ScheduleTeacherWindow * window = model->availabilities; window != NULL; window = window->next) {
		if (window->teacher == teacher && _contains(window, statement->assignment.day, startMinute, endMinute)) {
			insideAvailability = true;
			break;
		}
	}
	if (!insideAvailability) {
		logError(_logger, "Teacher \"%s\" is not available for the complete assignment.", teacher->name);
		valid = false;
	}

	for (ScheduleTeacherWindow * window = model->blockedWindows; window != NULL; window = window->next) {
		if (window->teacher == teacher && _overlaps(
			window->day,
			window->startMinute,
			window->endMinute,
			statement->assignment.day,
			startMinute,
			endMinute
		)) {
			logError(_logger, "Teacher \"%s\" has a blocked window that overlaps the assignment.", teacher->name);
			valid = false;
			break;
		}
	}

	for (ScheduleAssignment * existing = model->assignments; existing != NULL; existing = existing->next) {
		if (!_overlaps(
			existing->day,
			existing->startMinute,
			existing->endMinute,
			statement->assignment.day,
			startMinute,
			endMinute
		)) {
			continue;
		}
		if (existing->teacher == teacher) {
			logError(_logger, "Teacher \"%s\" has overlapping assignments.", teacher->name);
			valid = false;
		}
		if (existing->room == room) {
			logError(_logger, "Room \"%s\" has overlapping assignments.", room->name);
			valid = false;
		}
		if (existing->section == section) {
			logError(_logger, "Section \"%s\" has overlapping assignments.", section->name);
			valid = false;
		}
	}

	if (!valid) {
		return false;
	}
	if (addScheduleAssignment(
		model,
		section,
		teacher,
		room,
		statement->assignment.day,
		startMinute,
		endMinute
	) == NULL) {
		logCritical(_logger, "Not enough memory to register assignment for section \"%s\".", section->name);
		return false;
	}
	return true;
}

static bool _analyzeStatement(ScheduleModel * model, const Statement * statement) {
	switch (statement->type) {
		case TEACHER_STATEMENT:
			return _analyzeTeacher(model, statement);
		case ROOM_STATEMENT:
			return _analyzeRoom(model, statement);
		case COURSE_STATEMENT:
			return _analyzeCourse(model, statement);
		case SECTION_STATEMENT:
			return _analyzeSection(model, statement);
		case AVAILABILITY_STATEMENT:
			return _analyzeAvailability(model, statement, false);
		case BLOCKED_STATEMENT:
			return _analyzeAvailability(model, statement, true);
		case ASSIGNMENT_STATEMENT:
			return _analyzeAssignment(model, statement);
		case PRINT_STATEMENT:
			model->printRequested = true;
			return true;
		default:
			logError(_logger, "Unknown statement type: %d.", statement->type);
			return false;
	}
}

CompilationStatus executeSemanticAnalysis(CompilerState * compilerState) {
	Program * program = compilerState->abstractSyntaxtTree;
	ScheduleModel * model = createScheduleModel();
	if (model == NULL) {
		logCritical(_logger, "Not enough memory to create the schedule.");
		return OUT_OF_MEMORY;
	}

	bool valid = true;
	for (StatementList * cursor = program->statements; cursor != NULL; cursor = cursor->next) {
		if (!_analyzeStatement(model, cursor->statement)) {
			valid = false;
		}
	}
	if (!valid) {
		destroyScheduleModel(model);
		return FAILED;
	}

	compilerState->semanticModel = model;
	logDebugging(_logger, "Semantic analysis is done.");
	return SUCCEEDED;
}
