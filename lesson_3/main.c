int counter = 0;
int main()
{
  int *p_int;
  p_int  =&counter;
  while(*p_int<21) /* Value that is stored in adress of p_int */
  {
    ++(*p_int);//++counter;
  }
   p_int = (int *)0x20000002U;
  *p_int = 0xDEADBEEF;
  
  return 0;
}
