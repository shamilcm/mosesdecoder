#ifndef moses_GenericGlobalLexicalModel_h
#define moses_GenericGlobalLexicalModel_h

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "StatelessFeatureFunction.h"
#include "moses/Factor.h"
#include "moses/Phrase.h"
#include "moses/TypeDef.h"
#include "moses/Util.h"
#include "moses/Range.h"
#include "moses/FactorTypeSet.h"
#include "moses/Sentence.h"
#include <boost/lexical_cast.hpp>

#ifdef WITH_THREADS
#include <boost/thread/tss.hpp>
#endif

namespace Moses
{

class Factor;
class Phrase;
class Hypothesis;
class InputType;

/** 
 * 
 * Description
 * 
 */
class GenericGlobalLexicalModel : public StatelessFeatureFunction
{
  boost::unordered_map< const Word*, std::map< const Word*, float, UnorderedComparer<Word> >, UnorderedComparer<Word> > DoubleHash;
  boost::unordered_map< const Word*, float, UnorderedComparer<Word> > SingleHash;
  //typedef std::map< const TargetPhrase*, float > LexiconCache;
  typedef std::map< const std::string, float > PhraseCache;

  struct ThreadLocalStorage {
    //LexiconCache cache;
    PhraseCache ph_cache;
    //const Sentence *input;
    const InputType *input;
  };

private:
  //DoubleHash m_hash;
  mutable boost::unordered_map<std::string, double> probs_matrix;
  double min_lexicon_log_prob;
#ifdef WITH_THREADS
  boost::thread_specific_ptr<ThreadLocalStorage> m_local;
#else
  std::auto_ptr<ThreadLocalStorage> m_local;
#endif
  //Word *m_bias;

  FactorMask m_inputFactors, m_outputFactors;
  std::vector<FactorType> m_inputFactorsVec, m_outputFactorsVec;
  std::string lexicon_probs_path;

  void Load(AllOptions::ptr const& opts);

  float ScorePhrase( const TargetPhrase& targetPhrase ) const;
  float GetFromCacheOrScorePhrase( const TargetPhrase& targetPhrase ) const;
  
  std::string concat3(const std::string& token1, const std::string& token2, const int token3) const;
  bool is_double(const std::string& token) const;
  //template <typename T> std::string to_string(const T& n) const;
  double get_glm_log_prob_using_source_id(const Word& targetWord) const;
  double get_glm_log_prob_using_source_sentence(const Word& targetWord) const;
  float ScoreWord(const Word& targetWord) const;

public:
  GenericGlobalLexicalModel(const std::string &line);
  virtual ~GenericGlobalLexicalModel();

  void SetParameter(const std::string& key, const std::string& value);

  void InitializeForInput(ttasksptr const& ttask);

  bool IsUseable(const FactorMask &mask) const;

  void EvaluateInIsolation(const Phrase &source
                           , const TargetPhrase &targetPhrase
                           , ScoreComponentCollection &scoreBreakdown
                           , ScoreComponentCollection &estimatedFutureScore) const {
  }

  void EvaluateWhenApplied(const Hypothesis& hypo,
                           ScoreComponentCollection* accumulator) const {
  }
  void EvaluateWhenApplied(const ChartHypothesis &hypo,
                           ScoreComponentCollection* accumulator) const {
  }

  void EvaluateWithSourceContext(const InputType &input
                                 , const InputPath &inputPath
                                 , const TargetPhrase &targetPhrase
                                 , const StackVec *stackVec
                                 , ScoreComponentCollection &scoreBreakdown
                                 , ScoreComponentCollection *estimatedFutureScore) const;

  void EvaluateTranslationOptionListWithSourceContext(const InputType &input
      , const TranslationOptionList &translationOptionList) const {
  }

};

}
#endif
