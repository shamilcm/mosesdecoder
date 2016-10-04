#include <fstream>
#include "GGLM.h"
#include "moses/StaticData.h"
#include "moses/InputFileStream.h"
#include "moses/TranslationOption.h"
#include "moses/TranslationTask.h"
#include "moses/FactorCollection.h"
#include "util/exception.hh"

using namespace std;

namespace Moses {

GenericGlobalLexicalModel::GenericGlobalLexicalModel(const std::string &line) : StatelessFeatureFunction(1, line) {
	cerr << "[FUNC] GenericGlobalLexicalModel::GenericGlobalLexicalModel BEGIN" << endl;
	std::cerr << "Creating global lexical model...\n";
	min_lexicon_log_prob = 1; // Any positive number
	ReadParameters();
	cerr << "[FUNC] GenericGlobalLexicalModel::GenericGlobalLexicalModel END" << endl;
}

/* template <typename T> std::string GenericGlobalLexicalModel::to_string(const T& n) const {
	//cerr << "GenericGlobalLexicalModel::to_string BEGIN" << endl;
	std::ostringstream stm;
	stm << n;
	//cerr << "GenericGlobalLexicalModel::to_string END" << endl;
	return stm.str();
} */

std::string GenericGlobalLexicalModel::concat3(const std::string& token1, const std::string& token2, const int token3) const {
	//cerr << "GenericGlobalLexicalModel::concat3 BEGIN" << endl;
	std::stringstream ss;
	ss << token1 << token2 << token3;
	//cerr << "GenericGlobalLexicalModel::concat3 END" << endl;
	return ss.str();
}

bool GenericGlobalLexicalModel::is_double(const std::string& token) const {
	//cerr << "GenericGlobalLexicalModel::is_double BEGIN" << endl;
	try {
		boost::lexical_cast<double>(token);
		return true;
	} catch (boost::bad_lexical_cast &) {
		return false;
	}
	//cerr << "GenericGlobalLexicalModel::is_double END" << endl;
}

void GenericGlobalLexicalModel::SetParameter(const std::string& key, const std::string& value) {
	cerr << "[FUNC] GenericGlobalLexicalModel::SetParameter BEGIN" << endl;
	if (key == "path") {
		lexicon_probs_path = value;
	} else if (key == "input-factor") {
		m_inputFactorsVec = Tokenize<FactorType>(value,",");
	} else if (key == "output-factor") {
		m_outputFactorsVec = Tokenize<FactorType>(value,",");
	} else if (key == "min_lexicon_prob") {
		UTIL_THROW_IF2(!is_double(value), "[ERROR] Minimum lexicon probability (min_lexicon_prob) must be set to a double value");
		double min_lexicon_prob = Scan<double>(value);
		UTIL_THROW_IF2(min_lexicon_prob <= 0 || min_lexicon_prob > 1, "[ERROR] Minimum lexicon probability (min_lexicon_prob) must be in (0,1]");
		min_lexicon_prob = max(min_lexicon_prob, 1e-300);
		min_lexicon_log_prob = log(min_lexicon_prob);
		cerr << "Minimum lexicon probability is set to: " << min_lexicon_prob << ", log = " << min_lexicon_log_prob << endl;
	} else {
		StatelessFeatureFunction::SetParameter(key, value);
	}
	cerr << "[FUNC] GenericGlobalLexicalModel::SetParameter END" << endl;
}

GenericGlobalLexicalModel::~GenericGlobalLexicalModel() {
}

void GenericGlobalLexicalModel::Load(AllOptions::ptr const& opts) {
	cerr << "[FUNC] GenericGlobalLexicalModel::Load BEGIN" << endl;
	UTIL_THROW_IF2(min_lexicon_log_prob > 0, "[ERROR] Minimum lexicon probability (min_lexicon_prob) is not set in your moses.ini");
	cerr << "Loading lexicon probs: " << lexicon_probs_path << endl;
	//std::ifstream lexicon_probs_file(lexicon_probs_path.c_str());
	InputFileStream lexicon_probs_file(lexicon_probs_path.c_str());
	//UTIL_THROW_IF2(!lexicon_probs_file.is_open(), "[ERROR] Lexicon file cannot be opened: " << lexicon_probs_path);
	std::string line;
	int sentence_id = 0;
	int insert_counter = 0;
	while (std::getline(lexicon_probs_file, line)) {
		std::istringstream iss(line);
		std::string word_str;
		std::string log_prob_str;
		while (iss >> word_str >> log_prob_str) {
			UTIL_THROW_IF2(!is_double(log_prob_str), "[ERROR] Expecting a double for log probailities but got: " << log_prob_str);
			double log_prob = Scan<double>(log_prob_str);
			UTIL_THROW_IF2(log_prob > 0, "[ERROR] Lexicon file should contain log probabibilities but I got: " << log_prob);
			//UTIL_THROW_IF2(log_prob < min_lexicon_log_prob, "[ERROR] Lexicon file log probabibilities should be larger than log(min_lexicon_prob) = "
			//	<< min_lexicon_log_prob << ", but got: " << log_prob << " at line " << (sentence_id+1));
			probs_matrix.insert(std::make_pair<std::string, double>(concat3(word_str, " ", sentence_id), log_prob));
			insert_counter++;
		}
		sentence_id++;
	}
	cerr << "Number of inserts: " << insert_counter << endl;
	cerr << "Number of lines: " << sentence_id << endl;
	cerr << "Hash size: " << probs_matrix.size() << endl;
	//lexicon_probs_file.close();
	cerr << "[FUNC] GenericGlobalLexicalModel::Load END" << endl;
}
 
 void GenericGlobalLexicalModel::InitializeForInput(ttasksptr const& ttask)                                                  
  {                                                                                                                                                                                                                           
    //UTIL_THROW_IF2(ttask->GetSource()->GetType() != SentenceInput,                                                          
    //              "GlobalLexicalModel works only with sentence input.");                                                   
    //Sentence const* s = reinterpret_cast<Sentence const*>(ttask->GetSource().get());  
    InputType const* source = ttask->GetSource().get();                                     
    m_local.reset(new ThreadLocalStorage);                                                                                                               
    m_local->input = source;   
    VERBOSE(1, "[INPUT] " << source << endl);
 	VERBOSE(1, "[FUNC] GenericGlobalLexicalModel::InitializeForInput END" << endl); 
 }

// void GenericGlobalLexicalModel::InitializeForInput(const InputType& source) {
// 	VERBOSE(1, "[FUNC] GenericGlobalLexicalModel::InitializeForInput BEGIN" << endl);
// 	m_local.reset(new ThreadLocalStorage);
// 	m_local->input = &source;
// 	VERBOSE(1, "[INPUT] " << source << endl);
// 	VERBOSE(1, "[FUNC] GenericGlobalLexicalModel::InitializeForInput END" << endl);
// }

double GenericGlobalLexicalModel::get_glm_log_prob_using_source_sentence(const Word& targetWord) const {
	//cerr << "GenericGlobalLexicalModel::get_glm_log_prob_using_source_sentence BEGIN" << endl;
	UTIL_THROW_IF2(1, "[ERROR] GenericGlobalLexicalModel::get_glm_log_prob_using_source_sentence method is not implemented yet.");
	//const Sentence& source = *(m_local->input);
	//cerr << "GenericGlobalLexicalModel::get_glm_log_prob_using_source_sentence END" << endl;
	return 0;
}

double GenericGlobalLexicalModel::get_glm_log_prob_using_source_id(const Word& targetWord) const {
	//cerr << "GenericGlobalLexicalModel::get_glm_log_prob_using_source_id BEGIN" << endl;
	const InputType& source = *(m_local->input);
	//cerr << "SRC: " << source << endl;
	int sentence_id = source.GetTranslationId();
     
	//cerr << "ID: " << sentence_id << endl;
	const std::string word_str = (targetWord[0]->GetString()).as_string();
	//cerr << "Target word: " << targetWord << " [0]: " << targetWord[0] << " [to_string]: " << word_str << " [GetString]: " << targetWord[0]->GetString() << endl;
	boost::unordered_map<std::string, double>::iterator iter;
	//cerr << "ORG: " << word_str << " " << sentence_id << endl;
	//cerr << "CAT: " << concat3(word_str, " ", sentence_id) << endl;
	iter = probs_matrix.find(concat3(word_str, " ", sentence_id));
	if (iter != probs_matrix.end()) {
		//cerr << "Prob1:" << iter->second <<endl;
		return iter->second;
	}
	//cerr << "NOT FOUND => Query: " << word_str << " " << sentence_id << endl;
	//cerr << "GenericGlobalLexicalModel::get_glm_log_prob_using_source_id END" << endl;
	iter = probs_matrix.find(concat3("<unk>", " ", sentence_id));
	if (iter != probs_matrix.end()){
		//cerr << "Prob:" << iter->second << endl;
		return iter->second;
	}
//	cerr << "NOT FOUND => Query: " << word_str << " " << sentence_id << endl;
//	cerr << "GenericGlobalLexicalModel::get_glm_log_prob_using_source_id END" << endl;
	return min_lexicon_log_prob; // The minimum lexicon probability allowed
}

float GenericGlobalLexicalModel::ScoreWord(const Word& targetWord) const {
	//cerr << "GenericGlobalLexicalModel::ScoreWord BEGIN" << endl;
	double log_prob = get_glm_log_prob_using_source_id(targetWord);
	//cerr << "GLM score for word (" << targetWord << ") = " << log_prob << endl;
	//return log_prob;
	//cerr << "GenericGlobalLexicalModel::ScoreWord END" << endl;
	return (float) log_prob;
}

float GenericGlobalLexicalModel::ScorePhrase(const TargetPhrase& targetPhrase) const {
	//cerr << "GenericGlobalLexicalModel::ScorePhrase BEGIN" << endl;
	float score = 0;
	for (size_t targetIndex = 0; targetIndex < targetPhrase.GetSize(); targetIndex++) {
		const Word& targetWord = targetPhrase.GetWord(targetIndex);
		//VERBOSE(1, "glm " << targetWord << ": " << endl);
		score += FloorScore(ScoreWord(targetWord));
	}
	//cerr << "GenericGlobalLexicalModel::ScorePhrase END" << endl;
	return score;
}

float GenericGlobalLexicalModel::GetFromCacheOrScorePhrase(const TargetPhrase& targetPhrase) const {
	//cerr << "GenericGlobalLexicalModel::GetFromCacheOrScorePhrase BEGIN" << endl;
	
	//LexiconCache& m_cache = m_local->cache;
	//const LexiconCache::const_iterator query = m_cache.find(&targetPhrase);
	//if (query != m_cache.end()) {
	//	cerr << "  => [CACHE HIT] phrase (" << targetPhrase << ") = " << query->second << endl;
	//	return query->second;
	//}
	
	stringstream strme;
	for (size_t i = 0; i < targetPhrase.GetSize(); i++) {
		strme << targetPhrase.GetWord(i)[0]->GetString() << " ";
	}
	std::string phrase_str = strme.str();
	//cerr << "### PHRASE STRING ### : " << phrase_str << endl;
	
	PhraseCache& m_ph_cache = m_local->ph_cache;
	const PhraseCache::const_iterator query = m_ph_cache.find(phrase_str);
	if (query != m_ph_cache.end()) {
		//cerr << "  => [*** CACHE HIT ***] phrase (" << phrase_str << ") = " << query->second << " : " << targetPhrase << endl;
		return query->second;
	}
	
	float score = ScorePhrase(targetPhrase);
	//cerr << "  => GLM score for phrase (" << phrase_str << ") = " << score << " : " << targetPhrase << endl;
	
	//m_cache.insert(pair<const TargetPhrase*, float>(&targetPhrase, score));
	m_ph_cache.insert(pair<const std::string, float>(phrase_str, score));
	
	//VERBOSE(1, "add to cache " << targetPhrase << ": " << score << endl);
	//cerr << "GenericGlobalLexicalModel::GetFromCacheOrScorePhrase END" << endl;
	return score;
}

/*
void GenericGlobalLexicalModel::EvaluateInIsolation(const Phrase &source, const TargetPhrase &targetPhrase,
		ScoreComponentCollection &scoreBreakdown, ScoreComponentCollection &estimatedFutureScore) const {
	cerr << "GenericGlobalLexicalModel::EvaluateInIsolation BEGIN" << endl;
	scoreBreakdown.PlusEquals(this, GetFromCacheOrScorePhrase(targetPhrase));
	cerr << "GenericGlobalLexicalModel::EvaluateInIsolation END" << endl;
}
*/

void GenericGlobalLexicalModel::EvaluateWithSourceContext(const InputType &input, const InputPath &inputPath, const TargetPhrase &targetPhrase,
		const StackVec *stackVec, ScoreComponentCollection &scoreBreakdown, ScoreComponentCollection *estimatedFutureScore) const {
	//cerr << "GenericGlobalLexicalModel::EvaluateWithSourceContext BEGIN" << endl;
	scoreBreakdown.PlusEquals(this, GetFromCacheOrScorePhrase(targetPhrase));
	//cerr << "GenericGlobalLexicalModel::EvaluateWithSourceContext END" << endl;
}

bool GenericGlobalLexicalModel::IsUseable(const FactorMask &mask) const {
	cerr << "[FUNC] GenericGlobalLexicalModel::IsUseable BEGIN" << endl;
	for (size_t i = 0; i < m_outputFactors.size(); ++i) {
		if (m_outputFactors[i]) {
			if (!mask[i]) {
				return false;
			}
		}
	}
	cerr << "[FUNC] GenericGlobalLexicalModel::IsUseable END" << endl;
	return true;
}

}
