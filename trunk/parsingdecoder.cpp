#include "parsingdecoder.h"

MorphAttribute ParsingDecoder::nextMorphAttribute(QString type, QString value)
{
    static int index = 0;
    QBitArray bitArray(81);
    bitArray.setBit(index);
    index = index + 1;

    return MorphAttribute(bitArray, type, value);
}

ParsingDecoder::ParsingDecoder()
{
    zero.resize(81);

    QString PART_OF_SPEECH = "Part of Speech";
    QString VOICE = "Voice";
    QString MOOD = "Mood";
    QString TENSE = "Tense";
    QString PERSON = "Person";
    QString CASE = "Case";
    QString GENDER = "Gender";
    QString NUMBER = "Number";
    QString SUFFIX = "Extra Information";
    QString VERB_EXTRA = "Extra Information on Verb";

morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Noun"                                ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Adjective"                           ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Article"                             ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Verb"                                ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Personal Pronoun"                    ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Relative Pronoun"                    ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Reciprocal Pronoun"                  ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Demonstrative Pronoun"               ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Correlative Pronoun"                 ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Interrogative Pronoun"               ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Indefinite Pronoun"                  ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Correlative or Interrogative Pronoun"));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Reflexive Pronoun"                   ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Possessive Pronoun"                  ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Adverb"                              ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Conjunction"                         ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Cond"                                ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Particle"                            ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Preposition"                         ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Interjection"                        ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Aramaic"                             ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Hebrew"                              ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Proper Noun Indeclinable"            ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Numeral Indeclinable"                ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Letter Indeclinable"                 ));
morphAttributes.append(nextMorphAttribute(PART_OF_SPEECH, "Noun Other Type Indeclinable"        ));

morphAttributes.append(nextMorphAttribute(VOICE,"Active"                    ));
morphAttributes.append(nextMorphAttribute(VOICE,"Middle"                    ));
morphAttributes.append(nextMorphAttribute(VOICE,"Passive"                   ));
morphAttributes.append(nextMorphAttribute(VOICE,"Middle Or Passive"         ));
morphAttributes.append(nextMorphAttribute(VOICE,"Middle Deponent"           ));
morphAttributes.append(nextMorphAttribute(VOICE,"Passive Deponent"          ));
morphAttributes.append(nextMorphAttribute(VOICE,"Middle or Passive Deponent"));
morphAttributes.append(nextMorphAttribute(VOICE,"Impersonal Active"         ));
morphAttributes.append(nextMorphAttribute(VOICE,"No Voice"                  ));

morphAttributes.append(nextMorphAttribute(MOOD,"Indicative"                    ));
morphAttributes.append(nextMorphAttribute(MOOD,"Subjunctive"                    ));
morphAttributes.append(nextMorphAttribute(MOOD,"Optative"                   ));
morphAttributes.append(nextMorphAttribute(MOOD,"Imperative"         ));
morphAttributes.append(nextMorphAttribute(MOOD,"Infinitive"           ));
morphAttributes.append(nextMorphAttribute(MOOD,"Participle"          ));
morphAttributes.append(nextMorphAttribute(MOOD,"Imperative Participle"));

morphAttributes.append(nextMorphAttribute(TENSE,"Present"          ));
morphAttributes.append(nextMorphAttribute(TENSE,"Imperfect"        ));
morphAttributes.append(nextMorphAttribute(TENSE,"Future"           ));
morphAttributes.append(nextMorphAttribute(TENSE,"Second Future"    ));
morphAttributes.append(nextMorphAttribute(TENSE,"Aorist"           ));
morphAttributes.append(nextMorphAttribute(TENSE,"Second Aorist"    ));
morphAttributes.append(nextMorphAttribute(TENSE,"Perfect"          ));
morphAttributes.append(nextMorphAttribute(TENSE,"Second Perfect"   ));
morphAttributes.append(nextMorphAttribute(TENSE,"Pluperfect"       ));
morphAttributes.append(nextMorphAttribute(TENSE,"Second Pluperfect"));
morphAttributes.append(nextMorphAttribute(TENSE,"No Tense Stated"  ));

morphAttributes.append(nextMorphAttribute(PERSON, "First Person"));
morphAttributes.append(nextMorphAttribute(PERSON, "Second Person"));
morphAttributes.append(nextMorphAttribute(PERSON, "Third Person"));

morphAttributes.append(nextMorphAttribute(CASE, "Nominative"));
morphAttributes.append(nextMorphAttribute(CASE, "Vocative"));
morphAttributes.append(nextMorphAttribute(CASE, "Genitive"));
morphAttributes.append(nextMorphAttribute(CASE, "Dative"));
morphAttributes.append(nextMorphAttribute(CASE, "Accusative"));

morphAttributes.append(nextMorphAttribute(GENDER, "Masculine"));
morphAttributes.append(nextMorphAttribute(GENDER, "Feminine"));
morphAttributes.append(nextMorphAttribute(GENDER, "Neuter"));

morphAttributes.append(nextMorphAttribute(NUMBER, "Singular"));
morphAttributes.append(nextMorphAttribute(NUMBER, "Plural"));

morphAttributes.append(nextMorphAttribute(SUFFIX, "Superlative"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Comparative"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Abbreviated"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Interrogative"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Negative"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Attic"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Particle Attached"));
morphAttributes.append(nextMorphAttribute(SUFFIX, "Crasis"));

morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Middle Significance"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Contracted Form"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Transitive"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Aeolic"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Attic"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Apocopated Form"));
morphAttributes.append(nextMorphAttribute(VERB_EXTRA, "Irregular or Impure Form"));

}

QList<ParseAttribute> ParsingDecoder::_parse(QBitArray bits)
{
    QList<ParseAttribute> result;

    for(int i=0; i<morphAttributes.size(); i++)
    {
        if((morphAttributes.at(i).bitArray & bits) != zero)
        {
            result.append(ParseAttribute(morphAttributes.at(i).type, morphAttributes.at(i).value));
        }
    }
    return result;
}


 ParsingDecoder& ParsingDecoder::instance()
 {
      static ParsingDecoder singleton;
      return singleton;
 }

QList<ParseAttribute> ParsingDecoder::parse(QBitArray bits)
{
    return instance()._parse(bits);
}

