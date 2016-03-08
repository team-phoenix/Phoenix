#include "reducefunctor.h"

using namespace Library;

ReduceFunctor::ReduceFunctor( const Step step )
    : mStep( step ) {

}

void ReduceFunctor::operator()( FileList &mergedList, const FileEntry &entry ) {
    mergedList.append( entry );
}

void ReduceFunctor::operator()( FileList &mergedList, const FileList &givenList ) {

    // Find out if this entry is a dupe
    // If so, remove the dupe and ignore this entry too
    if( mStep == Three ) {

        for( FileEntry entry : givenList ) {
            // qCDebug( phxLibrary ) << "\tLooking for:" << entry.filePath;

            // FIXME: Is this always true?
            // OS X can have a case insensitive file system
#if defined(Q_OS_MACX) || defined(Q_OS_LINUX)
            Qt::CaseSensitivity caseSensitive = Qt::CaseSensitive;
#else
            Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive;
#endif

            QMutableListIterator<FileEntry> mainEntryIterator( mergedList );

            bool dupe = false;

            while( mainEntryIterator.hasNext() ) {
                FileEntry mainEntry = mainEntryIterator.next();

                if( QString::compare( mainEntry.filePath, entry.filePath, caseSensitive ) == 0 ) {
                    // qCDebug( phxLibrary ) << "\tFound .bin file that is part of a .cue file:" << mainEntry.filePath;
                    mainEntryIterator.remove();
                    dupe = true;
                }
            }

            if( !dupe ) {
                mergedList.append( entry );
            }
        }
    } else {
        mergedList.append( givenList );
    }
}

