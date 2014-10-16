#pragma once

#include "moses/Syntax/PHyperedge.h"
#include "moses/Syntax/PVertex.h"
#include "moses/Syntax/S2T/Parsers/Parser.h"
#include "moses/Syntax/S2T/RuleTrieCYKPlus.h"
#include "moses/WordsRange.h"

namespace Moses
{
namespace Syntax
{
namespace S2T
{

template<typename Callback>
class RecursiveCYKPlusParser : public Parser<Callback>
{
 public:
  typedef Parser<Callback> Base;
  typedef RuleTrieCYKPlus RuleTrie;

  RecursiveCYKPlusParser(PChart &, const RuleTrie &, std::size_t);

  ~RecursiveCYKPlusParser() {}

  void EnumerateHyperedges(const WordsRange &, Callback &);

 private:

  void GetTerminalExtension(const RuleTrie::Node &, std::size_t, std::size_t);

  void GetNonTerminalExtension(const RuleTrie::Node &, std::size_t,
                               std::size_t);

  void AddAndExtend(const RuleTrie::Node &, std::size_t, PVertex &);

  bool IsNonLexicalUnary(const PHyperedge &) const;

  const RuleTrie &m_ruleTable;
  const std::size_t m_maxChartSpan;
  std::size_t m_maxEnd;
  PHyperedge m_hyperedge;
  Callback *m_callback;
};

}  // namespace S2T
}  // namespace Syntax
}  // namespace Moses

// Implementation
#include "RecursiveCYKPlusParser-inl.h"
