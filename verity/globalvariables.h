#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include <QString>


extern QString PROGRAM_NAME;


//extern QString SIZE_SETTING;
//extern QString POS_SETTING;
//extern QString WINDOW_STATE_SETTING;
//extern QString DATA_PATH_SETTING;


extern QString BIBLE_TEXT_BROWSER_SETTING_GROUP;
extern QString PARALLEL_DISPLAY_SETTINGS_GROUP;
extern QString FONT_FAMILY_SETTINGS;
extern QString ACTIVE_TEXTS;
extern QString TEXT_SETTING;
extern QString FONT_SETTING;
extern QString SEARCH_INDICATOR;
extern QString INDEX_NAME;

extern QString DATA_PATH;


namespace EventType
{
    enum EventType
    {
        STRONGS,
        WORD_CLICKED,
        PARSING,
        NET_NOTE,
        CLOSING,
        BIBLE_REFERENCE,
        SEARCH,
    };
}


#endif // GLOBALVARIABLES_H
