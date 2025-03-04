/**
 * License: GPLv3 LGPLv3
 * Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
 * Email: supine0703@outlook.com
 * GitHub: https://github.com/supine0703
 * Repository: https://github.com/supine0703/qt-settings
 */

#ifndef ___LZL_QT_UTILS__LZL_QT_SETTINGS_EXPORTS_H__
#define ___LZL_QT_UTILS__LZL_QT_SETTINGS_EXPORTS_H__

#include <qcompilerdetection.h>

#if defined(LZL_QT_SETTINGS_SHARED) || !defined(LZL_QT_SETTINGS_STATIC)
    #if defined(LZL_QT_SETTINGS_LIBRARY)
        #define LZL_QT_SETTINGS_EXPORT Q_DECL_EXPORT
    #else
        #define LZL_QT_SETTINGS_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define LZL_QT_SETTINGS_EXPORT
#endif

#endif // ___LZL_QT_UTILS__LZL_QT_SETTINGS_EXPORTS_H__
