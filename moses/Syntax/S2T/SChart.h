#pragma once

#include <vector>

#include <boost/unordered_map.hpp>

#include "moses/Syntax/NonTerminalMap.h"
#include "moses/Syntax/SVertexBeam.h"
#include "moses/Syntax/SymbolEqualityPred.h"
#include "moses/Syntax/SymbolHasher.h"
#include "moses/Word.h"

namespace Moses
{
namespace Syntax
{
namespace S2T
{

struct SChart
{
  struct Cell
  {
    typedef boost::unordered_map<Word, SVertexBeam, SymbolHasher,
                                 SymbolEqualityPred> TMap;
    typedef NonTerminalMap<SVertexBeam> NMap;
    TMap terminalBeams;
    NMap nonTerminalBeams;
  };
  std::vector<std::vector<Cell> > cells;
};

}  // S2T
}  // Syntax
}  // Moses
