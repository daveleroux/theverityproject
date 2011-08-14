#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include <QString>

extern QString PROGRAM_NAME;

extern QString MAIN_WINDOW_SETTING_GROUP;
extern QString SIZE_SETTING;
extern QString POS_SETTING;
extern QString WINDOW_STATE_SETTING;
extern QString DATA_PATH_SETTING;

extern QString BIBLE_TEXT_BROWSER_SETTING_GROUP;
extern QString PARALLEL_DISPLAY_SETTINGS_GROUP;
extern QString FONT_FAMILY_SETTINGS;
extern QString ACTIVE_TEXTS;
extern QString TEXT_SETTING;
extern QString FONT_SETTING;

extern QString DATA_PATH;

namespace EventType
{
    enum EventType
    {
        STRONGS,
        WORD_CLICKED,
        PARSING,
    };
}

#endif // GLOBALVARIABLES_H
