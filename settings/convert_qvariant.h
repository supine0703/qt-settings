#ifndef LZL_QT_UTILS_CONVERT_QVARIANT_H
#define LZL_QT_UTILS_CONVERT_QVARIANT_H

#include <QVariant>

namespace lzl::qt_utils {

// ConvertQVariant template
template <typename T>
struct ConvertQVariant; // Converts the QVariant type to ...

// Different types of template specifier

// Basic types
template <>
struct ConvertQVariant<bool>
{
    static bool convert(const QVariant& value) { return value.toBool(); }
};

template <>
struct ConvertQVariant<int>
{
    static int convert(const QVariant& value) { return value.toInt(); }
};

template <>
struct ConvertQVariant<unsigned int>
{
    static unsigned int convert(const QVariant& value) { return value.toUInt(); }
};

template <>
struct ConvertQVariant<long long>
{
    static long long convert(const QVariant& value) { return value.toLongLong(); }
};

template <>
struct ConvertQVariant<unsigned long long>
{
    static unsigned long long convert(const QVariant& value) { return value.toULongLong(); }
};

template <>
struct ConvertQVariant<float>
{
    static float convert(const QVariant& value) { return value.toFloat(); }
};

template <>
struct ConvertQVariant<double>
{
    static double convert(const QVariant& value) { return value.toDouble(); }
};


// Qt types
template <>
struct ConvertQVariant<QChar>
{
    static QString convert(const QVariant& value) { return value.toChar(); }
};

template <>
struct ConvertQVariant<QString>
{
    static QString convert(const QVariant& value) { return value.toString(); }
};

// more ... ...

} // namespace lzl::qt_utils

#endif // LZL_QT_UTILS_CONVERT_QVARIANT_H
