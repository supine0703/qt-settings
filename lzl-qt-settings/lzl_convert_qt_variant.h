/**
 * License: GPL-3.0
 * Copyright (c) 2024 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: qt-settings <https://github.com/supine0703/qt-settings>
 */

#ifndef __LZL_UTILS_CONVERT_QT_VARIANT_H__
#define __LZL_UTILS_CONVERT_QT_VARIANT_H__

#include <QVariant>

namespace lzl::utils {

// ConvertQVariant template
template <typename T>
struct ConvertQVariant; // Converts the QVariant type to ...

// Different types of template specifier

// Basic types
template <>
struct ConvertQVariant<bool>
{
    static auto convert(const QVariant& value) { return value.toBool(); }
};

template <>
struct ConvertQVariant<int>
{
    static auto convert(const QVariant& value) { return value.toInt(); }
};

template <>
struct ConvertQVariant<unsigned int>
{
    static auto convert(const QVariant& value) { return value.toUInt(); }
};

template <>
struct ConvertQVariant<long long>
{
    static auto convert(const QVariant& value) { return value.toLongLong(); }
};

template <>
struct ConvertQVariant<unsigned long long>
{
    static auto convert(const QVariant& value) { return value.toULongLong(); }
};

template <>
struct ConvertQVariant<float>
{
    static auto convert(const QVariant& value) { return value.toFloat(); }
};

template <>
struct ConvertQVariant<double>
{
    static auto convert(const QVariant& value) { return value.toDouble(); }
};

// Qt types
template <>
struct ConvertQVariant<QChar>
{
    static auto convert(const QVariant& value) { return value.toChar(); }
};

template <>
struct ConvertQVariant<QString>
{
    static auto convert(const QVariant& value) { return value.toString(); }
};

template <>
struct ConvertQVariant<const QString&>
{
    static auto convert(const QVariant& value) { return value.toString(); }
};

// more ... ...

} // namespace lzl::utils

#endif // __LZL_QT_UTILS_CONVERT_QT_VARIANT_H__
