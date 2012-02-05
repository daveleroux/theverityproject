#include "netanalyzer.h"

//NetAnalyzer::NetAnalyzer() : WhitespaceAnalyzer()
//{

//}

TokenStream* NetAnalyzer::tokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader)
  {
    TokenStream* tokenStream = WhitespaceAnalyzer::tokenStream(fieldName,reader);

//    return new LowerCaseFilter(tokenStream, true);
    return tokenStream;

//    StandardFilter();
  }

