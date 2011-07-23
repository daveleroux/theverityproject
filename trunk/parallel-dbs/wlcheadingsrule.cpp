#include "wlcheadingsrule.h"
#include "globalvariables.h"

WlcHeadingsRule::WlcHeadingsRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
//    QStringList greekBookNames;
//    greekBookNames
//            << QString::fromUtf8("ΚΑΤΑ ΜΑΘΘΑΙΟΝ"         )
//            << QString::fromUtf8("ΚΑΤΑ ΜΑΡΚΟΝ"           )
//            << QString::fromUtf8("ΚΑΤΑ ΛΟΥΚΑΝ"           )
//            << QString::fromUtf8("ΚΑΤΑ ΙΩΑΝΝΗΝ"          )
//            << QString::fromUtf8("ΠΡΑΞΕΙΣ ΑΠΟΣΤΟΛΩΝ"     )
//            << QString::fromUtf8("ΠΡΟΣ ΡΩΜΑΙΟΥΣ"         )
//            << QString::fromUtf8("ΠΡΟΣ ΚΟΡΙΝΘΙΟΥΣ Α"     )
//            << QString::fromUtf8("ΠΡΟΣ ΚΟΡΙΝΘΙΟΥΣ Β"     )
//            << QString::fromUtf8("ΠΡΟΣ ΓΑΛΑΤΑΣ"          )
//            << QString::fromUtf8("ΠΡΟΣ ΕΦΕΣΙΟΥΣ"         )
//            << QString::fromUtf8("ΠΡΟΣ ΦΙΛΙΠΠΗΣΙΟΥΣ"     )
//            << QString::fromUtf8("ΠΡΟΣ ΚΟΛΟΣΣΑΕΙΣ"       )
//            << QString::fromUtf8("ΠΡΟΣ ΘΕΣΣΑΛΟΝΙΚΕΙΣ Α"  )
//            << QString::fromUtf8("ΠΡΟΣ ΘΕΣΣΑΛΟΝΙΚΕΙΣ Β"  )
//            << QString::fromUtf8("ΠΡΟΣ ΤΙΜΟΘΕΟΝ Α"       )
//            << QString::fromUtf8("ΠΡΟΣ ΤΙΜΟΘΕΟΝ Β"       )
//            << QString::fromUtf8("ΠΡΟΣ ΤΙΤΟΝ"            )
//            << QString::fromUtf8("ΠΡΟΣ ΦΙΛΗΜΟΝΑ"         )
//            << QString::fromUtf8("ΠΡΟΣ ΕΒΡΑΙΟΥΣ"         )
//            << QString::fromUtf8("ΙΑΚΩΒΟΥ"               )
//            << QString::fromUtf8("ΠΕΤΡΟΥ Α"              )
//            << QString::fromUtf8("ΠΕΤΡΟΥ Β"              )
//            << QString::fromUtf8("ΙΩΑΝΝΟΥ Α"             )
//            << QString::fromUtf8("ΙΩΑΝΝΟΥ Β"             )
//            << QString::fromUtf8("ΙΩΑΝΝΟΥ Γ"             )
//            << QString::fromUtf8("ΙΟΥΔΑ"                 )
//            << QString::fromUtf8("ΑΠΟΚΑΛΥΨΙΣ ΙΩΑΝΝΟΥ"    );



//    for(int i=40; i < 40 + greekBookNames.size(); i++)
//        bookNameForNumber.insert(i, greekBookNames.at(i-40));
}

bool WlcHeadingsRule::applies(VerseReference verseReference)
{
    if(verseReference.chapter == 0 && verseReference.verse == 0)
    {
        return verseReference.textIfLucky.contains("<bookName>");
//        QString standardName = bookNameForNumber.value(verseReference.book);
//        return verseReference.text.contains("<bookName>" + standardName + "</bookName>");
    }
    else
        return false;
}

int WlcHeadingsRule::getSyncNumber(VerseReference wlcVerseReference)
{
    return GlobalsHelper::getParallelForBookNumber(wlcVerseReference.book);
}

