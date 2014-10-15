#pragma once

#include <vector>

#include <boost/unordered_map.hpp>

#include "moses/Syntax/NonTerminalMap.h"
#include "moses/Syntax/PVertex.h"
#include "moses/Syntax/SymbolEqualityPred.h"
#include "moses/Syntax/SymbolHasher.h"
#include "moses/Word.h"


namespace Moses
{
namespace Syntax
{
namespace S2T
{

struct PChart
{
  struct Cell
  {
    typedef boost::unordered_map<Word, PVertex, SymbolHasher,
                                 SymbolEqualityPred> TMap;
    typedef NonTerminalMap<PVertex> NMap;
    // Collection of terminal vertices (keyed by terminal symbol).
    TMap terminalVertices;
    // Collection of non-terminal vertices (keyed by non-terminal symbol).
    NMap nonTerminalVertices;
  };
  std::vector<std::vector<Cell> > cells;
};

}  // S2T
}  // Syntax
}  // Moses
