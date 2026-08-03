#include "utility.h"

namespace Utility {

QString formatFileSize(qint64 bytes)
{
    constexpr qint64 kb = 1024;
    constexpr qint64 mb = kb * 1024;
    constexpr qint64 gb = mb * 1024;
    constexpr qint64 tb = gb * 1024;

    if (bytes < kb)
        return QString("%1 bytes").arg(bytes);
    if (bytes < mb)
        return QString::number(bytes / static_cast<double>(kb), 'f', 1) + " KB";
    if (bytes < gb)
        return QString::number(bytes / static_cast<double>(mb), 'f', 1) + " MB";
    if (bytes < tb)
        return QString::number(bytes / static_cast<double>(gb), 'f', 1) + " GB";
    return QString::number(bytes / static_cast<double>(tb), 'f', 1) + " TB";
}

QString formatPermissions(QFileDevice::Permissions permissions)
{
    QString result;
    result += (permissions & QFileDevice::ReadOwner)  ? 'r' : '-';
    result += (permissions & QFileDevice::WriteOwner) ? 'w' : '-';
    result += (permissions & QFileDevice::ExeOwner)   ? 'x' : '-';
    result += (permissions & QFileDevice::ReadGroup)  ? 'r' : '-';
    result += (permissions & QFileDevice::WriteGroup) ? 'w' : '-';
    result += (permissions & QFileDevice::ExeGroup)   ? 'x' : '-';
    result += (permissions & QFileDevice::ReadOther)  ? 'r' : '-';
    result += (permissions & QFileDevice::WriteOther) ? 'w' : '-';
    result += (permissions & QFileDevice::ExeOther)   ? 'x' : '-';
    return result;
}

} // namespace Utility
