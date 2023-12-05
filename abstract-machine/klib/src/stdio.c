#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // char buffer[1024];
  // va_list arg;
  // va_start(arg, fmt);
  // int done = vsprintf(buffer, fmt, arg);  // 将格式化的内容(字符串)保存在buffer中
  // putstr(buffer);
  // va_end(arg);void printf(char *format,...)
	char buffer[128];
	int temp = 0;
	va_list arg;

	const char* p_string = fmt;
	char* p_buffer = buffer;
	char* p_temp   = NULL;

	unsigned counter = 0;

	va_start(arg,fmt);

	while(*p_string != '\0')
	{
		if(*p_string == '%')
		{
			p_string++;
			if(*p_string == 'd')//int
			{
				char inttemp[100];
				int intlen = 0;
				temp = va_arg(arg,int);
				if(temp == 0)
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)
				{
					while(temp != 0)
					{
						inttemp[intlen] = temp % 10 + '0';
						intlen++;
						temp /= 10;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
				else if(temp < 0)
				{
					if(temp == -2147483648) // only one special example
					{
						*(p_buffer++) = '-';
						*(p_buffer++) = '2';
						*(p_buffer++) = '1';
						*(p_buffer++) = '4';
						*(p_buffer++) = '7';
						*(p_buffer++) = '4';
						*(p_buffer++) = '8';
						*(p_buffer++) = '3';
						*(p_buffer++) = '6';
						*(p_buffer++) = '4';
						*(p_buffer++) = '8';
						counter += (11);
					}
					else
					{
						temp = -temp;
						while(temp != 0)
						{
							inttemp[intlen] = temp % 10 + '0';
							intlen++;
							temp /= 10;
						}
						counter += (intlen+1);
						intlen--;
						*(p_buffer++) = '-';
						while(intlen!=-1)
						{
							*(p_buffer++) = inttemp[intlen]; 
							intlen--;
						}
					}
				}
			}
			else if(*p_string == 'x')//16
			{
				char inttemp[100];
				int intlen = 0;
				temp = va_arg(arg,int);
				if(temp == 0)
				{
					*(p_buffer++) = '0';
					counter++;
				}
				else if(temp > 0)
				{
					while(temp != 0)
					{
						int r = temp % 16;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						temp /= 16;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
				else
				{
					long long a = 0x100000000;
					a += temp;
					while(a != 0)
					{
						long long r = a % 16;
						if(r < 10)	
							inttemp[intlen] = r + '0';
						else 
							inttemp[intlen] = r - 10 + 'a';
						intlen++;
						a /= 16;
					}
					counter += intlen;
					intlen--;
					while(intlen!=-1)
					{
						*(p_buffer++) = inttemp[intlen]; 
						intlen--;
					}
				}
			}		
			else if(*p_string == 'c')
			{
				temp = va_arg(arg,int);
				*(p_buffer++) = temp;
				counter++;
			}
			else if(*p_string == 's')
			{	
				p_temp = va_arg(arg,char*);
				while(*p_temp != '\0')
				{
					*(p_buffer) = *(p_temp);
					p_buffer++;
					p_temp++;
					counter++;
				}
			}
			p_string++;
		}
		else //not %
		{
			*(p_buffer++) = *(p_string++);
			counter++;
		}
	}

	va_end(arg);

	putstr(buffer);
  // return done;
  return counter;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out,1024, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsprintf(out, fmt, ap);
	va_end(ap);
	return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsnprintf(out, n, fmt, ap);
	va_end(ap);
	return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int count1 = 0,count2 = 0;
  while(*(fmt + count2) != '\0' && count2 < n){
    if(*(fmt + count2) == '%'){
      if(*(fmt + count2 + 1) == 'd'){
        int arg = va_arg(ap,int);
        int temp = arg;
        int digits = 0;
        if(temp == 0){
          digits = 1;
        }else{
          while(temp != 0){
            temp/= 10;
            digits ++;
          }
        }
        if(count1 + digits < n){
          int disvisor = 1;
          for(int i = 0;i<digits -1;i++){
            disvisor *= 10;
          }
          while(disvisor != 0){
            int digit = arg/disvisor;
            *(out + count1) = '0' + digit;
            count1++;
            arg %= disvisor;
            disvisor /= 10;
          }
        }
      }
      else if(*(fmt + count2 + 1) == 's'){
        char *arg = va_arg(ap,char *);
        while(*arg != '\0' && count1 < n){
          *(out + count1) = *arg;
          count1++;
          arg++;
        }
      }
      else if(*(fmt + count2 + 1) == 'c'){
        int arg = va_arg(ap,int);
        *(out + count1) = arg;
        count1++;
      }
      count2 += 2;
    }
    else{
      *(out + count1) = *(fmt + count2);
      count1++;
      count2++;
    } 
  }
  *(out + count1) = '\0';
  return count1;
}

#endif