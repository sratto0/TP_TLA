#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../support/configuration/Environment.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/Token.h"
#include "../../support/type/TokenLabel.h"
#include "../Frontend.h"

/** Initialize module's internal state. */
ModuleDestructor initializeFlexActionsModule();

/**
 * Flex semantic actions.
 */

CompilationStatus EOFLexemeAction();
CompilationStatus IgnoredLexemeAction();
CompilationStatus IntegerLexemeAction();
CompilationStatus KeywordLexemeAction(TokenLabel label);
CompilationStatus StringLexemeAction();
CompilationStatus UnknownLexemeAction();

#endif
