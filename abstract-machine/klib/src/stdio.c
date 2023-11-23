#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  int output_count = 0;
  const char *format_ptr = fmt;

  while (*format_ptr != '\0')
  {
    if (*format_ptr == '%')
    {
      format_ptr++;
      if (*format_ptr == 'd')
      {
        // Handle %d format specifier
        int num = va_arg(args, int);
        char str[20];
        sprintf(str, "%d", num);
        char *str_ptr = str;
        while (*str_ptr != '\0')
        {
          putch(*str_ptr);
          str_ptr++;
          output_count++;
        }
      }
      else if (*format_ptr == 's')
      {
        // Handle %s format specifier
        char *str = va_arg(args, char *);
        char *str_ptr = str;
        while (*str_ptr != '\0')
        {
          putch(*str_ptr);
          str_ptr++;
          output_count++;
        }
      }
    }
    else
    {
      putch(*format_ptr);
      output_count++;
    }

    format_ptr++;
  }

  va_end(args);
  return output_count;
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  return vsnprintf(out, INT32_MAX, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  int ret;
  va_list ap;
  va_start(ap, fmt);
  ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  int count = 0;   // 用于计算写入缓冲区的字符数
  int written = 0; // 用于记录已写入字符数

  while (fmt[count] != '\0' && written < n - 1)
  {
    if (fmt[count] != '%')
    {
      // 普通字符直接复制到输出缓冲区
      out[written] = fmt[count];
      count++;
      written++;
    }
    else
    {
      // 处理格式化指示符
      count++;
      if (fmt[count] == '\0')
      {
        break; // 格式化指示符不完整，退出循环
      }

      if (fmt[count] == '%')
      {
        // 转义的百分号
        out[written] = '%';
        count++;
        written++;
      }
      else if (fmt[count] == 'd')
      {
        // 处理 %d 格式化指示符
        int arg = va_arg(ap, int);
        int digits = snprintf(out + written, n - written, "%d", arg);
        if (digits >= n - written)
        {
          // 输出缓冲区空间不足，退出循环
          break;
        }
        written += digits;
        count++;
      }
      else if (fmt[count] == 's')
      {
        // 处理 %s 格式化指示符
        char *arg = va_arg(ap, char *);
        int str_len = snprintf(out + written, n - written, "%s", arg);
        if (str_len >= n - written)
        {
          // 输出缓冲区空间不足，退出循环
          break;
        }
        written += str_len;
        count++;
      }
      else
      {
        // 不支持的格式化指示符，忽略该指示符
        count++;
      }
    }
  }

  out[written] = '\0'; // 在输出缓冲区末尾添加字符串结束符

  return written;
}

#endif
