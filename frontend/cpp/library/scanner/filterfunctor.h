#pragma once

#include "librarytypes.h"

#include <QFileInfo>

namespace Library {

    class FilterFunctor {
        public:
            enum Step {
                // Mark .bin files from main list that came from step 3's output as not needing to be scanned (cheaper than removing?)
                // FIXME: This operation is O( stepThreeOutputCount * mainListCount ) ~= O( n^2 )... bad enough it should be its own step?
                Four,
            };

            FilterFunctor( const Step step );

            bool operator()( const FileEntry &entry );

            // Checks if file is a bios file, if it is the QString passed to this
            // function will be updated with the proper bios name.
            bool isBios( QFileInfo &info, QString &trueBiosName );


        private:
            Step mStep;

            // Maps SHA-1 checksums to a dictionary of metadata (other hashes, filename, system, region) for firmware (BIOS) files
            QHash<QString, QHash<QString, QString>> mFirmwareMap;

    };

}
