#include "ScheduleModel.h"
#include <stdlib.h>
#include <string.h>

static char * _copyString(const char * value) {
	char * copy = calloc(strlen(value) + 1, sizeof(char));
	if (copy != NULL) {
		strcpy(copy, value);
	}
	return copy;
}

ScheduleModel * createScheduleModel(void) {
	return calloc(1, sizeof(ScheduleModel));
}

void destroyScheduleModel(ScheduleModel * model) {
	if (model == NULL) {
		return;
	}

	while (model->assignments != NULL) {
		ScheduleAssignment * next = model->assignments->next;
		free(model->assignments);
		model->assignments = next;
	}
	while (model->availabilities != NULL) {
		ScheduleTeacherWindow * next = model->availabilities->next;
		free(model->availabilities);
		model->availabilities = next;
	}
	while (model->blockedWindows != NULL) {
		ScheduleTeacherWindow * next = model->blockedWindows->next;
		free(model->blockedWindows);
		model->blockedWindows = next;
	}
	while (model->sections != NULL) {
		ScheduleSection * next = model->sections->next;
		free(model->sections->name);
		free(model->sections);
		model->sections = next;
	}
	while (model->courses != NULL) {
		ScheduleCourse * next = model->courses->next;
		free(model->courses->name);
		free(model->courses);
		model->courses = next;
	}
	while (model->rooms != NULL) {
		ScheduleRoom * next = model->rooms->next;
		free(model->rooms->name);
		free(model->rooms);
		model->rooms = next;
	}
	while (model->teachers != NULL) {
		ScheduleTeacher * next = model->teachers->next;
		free(model->teachers->name);
		free(model->teachers);
		model->teachers = next;
	}
	free(model);
}

ScheduleTeacher * findScheduleTeacher(const ScheduleModel * model, const char * name) {
	for (ScheduleTeacher * teacher = model->teachers; teacher != NULL; teacher = teacher->next) {
		if (strcmp(teacher->name, name) == 0) {
			return teacher;
		}
	}
	return NULL;
}

ScheduleRoom * findScheduleRoom(const ScheduleModel * model, const char * name) {
	for (ScheduleRoom * room = model->rooms; room != NULL; room = room->next) {
		if (strcmp(room->name, name) == 0) {
			return room;
		}
	}
	return NULL;
}

ScheduleCourse * findScheduleCourse(const ScheduleModel * model, const char * name) {
	for (ScheduleCourse * course = model->courses; course != NULL; course = course->next) {
		if (strcmp(course->name, name) == 0) {
			return course;
		}
	}
	return NULL;
}

ScheduleSection * findScheduleSection(const ScheduleModel * model, const char * name) {
	for (ScheduleSection * section = model->sections; section != NULL; section = section->next) {
		if (strcmp(section->name, name) == 0) {
			return section;
		}
	}
	return NULL;
}

ScheduleTeacher * addScheduleTeacher(ScheduleModel * model, const char * name) {
	ScheduleTeacher * teacher = calloc(1, sizeof(ScheduleTeacher));
	if (teacher == NULL || (teacher->name = _copyString(name)) == NULL) {
		free(teacher);
		return NULL;
	}
	teacher->next = model->teachers;
	model->teachers = teacher;
	return teacher;
}

ScheduleRoom * addScheduleRoom(ScheduleModel * model, const char * name, int capacity) {
	ScheduleRoom * room = calloc(1, sizeof(ScheduleRoom));
	if (room == NULL || (room->name = _copyString(name)) == NULL) {
		free(room);
		return NULL;
	}
	room->capacity = capacity;
	room->next = model->rooms;
	model->rooms = room;
	return room;
}

ScheduleCourse * addScheduleCourse(ScheduleModel * model, const char * name, int students) {
	ScheduleCourse * course = calloc(1, sizeof(ScheduleCourse));
	if (course == NULL || (course->name = _copyString(name)) == NULL) {
		free(course);
		return NULL;
	}
	course->students = students;
	course->next = model->courses;
	model->courses = course;
	return course;
}

ScheduleSection * addScheduleSection(ScheduleModel * model, const char * name, ScheduleCourse * course) {
	ScheduleSection * section = calloc(1, sizeof(ScheduleSection));
	if (section == NULL || (section->name = _copyString(name)) == NULL) {
		free(section);
		return NULL;
	}
	section->course = course;
	section->next = model->sections;
	model->sections = section;
	return section;
}

ScheduleTeacherWindow * addScheduleTeacherWindow(
	ScheduleTeacherWindow ** windows,
	ScheduleTeacher * teacher,
	Day day,
	int startMinute,
	int endMinute
) {
	ScheduleTeacherWindow * window = calloc(1, sizeof(ScheduleTeacherWindow));
	if (window == NULL) {
		return NULL;
	}
	window->teacher = teacher;
	window->day = day;
	window->startMinute = startMinute;
	window->endMinute = endMinute;
	window->next = *windows;
	*windows = window;
	return window;
}

ScheduleAssignment * addScheduleAssignment(
	ScheduleModel * model,
	ScheduleSection * section,
	ScheduleTeacher * teacher,
	ScheduleRoom * room,
	Day day,
	int startMinute,
	int endMinute
) {
	ScheduleAssignment * assignment = calloc(1, sizeof(ScheduleAssignment));
	if (assignment == NULL) {
		return NULL;
	}
	assignment->section = section;
	assignment->teacher = teacher;
	assignment->room = room;
	assignment->day = day;
	assignment->startMinute = startMinute;
	assignment->endMinute = endMinute;
	assignment->next = model->assignments;
	model->assignments = assignment;
	return assignment;
}

const char * scheduleDayName(Day day) {
	switch (day) {
		case DAY_MONDAY: return "Monday";
		case DAY_TUESDAY: return "Tuesday";
		case DAY_WEDNESDAY: return "Wednesday";
		case DAY_THURSDAY: return "Thursday";
		case DAY_FRIDAY: return "Friday";
		case DAY_SATURDAY: return "Saturday";
		case DAY_SUNDAY: return "Sunday";
		default: return "Unknown";
	}
}
