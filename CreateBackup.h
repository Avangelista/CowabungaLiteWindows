#ifndef CREATEBACKUP_H
#define CREATEBACKUP_H

#include <QString>

class CreateBackup
{
public:
    static bool createBackup(const QString& indir, const QString& outdir);
};


#endif // CREATEBACKUP_H
