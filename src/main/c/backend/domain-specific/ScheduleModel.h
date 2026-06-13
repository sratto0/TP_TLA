#ifndef SCHEDULE_MODEL_HEADER
#define SCHEDULE_MODEL_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdbool.h>

typedef struct ScheduleModel ScheduleModel;
typedef struct ScheduleAssignment ScheduleAssignment;
typedef struct ScheduleCourse ScheduleCourse;
typedef struct ScheduleRoom ScheduleRoom;
typedef struct ScheduleSection ScheduleSection;
typedef struct ScheduleTeacher ScheduleTeacher;
typedef struct ScheduleTeacherWindow ScheduleTeacherWindow;

struct ScheduleTeacher {
	char * name;
	ScheduleTeacher * next;
};

struct ScheduleRoom {
	char * name;
	int capacity;
	ScheduleRoom * next;
};

struct ScheduleCourse {
	char * name;
	int students;
	ScheduleCourse * next;
};

struct ScheduleSection {
	char * name;
	ScheduleCourse * course;
	ScheduleSection * next;
};

struct ScheduleTeacherWindow {
	ScheduleTeacher * teacher;
	Day day;
	int startMinute;
	int endMinute;
	ScheduleTeacherWindow * next;
};

struct ScheduleAssignment {
	ScheduleSection * section;
	ScheduleTeacher * teacher;
	ScheduleRoom * room;
	Day day;
	int startMinute;
	int endMinute;
	ScheduleAssignment * next;
};

struct ScheduleModel {
	ScheduleTeacher * teachers;
	ScheduleRoom * rooms;
	ScheduleCourse * courses;
	ScheduleSection * sections;
	ScheduleTeacherWindow * availabilities;
	ScheduleTeacherWindow * blockedWindows;
	ScheduleAssignment * assignments;
	bool printRequested;
};

ScheduleModel * createScheduleModel(void);
void destroyScheduleModel(ScheduleModel * model);

ScheduleTeacher * findScheduleTeacher(const ScheduleModel * model, const char * name);
ScheduleRoom * findScheduleRoom(const ScheduleModel * model, const char * name);
ScheduleCourse * findScheduleCourse(const ScheduleModel * model, const char * name);
ScheduleSection * findScheduleSection(const ScheduleModel * model, const char * name);

ScheduleTeacher * addScheduleTeacher(ScheduleModel * model, const char * name);
ScheduleRoom * addScheduleRoom(ScheduleModel * model, const char * name, int capacity);
ScheduleCourse * addScheduleCourse(ScheduleModel * model, const char * name, int students);
ScheduleSection * addScheduleSection(ScheduleModel * model, const char * name, ScheduleCourse * course);
ScheduleTeacherWindow * addScheduleTeacherWindow(
	ScheduleTeacherWindow ** windows,
	ScheduleTeacher * teacher,
	Day day,
	int startMinute,
	int endMinute
);
ScheduleAssignment * addScheduleAssignment(
	ScheduleModel * model,
	ScheduleSection * section,
	ScheduleTeacher * teacher,
	ScheduleRoom * room,
	Day day,
	int startMinute,
	int endMinute
);

const char * scheduleDayName(Day day);

#endif
