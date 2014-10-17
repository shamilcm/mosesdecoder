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

class PChart
{
 public:
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

  struct CompressedItem {
    std::size_t end;
    const PVertex *vertex;
  };

  typedef std::vector<std::vector<CompressedItem> > CompressedRow;

  PChart(std::size_t width, bool maintainCompressedChart);

  ~PChart();

  std::size_t GetWidth() const { return cells.size(); }

  const Cell &GetCell(std::size_t start, std::size_t end) const {
    return cells[start][end];
  }

  // Insert the given PVertex and return a reference to the inserted object.
  PVertex &AddVertex(const PVertex &v) {
    Cell &cell = cells[v.span.GetStartPos()][v.span.GetEndPos()];
    if (!v.symbol.IsNonTerminal()) {
      Cell::TMap::value_type x(v.symbol, v);
      std::pair<Cell::TMap::iterator, bool> ret =
          cell.terminalVertices.insert(x);
      return ret.first->second;
    }
    PVertex *p = cell.nonTerminalVertices.Insert(v.symbol, v);
    if (m_compressed) {
      // TODO
    }
    return *p;
  }

  const CompressedRow &GetCompressedRow(std::size_t start) const {
    return (*m_compressed)[start];
  }

 private:
  typedef std::vector<CompressedRow> CompressedChart;

  std::vector<std::vector<Cell> > cells;
  CompressedChart *m_compressed;
};

}  // S2T
}  // Syntax
}  // Moses
