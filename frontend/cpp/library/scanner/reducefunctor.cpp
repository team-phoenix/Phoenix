#include "reducefunctor.h"

using namespace Library;

ReduceFunctor::ReduceFunctor( const Step step )
    : mStep( step ) {

}

void ReduceFunctor::operator()( FileList &mergedList, const FileEntry &entry ) {
    mergedList.append( entry );
}

void ReduceFunctor::operator()( FileList &mergedList, const FileList &givenList ) {
    mergedList.append( givenList );
}

