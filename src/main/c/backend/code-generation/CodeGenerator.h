#ifndef CODE_GENERATOR_HEADER
#define CODE_GENERATOR_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"

ModuleDestructor initializeCodeGeneratorModule(void);

CompilationStatus executeCodeGenerator(CompilerState * compilerState);

#endif
