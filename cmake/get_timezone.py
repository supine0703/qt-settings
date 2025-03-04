"""
License: MIT
Copyright (c) 2024-2025 李宗霖 (Li Zonglin)
Email: supine0703@outlook.com
GitHub: https://github.com/supine0703
Repository: https://github.com/supine0703/lzl-cpp-lib
"""
# Get the current timezone offset from UTC in the format UTC±HH:MM
import time

offset = -time.timezone if (time.localtime().tm_isdst == 0) else -time.altzone
offset_hours = offset // 3600
offset_minutes = (abs(offset) % 3600) // 60
sign = "+" if offset_hours >= 0 else "-"
print(f"UTC{sign}{abs(offset_hours):02}:{offset_minutes:02}")
