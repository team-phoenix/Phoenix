#ifndef REDUCEFUNCTOR_H
#define REDUCEFUNCTOR_H

#include "frontendcommon.h"
#include "logging.h"
#include "librarytypes.h"

namespace Library {

    class ReduceFunctor {
        public:
            enum Step {
                One,
                Two,
                Three,
                Four,
                FourFilter
            };

            ReduceFunctor( const Step step );

            void operator()( FileList &mergedList, const FileList &givenList );
            void operator()( FileList &mergedList, const FileEntry &entry );

        private:
            Step mStep;
    };

}

#endif // REDUCEFUNCTOR_H
