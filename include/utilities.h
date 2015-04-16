#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>

/* The Utilities class is a global singleton class.
 * This class will probably be merged into the PhoenixGlobals class.
 */

class Utilities {
    public:
        Utilities();
        ~Utilities();

        void createFolder( const QString &path );

};

#endif // UTILITIES_H
