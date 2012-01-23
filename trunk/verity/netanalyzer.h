#ifndef NETANALYZER_H
#define NETANALYZER_H

#include "CLucene.h"
#include "CLucene/CLConfig.h"
#include "CLucene/analysis/Analyzers.h"

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;

class NetAnalyzer : public WhitespaceAnalyzer
{
public:
//    NetAnalyzer();
    TokenStream* tokenStream(const TCHAR* fieldName, CL_NS(util)::Reader* reader);

};

#endif // NETANALYZER_H
