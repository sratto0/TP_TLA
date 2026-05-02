#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static LexicalAnalyzer * _lexicalAnalyzer = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: FlexActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer) {
	_lexicalAnalyzer = lexicalAnalyzer;
	_logger = createLogger("FlexActions");
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	return _shutdownFlexActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logTokenAction(const char * actionName, Token * token);
static const char * _tokenLabelToString(TokenLabel label);

/**
 * Logs a lexical-analyzer action over a token in DEBUGGING level.
 */
static void _logTokenAction(const char * actionName, Token * token) {
	char * _lexeme = escape(token->lexeme);
	logDebugging(_logger, WARNING_COLOR "%s" DEFAULT_COLOR ": Token(context=%d, label=%s(%d), length=%d, lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
		actionName,
		token->context,
		_tokenLabelToString(token->label),
		token->label,
		token->length,
		INFORMATION_COLOR, _lexeme, DEFAULT_COLOR,
		token->line,
		token->semanticValue);
	free(_lexeme);
	_lexeme = NULL;
}

/**
 * Converts a token label to a human-readable name for lexical debugging.
 */
static const char * _tokenLabelToString(TokenLabel label) {
	switch (label) {
		case 0: return "EOF";
		case ASSIGN: return "ASSIGN";
		case AT: return "AT";
		case AVAILABLE: return "AVAILABLE";
		case BLOCKED: return "BLOCKED";
		case CAPACITY: return "CAPACITY";
		case COLON: return "COLON";
		case COURSE: return "COURSE";
		case DASH: return "DASH";
		case FRIDAY: return "FRIDAY";
		case IGNORED: return "IGNORED";
		case IN: return "IN";
		case INTEGER: return "INTEGER";
		case MONDAY: return "MONDAY";
		case OF: return "OF";
		case PRINT: return "PRINT";
		case ROOM: return "ROOM";
		case SATURDAY: return "SATURDAY";
		case SCHEDULE: return "SCHEDULE";
		case SECTION: return "SECTION";
		case SEMICOLON: return "SEMICOLON";
		case STRING: return "STRING";
		case STUDENTS: return "STUDENTS";
		case SUNDAY: return "SUNDAY";
		case TEACHER: return "TEACHER";
		case THURSDAY: return "THURSDAY";
		case TO: return "TO";
		case TUESDAY: return "TUESDAY";
		case UNKNOWN: return "UNKNOWN";
		case WEDNESDAY: return "WEDNESDAY";
		default: return "UNMAPPED";
	}
}

/* PUBLIC FUNCTIONS */

CompilationStatus EOFLexemeAction() {
	CompilationStatus status = IN_PROGRESS;
	Token * token = createToken(_lexicalAnalyzer, 0);
	_logTokenAction(__FUNCTION__, token);
	if (!popInputBuffer(_lexicalAnalyzer)) {
		status = pushToken(_lexicalAnalyzer, token);
		FlexContext context = currentLexicalAnalyzerContext(_lexicalAnalyzer);
		if (0 < context) {
			logError(_logger, "The final context is not closed (context=%d).", context);
			status = FAILED;
		}
	}
	destroyToken(token);
	return status;
}

CompilationStatus IgnoredLexemeAction() {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, IGNORED);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus IntegerLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, INTEGER);
	token->semanticValue->integer = atoi(token->lexeme);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus KeywordLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus StringLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, STRING);
	const int contentLength = token->length - 2;
	char * value = calloc(contentLength + 1, sizeof(char));
	if (value == NULL) {
		destroyToken(token);
		return FAILED;
	}
	strncpy(value, token->lexeme + 1, contentLength);
	token->semanticValue->string = value;
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	/**
	 * The parser now owns "value". It will be released by the grammar action or
	 * by Bison's destructor if a syntax error discards the token.
	 */
	destroyToken(token);
	return status;
}

CompilationStatus UnknownLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, UNKNOWN);
	_logTokenAction(__FUNCTION__, token);
	destroyToken(token);
	return FAILED;
}
