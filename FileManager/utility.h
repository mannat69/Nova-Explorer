#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QFileDevice>

namespace Utility {

// "483" -> "483 bytes", "2048" -> "2.0 KB", etc.
QString formatFileSize(qint64 bytes);

// Renders permissions the ls -l way, e.g. "rwxr-xr--".
QString formatPermissions(QFileDevice::Permissions permissions);

} // namespace Utility

#endif // UTILITY_H
