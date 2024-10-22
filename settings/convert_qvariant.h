#ifndef LZL_QT_UTILS_CONVERT_QVARIANT_H
#define LZL_QT_UTILS_CONVERT_QVARIANT_H

#include <QVariant>

namespace lzl::qt_utils {

template <typename T>
struct ConvertQVariant; // Converts the QVariant type to ...

// Different types of template specifier
template <>
struct ConvertQVariant<int>
{
    static int convert(const QVariant& value) { return value.toInt(); }
};

template <>
struct ConvertQVariant<bool>
{
    static bool convert(const QVariant& value) { return value.toBool(); }
};

template <>
struct ConvertQVariant<double>
{
    static double convert(const QVariant& value) { return value.toDouble(); }
};

template <>
struct ConvertQVariant<QString>
{
    static QString convert(const QVariant& value) { return value.toString(); }
};

} // namespace lzl::qt_utils

#endif // LZL_QT_UTILS_CONVERT_QVARIANT_H
