#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TOpt.h"
#include "discaster/ScriptLexer.h"
#include "discaster/StdInputStream.h"
#include "discaster/LexicalStream.h"
#include "discaster/ScriptParser.h"
#include "discaster/ScriptEvaluator.h"
#include "discaster/Object.h"
#include "discaster/Config.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;
using namespace BlackT;
using namespace Discaster;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "discaster: CD image creation tool" << std::endl;
    std::cout << "Usage: "
      << argv[0] << " <scriptfile> [options]" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "  -w       Suppress warnings" << std::endl;
    std::cout << "  -d       Enable debug output" << std::endl;
    
    return 0;
  }
  
  std::string inputFile = std::string(argv[1]);
  
  if (TOpt::hasFlag(argc, argv, "-w")) config.setWarningsOn(false);
  if (TOpt::hasFlag(argc, argv, "-d")) config.setDebugOutput(true);

  double timer = clock();

  try {
    Discaster::ScriptLexer lexer;
    Discaster::LexicalStream lexStream;
    std::ifstream ifs(inputFile.c_str(), ios_base::binary);
    Discaster::StdInputStream inputStream(ifs);
    
    lexer.lexProgram(lexStream, inputStream);
  //  while (!lexStream.eof()) std::cout << lexStream.get();
    
    ScriptParser parser;
    parser.parseProgram(lexStream);
//    parser.parseTree().prettyPrint(std::cout);

    ScriptEvaluator evaluator;
    evaluator.evaluateProgram(parser.parseTree());
  }
  catch (BlackT::TGenericException& e) {
    std::cout << "Exception:" << std::endl
      << e.problem() << std::endl;
    return 1;
  }
  
  timer = clock() - timer;
  std::cout << "Running time: " << timer/(double)CLOCKS_PER_SEC
    << " sec. " << std::endl;
  
  return 0;
}
