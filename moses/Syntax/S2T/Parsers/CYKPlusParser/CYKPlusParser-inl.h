#pragma once

#include "moses/Syntax/S2T/PChart.h"

namespace Moses
{
namespace Syntax
{
namespace S2T
{

template<typename Callback>
CYKPlusParser<Callback>::CYKPlusParser(PChart &chart, const RuleTrie &trie,
                                       std::size_t maxChartSpan)
  : Parser<Callback>(chart)
  , m_ruleTable(trie)
  , m_maxChartSpan(maxChartSpan)
  , m_callback(NULL)
{
  m_hyperedge.head = 0;
}

template<typename Callback>
void CYKPlusParser<Callback>::EnumerateHyperedges(const WordsRange &range,
                                                  Callback &callback)
{
  const std::size_t start = range.GetStartPos();
  const std::size_t end = range.GetEndPos();
  m_callback = &callback;
  const RuleTrie::Node &rootNode = m_ruleTable.GetRootNode();
  m_maxEnd = std::min(Base::m_chart.cells.size()-1, start+m_maxChartSpan-1);
  m_hyperedge.tail.clear();

  // Find all hyperedges where the first incoming vertex is a terminal covering
  // [start,end].
  GetTerminalExtension(rootNode, start, end);

  // Find all hyperedges where the first incoming vertex is a non-terminal
  // covering [start,end-1].
  if (end > start) {
    GetNonTerminalExtension(rootNode, start, end-1);
  }
}

// search all possible nonterminal extensions of a partial rule (pointed at by node) for a given span (StartPos, endPos).
// recursively try to expand partial rules into full rules up to m_lastPos.
template<typename Callback>
void CYKPlusParser<Callback>::GetNonTerminalExtension(
    const RuleTrie::Node &node,
    size_t start,
    size_t end) {

  // target non-terminal labels for the span
  PChart::Cell::NMap &vertexMap =
      Base::m_chart.cells[start][end].nonTerminalVertices;
  if (vertexMap.IsEmpty()) {
    return;
  }

  // non-terminal labels in phrase dictionary node
  const RuleTrie::Node::SymbolMap &nonTermMap = node.GetNonTerminalMap();

  // loop over possible expansions of the rule
  RuleTrie::Node::SymbolMap::const_iterator p;
  RuleTrie::Node::SymbolMap::const_iterator p_end = nonTermMap.end();
  for (p = nonTermMap.begin(); p != p_end; ++p) {
    // does it match possible source and target non-terminals?
    const Word &targetNonTerm = p->first;

    PVertex *q = vertexMap.Find(targetNonTerm);
    if (!q) {
      continue;
    }
    // create new rule
    const RuleTrie::Node &child = p->second;
    AddAndExtend(child, end, *q);
  }
}

// search all possible terminal extensions of a partial rule (pointed at by node) at a given position
// recursively try to expand partial rules into full rules up to m_lastPos.
template<typename Callback>
void CYKPlusParser<Callback>::GetTerminalExtension(
    const RuleTrie::Node &node,
    std::size_t start,
    std::size_t end) {

  PChart::Cell::TMap &vertexMap =
      Base::m_chart.cells[start][end].terminalVertices;
  if (vertexMap.empty()) {
    return;
  }

  for (PChart::Cell::TMap::iterator p = vertexMap.begin();
       p != vertexMap.end(); ++p) {
    const Word &terminal = p->first;
    PVertex &vertex = p->second;

    const RuleTrie::Node::SymbolMap &terminals = node.GetTerminalMap();

    // if node has small number of terminal edges, test word equality for each.
    if (terminals.size() < 5) {
      for (RuleTrie::Node::SymbolMap::const_iterator iter = terminals.begin();
           iter != terminals.end(); ++iter) {
        const Word &word = iter->first;
        if (word == terminal) {
          const RuleTrie::Node *child = & iter->second;
          AddAndExtend(*child, end, vertex);
        }
      }
    }
    // else, do hash lookup
    else {
      const RuleTrie::Node *child = node.GetChild(terminal);
      if (child != NULL) {
        AddAndExtend(*child, end, vertex);
      }
    }
  }
}

// if a (partial) rule matches, add it to list completed rules (if non-unary and non-empty), and try find expansions that have this partial rule as prefix.
template<typename Callback>
void CYKPlusParser<Callback>::AddAndExtend(
    const RuleTrie::Node &node, std::size_t end, PVertex &vertex) {
  m_hyperedge.tail.push_back(&vertex);

  // add target phrase collection (except if rule is empty or unary)
  const TargetPhraseCollection &tpc = node.GetTargetPhraseCollection();
  if (!tpc.IsEmpty() && !IsNonLexicalUnary(m_hyperedge)) {
    m_hyperedge.translations = &tpc;
    (*m_callback)(m_hyperedge, end);
  }

  // get all further extensions of rule (until reaching end of sentence or
  // max-chart-span)
  if (end < m_maxEnd) {
    if (!node.GetTerminalMap().empty()) {
      for (std::size_t newEndPos = end+1; newEndPos <= m_maxEnd; newEndPos++) {
        GetTerminalExtension(node, end+1, newEndPos);
      }
    }
    if (!node.GetNonTerminalMap().empty()) {
      for (std::size_t newEndPos = end+1; newEndPos <= m_maxEnd; newEndPos++) {
        GetNonTerminalExtension(node, end+1, newEndPos);
      }
    }
  }

  m_hyperedge.tail.pop_back();
}

template<typename Callback>
bool CYKPlusParser<Callback>::IsNonLexicalUnary(
    const PHyperedge &hyperedge) const
{
  return hyperedge.tail.size() == 1 &&
         hyperedge.tail[0]->symbol.IsNonTerminal();
}

}  // namespace S2T
}  // namespace Syntax
}  // namespace Moses
