#ifndef SETSTAGE3JOB_H
#define SETSTAGE3JOB_H

#include <Job.h>
#include <QString>

/**
 * @brief A job to write the selected Stage3 tarball name to /etc/calamares.conf
 */
class SetStage3Job : public Calamares::Job
{
public:
    explicit SetStage3Job(const QString& tarballName);

    QString prettyName() const override;
    Calamares::JobResult exec() override;

private:
    QString m_tarballName;
};

#endif // SETSTAGE3JOB_H
