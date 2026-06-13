#include "CodeGenerator.h"
#include "../domain-specific/ScheduleModel.h"
#include <stdio.h>
#include <stdlib.h>

static Logger * _logger = NULL;
static const char * _outputPath = "schedule.html";

static int _compareAssignments(const void * left, const void * right) {
	const ScheduleAssignment * leftAssignment = *(const ScheduleAssignment * const *) left;
	const ScheduleAssignment * rightAssignment = *(const ScheduleAssignment * const *) right;
	if (leftAssignment->day != rightAssignment->day) {
		return leftAssignment->day - rightAssignment->day;
	}
	if (leftAssignment->startMinute != rightAssignment->startMinute) {
		return leftAssignment->startMinute - rightAssignment->startMinute;
	}
	return leftAssignment->endMinute - rightAssignment->endMinute;
}

static void _outputEscapedHtml(FILE * output, const char * value) {
	for (const char * cursor = value; *cursor != '\0'; cursor++) {
		switch (*cursor) {
			case '&':
				fputs("&amp;", output);
				break;
			case '<':
				fputs("&lt;", output);
				break;
			case '>':
				fputs("&gt;", output);
				break;
			case '"':
				fputs("&quot;", output);
				break;
			case '\'':
				fputs("&#39;", output);
				break;
			default:
				fputc(*cursor, output);
				break;
		}
	}
}

static void _outputTime(FILE * output, int minute) {
	fprintf(output, "%02d:%02d", minute / 60, minute % 60);
}

static size_t _countRooms(const ScheduleModel * model) {
	size_t count = 0;
	for (ScheduleRoom * room = model->rooms; room != NULL; room = room->next) {
		count++;
	}
	return count;
}

static size_t _countTeachers(const ScheduleModel * model) {
	size_t count = 0;
	for (ScheduleTeacher * teacher = model->teachers; teacher != NULL; teacher = teacher->next) {
		count++;
	}
	return count;
}

static void _shutdownCodeGeneratorModule(void) {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: CodeGenerator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeCodeGeneratorModule(void) {
	_logger = createLogger("CodeGenerator");
	return _shutdownCodeGeneratorModule;
}

CompilationStatus executeCodeGenerator(CompilerState * compilerState) {
	ScheduleModel * model = compilerState->semanticModel;
	if (!model->printRequested) {
		logDebugging(_logger, "No output was requested.");
		return SUCCEEDED;
	}

	size_t assignmentCount = 0;
	for (ScheduleAssignment * assignment = model->assignments; assignment != NULL; assignment = assignment->next) {
		assignmentCount++;
	}

	ScheduleAssignment ** assignments = NULL;
	if (assignmentCount > 0) {
		assignments = calloc(assignmentCount, sizeof(ScheduleAssignment *));
	}
	if (assignmentCount > 0 && assignments == NULL) {
		logCritical(_logger, "Not enough memory to order the generated schedule.");
		return OUT_OF_MEMORY;
	}
	size_t index = 0;
	for (ScheduleAssignment * assignment = model->assignments; assignment != NULL; assignment = assignment->next) {
		assignments[index++] = assignment;
	}
	if (assignmentCount > 1) {
		qsort(assignments, assignmentCount, sizeof(ScheduleAssignment *), _compareAssignments);
	}

	FILE * output = fopen(_outputPath, "w");
	if (output == NULL) {
		free(assignments);
		logError(_logger, "Cannot create generated artifact: %s.", _outputPath);
		return FAILED;
	}

	fputs(
		"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"  <meta charset=\"UTF-8\">\n"
		"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"  <title>SchedLang Schedule</title>\n"
		"  <style>\n"
		"    :root { font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, \"Segoe UI\", sans-serif; }\n"
		"    * { box-sizing: border-box; }\n"
		"    body { margin: 0; min-height: 100vh; color: #172033; background: #eef2ff; }\n"
		"    .page { width: min(1180px, calc(100% - 2rem)); margin: 0 auto; padding: 2rem 0 3rem; }\n"
		"    .hero { padding: 2.5rem; color: white; border-radius: 24px; background: linear-gradient(135deg, #4338ca, #2563eb 55%, #0891b2); box-shadow: 0 24px 60px rgba(37, 99, 235, 0.24); }\n"
		"    .eyebrow { margin: 0 0 0.5rem; font-size: 0.78rem; font-weight: 800; letter-spacing: 0.14em; text-transform: uppercase; opacity: 0.8; }\n"
		"    h1 { margin: 0; font-size: clamp(2rem, 5vw, 3.4rem); line-height: 1.05; }\n"
		"    .subtitle { margin: 0.9rem 0 0; font-size: 1.05rem; opacity: 0.88; }\n"
		"    .stats { display: grid; grid-template-columns: repeat(3, 1fr); gap: 1rem; margin: 1.25rem 0; }\n"
		"    .stat { padding: 1.15rem 1.25rem; border-radius: 18px; background: white; box-shadow: 0 10px 28px rgba(30, 41, 59, 0.08); }\n"
		"    .stat-value { display: block; color: #3730a3; font-size: 1.8rem; font-weight: 800; }\n"
		"    .stat-label { color: #64748b; font-size: 0.84rem; font-weight: 700; text-transform: uppercase; }\n"
		"    .schedule-card { overflow: hidden; border-radius: 20px; background: white; box-shadow: 0 18px 45px rgba(30, 41, 59, 0.1); }\n"
		"    .table-wrap { overflow-x: auto; }\n"
		"    table { width: 100%; min-width: 850px; border-collapse: collapse; }\n"
		"    th, td { padding: 1rem 1.1rem; text-align: left; border-bottom: 1px solid #e2e8f0; }\n"
		"    th { color: #64748b; background: #f8fafc; font-size: 0.75rem; text-transform: uppercase; }\n"
		"    tbody tr:hover { background: #f8faff; }\n"
		"    .course { color: #1e1b4b; font-weight: 800; }\n"
		"    .muted { color: #64748b; }\n"
		"    .chip { display: inline-block; padding: 0.35rem 0.65rem; color: #3730a3; border-radius: 999px; background: #e0e7ff; font-size: 0.82rem; font-weight: 700; white-space: nowrap; }\n"
		"    .time { color: #0f766e; background: #ccfbf1; }\n"
		"    .occupancy { font-weight: 750; }\n"
		"    .empty { padding: 3rem 1rem; color: #64748b; text-align: center; }\n"
		"    footer { margin-top: 1.25rem; color: #64748b; font-size: 0.82rem; text-align: center; }\n"
		"    @media (max-width: 680px) { .page { width: calc(100% - 1rem); padding-top: 0.5rem; } .hero { padding: 1.7rem; } .stats { grid-template-columns: 1fr; } }\n"
		"  </style>\n"
		"</head>\n"
		"<body>\n"
		"  <main class=\"page\">\n"
		"    <header class=\"hero\">\n"
		"      <p class=\"eyebrow\">SchedLang</p>\n"
		"      <h1>Academic Schedule</h1>\n"
		"      <p class=\"subtitle\">A validated overview of courses, sections, teachers and rooms.</p>\n"
		"    </header>\n",
		output
	);
	fprintf(
		output,
		"    <section class=\"stats\" aria-label=\"Schedule summary\">\n"
		"      <article class=\"stat\"><span class=\"stat-value\">%zu</span><span class=\"stat-label\">Assignments</span></article>\n"
		"      <article class=\"stat\"><span class=\"stat-value\">%zu</span><span class=\"stat-label\">Teachers</span></article>\n"
		"      <article class=\"stat\"><span class=\"stat-value\">%zu</span><span class=\"stat-label\">Rooms</span></article>\n"
		"    </section>\n",
		assignmentCount,
		_countTeachers(model),
		_countRooms(model)
	);
	fputs(
		"    <section class=\"schedule-card\" aria-label=\"Schedule assignments\">\n"
		"      <div class=\"table-wrap\">\n"
		"        <table>\n"
		"          <thead>\n"
		"            <tr><th>Course</th><th>Section</th><th>Room</th><th>Teacher</th><th>Day</th><th>Time</th><th>Occupancy</th></tr>\n"
		"          </thead>\n"
		"          <tbody>\n",
		output
	);

	if (assignmentCount == 0) {
		fputs("            <tr><td class=\"empty\" colspan=\"7\">No assignments were defined for this schedule.</td></tr>\n", output);
	}
	for (index = 0; index < assignmentCount; index++) {
		ScheduleAssignment * assignment = assignments[index];
		fputs("            <tr><td class=\"course\">", output);
		_outputEscapedHtml(output, assignment->section->course->name);
		fputs("</td><td class=\"muted\">", output);
		_outputEscapedHtml(output, assignment->section->name);
		fputs("</td><td>", output);
		_outputEscapedHtml(output, assignment->room->name);
		fputs("</td><td>", output);
		_outputEscapedHtml(output, assignment->teacher->name);
		fputs("</td><td><span class=\"chip\">", output);
		_outputEscapedHtml(output, scheduleDayName(assignment->day));
		fputs("</span></td><td><span class=\"chip time\">", output);
		_outputTime(output, assignment->startMinute);
		fputc('-', output);
		_outputTime(output, assignment->endMinute);
		fprintf(
			output,
			"</span></td><td class=\"occupancy\">%d / %d</td></tr>\n",
			assignment->section->course->students,
			assignment->room->capacity
		);
	}
	fputs(
		"          </tbody>\n"
		"        </table>\n"
		"      </div>\n"
		"    </section>\n"
		"    <footer>Generated by SchedLang after semantic validation.</footer>\n"
		"  </main>\n"
		"</body>\n"
		"</html>\n",
		output
	);
	free(assignments);

	const bool writeFailed = ferror(output);
	const bool closeFailed = fclose(output) != 0;
	if (writeFailed || closeFailed) {
		logError(_logger, "Cannot finish generated artifact: %s.", _outputPath);
		return FAILED;
	}
	logInformation(_logger, "Generated schedule: %s.", _outputPath);
	return SUCCEEDED;
}
